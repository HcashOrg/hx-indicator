#include "BonusPage.h"
#include "ui_BonusPage.h"

#include "wallet.h"
#include "control/AssetIconItem.h"
#include "ToolButtonWidget.h"
#include "depositpage/FeeChargeWidget.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"


static const int ROWNUMBER = 7;
BonusPage::BonusPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BonusPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->bonusTableWidget->installEventFilter(this);
    ui->bonusTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->bonusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->bonusTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->bonusTableWidget->setFrameShape(QFrame::NoFrame);
    ui->bonusTableWidget->setMouseTracking(true);
    ui->bonusTableWidget->setShowGrid(false);//隐藏表格线

    ui->bonusTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->bonusTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->bonusTableWidget->horizontalHeader()->setVisible(true);
    ui->bonusTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->bonusTableWidget->setColumnWidth(0,140);
    ui->bonusTableWidget->setColumnWidth(1,230);
    ui->bonusTableWidget->setColumnWidth(2,120);
    ui->bonusTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->bonusTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->bonusTableWidget);

    ui->obtainAllBtn->setStyleSheet(TOOLBUTTON_STYLE_1);


    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&BonusPage::pageChangeSlot);

    blankWidget = new BlankDefaultWidget(ui->bonusTableWidget);
    blankWidget->setTextTip(tr("当前没有纪录!"));
    init();
}

BonusPage::~BonusPage()
{
    delete ui;
}

void BonusPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    checkObtainAllBtnVisible();
    inited = true;
}

void BonusPage::refresh()
{
    fetchBonusBalance();
}

void BonusPage::jsonDataUpdated(QString id)
{
    if( id == "id-get_bonus_balance-" + ui->accountComboBox->currentText())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.take("result").toArray();
        int size = array.size();
        ui->bonusTableWidget->setRowCount(0);
        ui->bonusTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            ui->bonusTableWidget->setRowHeight(i,40);

            QJsonArray amountArray = array.at(i).toArray();
            QString assetSymbol = amountArray.at(0).toString();
            unsigned long long bonusAmount = jsonValueToULL( amountArray.at(1));

            ui->bonusTableWidget->setItem(i,0,new QTableWidgetItem(assetSymbol));

            AssetIconItem* assetIconItem = new AssetIconItem();
            assetIconItem->setAsset(ui->bonusTableWidget->item(i,0)->text());
            ui->bonusTableWidget->setCellWidget(i, 0, assetIconItem);

            AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(assetSymbol));
            ui->bonusTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(bonusAmount,assetInfo.precision)));
            ui->bonusTableWidget->item(i,1)->setData(Qt::UserRole, QString::number(bonusAmount));

            if(bonusAmount > 0)
            {
                ui->bonusTableWidget->setItem(i,2,new QTableWidgetItem(tr("get bonus")));

                ToolButtonWidget *tbw = new ToolButtonWidget();
                tbw->setText(ui->bonusTableWidget->item(i,2)->text());
                tbw->setButtonFixSize(120,40);
                ui->bonusTableWidget->setCellWidget(i,2,tbw);
                connect(tbw,&ToolButtonWidget::clicked,std::bind(&BonusPage::on_bonusTableWidget_cellPressed,this,i,2));
            }
            else
            {
                ui->bonusTableWidget->setItem(i,2,new QTableWidgetItem(""));
            }

        }
        tableWidgetSetItemZebraColor(ui->bonusTableWidget);

        int page = (ui->bonusTableWidget->rowCount()%ROWNUMBER==0 && ui->bonusTableWidget->rowCount() != 0) ?
                    ui->bonusTableWidget->rowCount()/ROWNUMBER : ui->bonusTableWidget->rowCount()/ROWNUMBER+1;
        pageWidget->SetTotalPage(page);
        pageWidget->setShowTip(ui->bonusTableWidget->rowCount(),ROWNUMBER);
        pageChangeSlot(pageWidget->GetCurrentPage());

        pageWidget->setVisible(0 != ui->bonusTableWidget->rowCount());
        blankWidget->setVisible(ui->bonusTableWidget->rowCount() == 0);

        checkObtainAllBtnVisible();

        return;
    }

    if( id == "id-obtain_bonus_balance")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":{"))
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of get-bonus has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setDetailText(result);
            errorResultDialog.setInfoText(tr("Fail to get bonus!"));
            errorResultDialog.pop();
        }

        return;
    }

}

void BonusPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;
    HXChain::getInstance()->currentAccount = ui->accountComboBox->currentText();
    fetchBonusBalance();
}

void BonusPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void BonusPage::fetchBonusBalance()
{
    QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
    HXChain::getInstance()->postRPC( "id-get_bonus_balance-" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "get_bonus_balance",QJsonArray() << address));

}

void BonusPage::checkObtainAllBtnVisible()
{
    bool visible = false;
    for(int i = 0; i < ui->bonusTableWidget->rowCount(); i++)
    {
        QTableWidgetItem* item = ui->bonusTableWidget->item(i, 2);
        if(item)
        {
            if(!item->text().isEmpty())
            {
                visible = true;
            }
        }
    }
    ui->obtainAllBtn->setVisible(visible);
}

void BonusPage::on_bonusTableWidget_cellPressed(int row, int column)
{
    if(column == 2)
    {
        if(ui->bonusTableWidget->item(row,2)->text().isEmpty()) return;
        if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

        QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;

        FeeChargeWidget *feeCharge = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.minerIncomeFee.toDouble(),
                                                         HXChain::getInstance()->feeType,ui->accountComboBox->currentText(),
                                                         HXChain::getInstance()->mainFrame);
        connect(feeCharge,&FeeChargeWidget::confirmSignal,[this,address,row](){
            QJsonArray array;
            QJsonArray array2;
            array2 << ui->bonusTableWidget->item(row,0)->text();
            QString amountStr = ui->bonusTableWidget->item(row,1)->data(Qt::UserRole).toString();
            array2 << amountStr;
            array << array2;
            HXChain::getInstance()->postRPC( "id-obtain_bonus_balance",
                                             toJsonFormat( "obtain_bonus_balance",
                                                           QJsonArray() << address
                                                           << array
                                                           << true ));
        });
        feeCharge->show();

        return;
    }
}

void BonusPage::on_obtainAllBtn_clicked()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;

    FeeChargeWidget *feeCharge = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.minerIncomeFee.toDouble(),
                                                     HXChain::getInstance()->feeType,ui->accountComboBox->currentText(),
                                                     HXChain::getInstance()->mainFrame);
    connect(feeCharge,&FeeChargeWidget::confirmSignal,[this,address](){
        QJsonArray array;

        for(int i = 0; i < ui->bonusTableWidget->rowCount(); i++)
        {
            QString amountStr = ui->bonusTableWidget->item(i,1)->data(Qt::UserRole).toString();
            if(amountStr.toULongLong() < 1)     continue;
            QJsonArray array2;
            array2 << ui->bonusTableWidget->item(i,0)->text();
            array2 << amountStr;
            array << array2;
        }

        HXChain::getInstance()->postRPC( "id-obtain_bonus_balance",
                                         toJsonFormat( "obtain_bonus_balance",
                                                       QJsonArray() << address
                                                       << array
                                                       << true ));
    });
    feeCharge->show();
}

void BonusPage::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->bonusTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->bonusTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->bonusTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->bonusTableWidget->setRowHidden(i,true);
        }
    }
}

