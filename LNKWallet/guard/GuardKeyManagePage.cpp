#include "GuardKeyManagePage.h"
#include "ui_GuardKeyManagePage.h"

#include "wallet.h"
#include "control/AssetIconItem.h"
#include "showcontentdialog.h"
#include "ToolButtonWidget.h"
#include "GuardKeyUpdatingInfoDialog.h"
#include "dialog/checkpwddialog.h"
#include "commondialog.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "AssetChangeHistoryWidget.h"
#include "ChangeCrosschainAddressDialog.h"


GuardKeyManagePage::GuardKeyManagePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GuardKeyManagePage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->multisigTableWidget->installEventFilter(this);
    ui->multisigTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->multisigTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->multisigTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->multisigTableWidget->setFrameShape(QFrame::NoFrame);
    ui->multisigTableWidget->setMouseTracking(true);
    ui->multisigTableWidget->setShowGrid(false);//隐藏表格线

    ui->multisigTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->multisigTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->multisigTableWidget->horizontalHeader()->setVisible(true);
    ui->multisigTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->multisigTableWidget->setColumnWidth(0,120);
    ui->multisigTableWidget->setColumnWidth(1,140);
    ui->multisigTableWidget->setColumnWidth(2,140);
    ui->multisigTableWidget->setColumnWidth(3,100);
    ui->multisigTableWidget->setColumnWidth(4,80);
    ui->multisigTableWidget->setColumnWidth(5,80);
    ui->multisigTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->historyBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->importBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->changeAddressBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    init();
}

GuardKeyManagePage::~GuardKeyManagePage()
{
    delete ui;
}

void GuardKeyManagePage::init()
{
    inited = false;

    ui->accountComboBox->clear();
    QStringList accounts = UBChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    UBChain::getInstance()->mainFrame->installBlurEffect(ui->multisigTableWidget);

    showMultisigInfo();

    inited = true;
}

void GuardKeyManagePage::showMultisigInfo()
{
    QStringList assetIds = UBChain::getInstance()->assetInfoMap.keys();
    assetIds.removeAll("1.3.0");    // 不显示LNK

    int size = assetIds.size();
    ui->multisigTableWidget->setRowCount(0);
    ui->multisigTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->multisigTableWidget->setRowHeight(i,40);

        AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(assetIds.at(i));

        QString itemColor = (i % 2) ?"rgb(252,253,255)":"white";

        //资产名
        QString symbol = assetInfo.symbol;
        ui->multisigTableWidget->setItem(i,0,new QTableWidgetItem(symbol));
        ui->multisigTableWidget->setItem(i,1,new QTableWidgetItem(assetInfo.hotAddress));
        ui->multisigTableWidget->setItem(i,2,new QTableWidgetItem(assetInfo.coldAddress));
        ui->multisigTableWidget->setItem(i,3,new QTableWidgetItem(QString::number(assetInfo.effectiveBlock)));

        QStringList guardAccountIds = UBChain::getInstance()->getInstance()->getAssetMultisigUpdatedGuards(symbol);
        if(guardAccountIds.isEmpty())
        {
            ui->multisigTableWidget->setItem(i,4,new QTableWidgetItem(tr("no update")));
        }
        else
        {
            ui->multisigTableWidget->setItem(i,4,new QTableWidgetItem(tr("updating")));

            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->multisigTableWidget->item(i,4)->text());
            toolButton->setBackgroundColor(itemColor);
            ui->multisigTableWidget->setCellWidget(i,4,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&GuardKeyManagePage::on_multisigTableWidget_cellClicked,this,i,4));
        }

        if(ui->accountComboBox->currentText().isEmpty())
        {
            ui->multisigTableWidget->setItem(i,5,new QTableWidgetItem(tr("no guard")));
        }
        else
        {
            QString accountId = UBChain::getInstance()->formalGuardMap.value(ui->accountComboBox->currentText()).accountId;

            if(guardAccountIds.contains(accountId))
            {
                ui->multisigTableWidget->setItem(i,5,new QTableWidgetItem(tr("updated")));

                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->multisigTableWidget->item(i,5)->text());
                toolButton->setBackgroundColor(itemColor);
                toolButton->setEnabled(false);
                ui->multisigTableWidget->setCellWidget(i,5,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&GuardKeyManagePage::on_multisigTableWidget_cellClicked,this,i,5));
            }
            else
            {
                ui->multisigTableWidget->setItem(i,5,new QTableWidgetItem(tr("update")));

                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->multisigTableWidget->item(i,5)->text());
                toolButton->setBackgroundColor(itemColor);
                ui->multisigTableWidget->setCellWidget(i,5,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&GuardKeyManagePage::on_multisigTableWidget_cellClicked,this,i,5));
            }
        }


        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->multisigTableWidget->item(i,0)->text());
        assetIconItem->setBackgroundColor(itemColor);
        ui->multisigTableWidget->setCellWidget(i, 0, assetIconItem);



        for (int j : {1,2,3,4})
        {
            if(i%2)
            {
                ui->multisigTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                ui->multisigTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
            }
            else
            {
                ui->multisigTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                ui->multisigTableWidget->item(i,j)->setBackgroundColor(QColor("white"));
            }
        }
    }
}

void GuardKeyManagePage::jsonDataUpdated(QString id)
{
    if( id.startsWith("id-update_asset_private_keys"))
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of updating multisig-address has been sent,please wait for confirmation"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }
        return;
    }

    if( id == "finish-import_crosschain_key")
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("The cross-chain keys has already imported."));
        commonDialog.pop();
    }

    if( id == "id-import_crosschain_key")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;
    }
}

void GuardKeyManagePage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void GuardKeyManagePage::on_multisigTableWidget_cellClicked(int row, int column)
{
    if(column == 4)
    {
        if(ui->multisigTableWidget->item(row,4)->text() != tr("updating"))    return;

        GuardKeyUpdatingInfoDialog guardKeyUpdatingInfoDialog;
        guardKeyUpdatingInfoDialog.setAsset(ui->multisigTableWidget->item(row,0)->text());
        guardKeyUpdatingInfoDialog.pop();

        return;
    }

    if(column == 5)
    {
        if(ui->accountComboBox->currentText().isEmpty())    return;
        if(ui->multisigTableWidget->item(row,5)->text() != tr("update"))    return;

        QString assetSymbol = ui->multisigTableWidget->item(row,0)->text();
        CommonDialog commonDialog(CommonDialog::YesOrNo);
        commonDialog.setText(tr("You are changing the key of %1 multisig-address on the chain. Sure to change it?").arg(assetSymbol));
        if(commonDialog.pop())
        {
            CheckPwdDialog checkPwdDialog;
            if(!checkPwdDialog.pop())   return;

            UBChain::getInstance()->postRPC( "id-update_asset_private_keys", toJsonFormat( "update_asset_private_keys",
                                    QJsonArray() << ui->accountComboBox->currentText() << assetSymbol << true));

        }

        return;
    }
}

void GuardKeyManagePage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;

    UBChain::getInstance()->currentAccount = ui->accountComboBox->currentText();
    showMultisigInfo();
}

void GuardKeyManagePage::on_multisigTableWidget_cellPressed(int row, int column)
{
    if( column == 1 || column == 2)
    {
        ShowContentDialog showContentDialog( ui->multisigTableWidget->item(row, column)->text(),this);

        int x = ui->multisigTableWidget->columnViewportPosition(column) + ui->multisigTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->multisigTableWidget->rowViewportPosition(row) - 10 + ui->multisigTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->multisigTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void GuardKeyManagePage::on_historyBtn_clicked()
{
    AssetChangeHistoryWidget* assetChangeHistoryWidget = new AssetChangeHistoryWidget(this);
    assetChangeHistoryWidget->setAttribute(Qt::WA_DeleteOnClose);
    assetChangeHistoryWidget->setObjectName("assetChangeHistoryWidget");
    assetChangeHistoryWidget->move(0,0);
    assetChangeHistoryWidget->show();
    assetChangeHistoryWidget->raise();

    emit backBtnVisible(true);
}

void GuardKeyManagePage::on_importBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select the file of cross-chain key"), "", "*.gcck");
    qDebug() << "Fffffffffff " << filePath;

    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))     return;
    QByteArray ba = file.readAll();
    file.close();

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba,&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return ;
    QJsonObject object = parse_doucment.object();
    QStringList keys = object.keys();
    foreach (QString key, keys)
    {
        QJsonArray array = object.take(key).toArray();
        foreach (QJsonValue v, array)
        {
            QJsonObject object2 = v.toObject();
            QString wifKey = object2.take("wif_key").toString();
            UBChain::getInstance()->postRPC( "id-import_crosschain_key", toJsonFormat( "import_crosschain_key",
                                    QJsonArray()  << wifKey << key));
        }
    }

    UBChain::getInstance()->postRPC( "finish-import_crosschain_key", toJsonFormat( "import_crosschain_key",
                            QJsonArray() ));
}

void GuardKeyManagePage::on_changeAddressBtn_clicked()
{
    ChangeCrosschainAddressDialog changeCrosschainAddressDialog;
    changeCrosschainAddressDialog.pop();
}
