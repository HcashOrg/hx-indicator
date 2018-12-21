#include "LockContractPage.h"
#include "ui_LockContractPage.h"

#include "wallet.h"
#include "control/AssetIconItem.h"
#include "ToolButtonWidget.h"
#include "LockFundDialog.h"
#include "LockContractWithdrawDialog.h"

LockContractPage::LockContractPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LockContractPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->lockFundTableWidget->installEventFilter(this);
    ui->lockFundTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->lockFundTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->lockFundTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->lockFundTableWidget->setFrameShape(QFrame::NoFrame);
    ui->lockFundTableWidget->setMouseTracking(true);
    ui->lockFundTableWidget->setShowGrid(false);//隐藏表格线

    ui->lockFundTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->lockFundTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->lockFundTableWidget->horizontalHeader()->setVisible(true);
    ui->lockFundTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->lockFundTableWidget->setColumnWidth(0,180);
    ui->lockFundTableWidget->setColumnWidth(1,120);
    ui->lockFundTableWidget->setColumnWidth(2,180);
    ui->lockFundTableWidget->setColumnWidth(3,150);
    ui->lockFundTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->lockBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->withdrawBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    init();
}

LockContractPage::~LockContractPage()
{
    delete ui;
}

void LockContractPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    if(accounts.size() > 0)
    {
        ui->accountComboBox->addItems(accounts);

        if(accounts.contains(HXChain::getInstance()->currentAccount))
        {
            ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
        }
    }
    else
    {
        ui->label->hide();
        ui->accountComboBox->hide();

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->label->pos(), QSize(300,18)));
        label->setText(tr("There are no accounts in the wallet."));
    }

    ui->contractAddressLabel->setText(LOCKFUND_CONTRACT_ADDRESS);
    getUserLockInfo(ui->accountComboBox->currentText());
}

void LockContractPage::refresh()
{
    getUserLockInfo(ui->accountComboBox->currentText());
}

void LockContractPage::jsonDataUpdated(QString id)
{
    if( id == "LockContractPage+invoke_contract_offline+getUser+" + ui->accountComboBox->currentText())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QString resultStr = parse_doucment.object().value("result").toString();
            QJsonObject resultObject = QJsonDocument::fromJson(resultStr.toLatin1()).object();
            QStringList keys = resultObject.keys();

            ui->lockFundTableWidget->setRowCount(0);
            ui->lockFundTableWidget->setRowCount(keys.size());
            for(int i = 0; i < keys.size(); i++)
            {
                QString key = keys.at(i);
                QJsonObject object = resultObject.value(key).toObject();

                unsigned long long amount = jsonValueToULL( object.value("amount"));
                int depositTime = object.value("despositTime").toInt();
                bool isWithdrawable = object.value("isWithdrawable").toBool();

                ui->lockFundTableWidget->setItem(i, 0, new QTableWidgetItem(key));

                AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value( HXChain::getInstance()->getAssetId(key));
                ui->lockFundTableWidget->setItem(i, 1, new QTableWidgetItem( getBigNumberString(amount,assetInfo.precision)));
                ui->lockFundTableWidget->item(i,1)->setData(Qt::UserRole,amount);

                QDateTime utc;
                utc.setSecsSinceEpoch(depositTime + 14 * 24 * 3600);
                qDebug()  << depositTime << utc << utc.toLocalTime() << utc.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
                ui->lockFundTableWidget->setItem(i, 2, new QTableWidgetItem(utc.toLocalTime().toString("yyyy-MM-dd hh:mm:ss")));
                ui->lockFundTableWidget->setItem(i, 3, new QTableWidgetItem(tr("withdraw")));

                AssetIconItem* assetIconItem = new AssetIconItem();
                assetIconItem->setAsset(ui->lockFundTableWidget->item(i,0)->text());
                ui->lockFundTableWidget->setCellWidget(i, 0, assetIconItem);

                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->lockFundTableWidget->item(i,3)->text());
                toolButton->setEnabled(isWithdrawable);
                ui->lockFundTableWidget->setCellWidget(i,3,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&LockContractPage::on_lockFundTableWidget_cellClicked,this,i,3));
            }

            tableWidgetSetItemZebraColor(ui->lockFundTableWidget);

        }
        else
        {

        }

        return;
    }
}

void LockContractPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void LockContractPage::getUserLockInfo(QString accountName)
{
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(accountName);
    HXChain::getInstance()->postRPC( "LockContractPage+invoke_contract_offline+getUser+" + accountName, toJsonFormat( "invoke_contract_offline",
                                                                           QJsonArray() << accountName << LOCKFUND_CONTRACT_ADDRESS
                                                                           << "getUser"  << accountInfo.address));
}

void LockContractPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->lockFundTableWidget->setRowCount(0);
    getUserLockInfo(arg1);
}

void LockContractPage::on_lockFundTableWidget_cellClicked(int row, int column)
{
    if(column == 3)
    {
        if(ui->lockFundTableWidget->item(row,0) && ui->lockFundTableWidget->item(row,1))
        {
            LockContractWithdrawDialog lockContractWithdrawDialog(ui->accountComboBox->currentText(),
                                                                  ui->lockFundTableWidget->item(row,0)->text());
            lockContractWithdrawDialog.setMaxAmount(ui->lockFundTableWidget->item(row,1)->data(Qt::UserRole).toULongLong());
            lockContractWithdrawDialog.pop();
        }

        return;
    }

}

void LockContractPage::on_lockBtn_clicked()
{
    LockFundDialog lockFundDialog;
    lockFundDialog.pop();
}
