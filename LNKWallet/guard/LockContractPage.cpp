#include "LockContractPage.h"
#include "ui_LockContractPage.h"

#include "wallet.h"
#include "control/AssetIconItem.h"
#include "ToolButtonWidget.h"
#include "LockFundDialog.h"
#include "LockContractWithdrawDialog.h"
#include "showcontentdialog.h"


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

    ui->lockFundTableWidget->setColumnWidth(0,160);
    ui->lockFundTableWidget->setColumnWidth(1,140);
    ui->lockFundTableWidget->setColumnWidth(2,180);
    ui->lockFundTableWidget->setColumnWidth(3,150);
    ui->lockFundTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->lockBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    init();
}

LockContractPage::~LockContractPage()
{
    delete ui;
}

void LockContractPage::init()
{
    ui->accountComboBox->clear();
    ui->accountComboBox->addItem(tr("ALL"));
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    if(accounts.size() > 0)
    {
        ui->accountComboBox->addItems(accounts);
    }

    ui->contractAddressLabel->setText(LOCKFUND_CONTRACT_ADDRESS);

    ui->accountComboBox->setCurrentIndex(0);
}

void LockContractPage::refresh()
{
//    on_accountComboBox_currentIndexChanged(ui->accountComboBox->currentText());
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

            ui->lockFundTableWidget->horizontalHeaderItem(3)->setText("");
            ui->lockFundTableWidget->setRowCount(0);
            ui->lockFundTableWidget->setRowCount(keys.size());
            for(int i = 0; i < keys.size(); i++)
            {
                ui->lockFundTableWidget->setRowHeight(i,40);
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

        return;
    }

    if( id == "LockContractPage+invoke_contract_offline+getUsers")
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

            ui->lockFundTableWidget->horizontalHeaderItem(3)->setText(tr("ACCOUNT"));
            ui->lockFundTableWidget->setRowCount(0);
            int rowCount = 0;       // 如果一个地址投票了多个币种  分成多行显示
            int row = 0;
            for(int i = 0; i < keys.size(); i++)
            {
                QString number = keys.at(i);
                QJsonObject object = QJsonDocument::fromJson( resultObject.value(number).toString().toLatin1()).object();
                QStringList keys2 = object.keys();
                QString address = keys2.at(0);
                QJsonObject object2 = object.value(address).toObject();
                QStringList keys3 = object2.keys();
                rowCount += keys3.size();
                ui->lockFundTableWidget->setRowCount(rowCount);
qDebug() << "33333333333333 " << rowCount ;

                for(int j = 0; j < keys3.size(); j++)
                {
                    ui->lockFundTableWidget->setRowHeight(row,40);

                    QString symbol = keys3.at(j);
                    QJsonObject object3 = object2.value(symbol).toObject();
                    qDebug() << "44444444444 " << symbol << i << j ;

                    unsigned long long amount = jsonValueToULL( object3.value("amount"));
                    int depositTime = object3.value("despositTime").toInt();

                    ui->lockFundTableWidget->setItem(row, 0, new QTableWidgetItem(symbol));
                    AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value( HXChain::getInstance()->getAssetId(symbol));
                    ui->lockFundTableWidget->setItem(row, 1, new QTableWidgetItem( getBigNumberString(amount,assetInfo.precision)));
                    ui->lockFundTableWidget->item(row,1)->setData(Qt::UserRole,amount);

                    QDateTime utc;
                    utc.setSecsSinceEpoch(depositTime + 14 * 24 * 3600);
                    qDebug()  << depositTime << utc << utc.toLocalTime() << utc.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
                    ui->lockFundTableWidget->setItem(row, 2, new QTableWidgetItem(utc.toLocalTime().toString("yyyy-MM-dd hh:mm:ss")));

                    QString account = address;
                    if(!HXChain::getInstance()->addressToName(address).isEmpty())
                    {
                        account = HXChain::getInstance()->addressToName(address);
                    }
                    else if(!HXChain::getInstance()->guardOrCitizenAddressToName(address).isEmpty())
                    {
                        account = HXChain::getInstance()->guardOrCitizenAddressToName(address);
                    }
                    ui->lockFundTableWidget->setItem(row, 3, new QTableWidgetItem(account));

                    AssetIconItem* assetIconItem = new AssetIconItem();
                    assetIconItem->setAsset(ui->lockFundTableWidget->item(row,0)->text());
                    ui->lockFundTableWidget->setCellWidget(row, 0, assetIconItem);

                    row++;
                }
            }

            tableWidgetSetItemZebraColor(ui->lockFundTableWidget);
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
    if(accountName.isEmpty() || accountName == tr("ALL"))   return;
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(accountName);
    HXChain::getInstance()->postRPC( "LockContractPage+invoke_contract_offline+getUser+" + accountName, toJsonFormat( "invoke_contract_offline",
                                                                           QJsonArray() << accountName << LOCKFUND_CONTRACT_ADDRESS
                                                                           << "getUser"  << accountInfo.address));
}

void LockContractPage::getUsers()
{
    HXChain::getInstance()->postRPC( "LockContractPage+invoke_contract_offline+getUsers", toJsonFormat( "invoke_contract_offline",
                                                                           QJsonArray() << HXChain::getInstance()->accountInfoMap.keys().first()
                                                                           << LOCKFUND_CONTRACT_ADDRESS
                                                                           << "getUsers"  << "1000,0"));

}

void LockContractPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->lockFundTableWidget->setRowCount(0);
    if(arg1 == tr("ALL"))
    {
        getUsers();
    }
    else
    {
        getUserLockInfo(arg1);
    }
}

void LockContractPage::on_lockFundTableWidget_cellClicked(int row, int column)
{
    if(column == 3)
    {
        if(ui->lockFundTableWidget->item(row,0) && ui->lockFundTableWidget->item(row,1))
        {
            if(ui->accountComboBox->currentIndex() == 0)
            {
                ShowContentDialog showContentDialog( ui->lockFundTableWidget->item(row, column)->text(),this);

                int x = ui->lockFundTableWidget->columnViewportPosition(column) + ui->lockFundTableWidget->columnWidth(column) / 2
                        - showContentDialog.width() / 2;
                int y = ui->lockFundTableWidget->rowViewportPosition(row) - 10 + ui->lockFundTableWidget->horizontalHeader()->height();

                showContentDialog.move( ui->lockFundTableWidget->mapToGlobal( QPoint(x, y)));
                showContentDialog.exec();
            }
            else
            {
                LockContractWithdrawDialog lockContractWithdrawDialog(ui->accountComboBox->currentText(),
                                                                      ui->lockFundTableWidget->item(row,0)->text());
                lockContractWithdrawDialog.setMaxAmount(ui->lockFundTableWidget->item(row,1)->data(Qt::UserRole).toULongLong());
                lockContractWithdrawDialog.pop();
            }

        }

        return;
    }

}

void LockContractPage::on_lockBtn_clicked()
{
    LockFundDialog lockFundDialog;
    lockFundDialog.pop();
}
