#include <QListView>

#ifdef WIN32
#include <windows.h>
#endif



#include "transferpage.h"
#include "ui_transferpage.h"
#include "wallet.h"

#include "commondialog.h"
#include "transferconfirmdialog.h"
#include "transferrecordwidget.h"
#include "ContactChooseWidget.h"
#include "BlurWidget.h"

#include "poundage/PoundageDataUtil.h"
#include "FeeChooseWidget.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"

TransferPage::TransferPage(QString name,QWidget *parent,QString assettype) :
    QWidget(parent),
    accountName(name),
    assetType(assettype),
    inited(false),
    assetUpdating(false),
    contactUpdating(false),
    currentTopWidget(NULL),
    ui(new Ui::TransferPage)

{
	

    ui->setupUi(this);
    InitStyle();

    //初始化账户comboBox
    // 账户下拉框按字母顺序排序
    QStringList keys = UBChain::getInstance()->accountInfoMap.keys();

    //
    if(!keys.empty())
    {
        ui->accountComboBox->addItems( keys);
    }

    if(ui->accountComboBox->count() != 0)
    {
        if( accountName.isEmpty() )
        {
            ui->accountComboBox->setCurrentIndex(0);
        }
        else
        {
            ui->accountComboBox->setCurrentText( accountName);
        }
    }

    if( accountName.isEmpty())  // 如果是点击账单跳转
    {
        if( UBChain::getInstance()->addressMap.size() > 0)
        {
            accountName = UBChain::getInstance()->addressMap.keys().at(0);
        }
        else  // 如果还没有账户
        {
            emit back();    // 跳转在functionbar  这里并没有用
            return;
        }
    }



    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    setAmountPrecision();

    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->sendtoLineEdit->setValidator( validator );
    ui->sendtoLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->tipLabel3->hide();
    ui->tipLabel4->hide();
    ui->tipLabel6->hide();

    getAssets();
    if(!assetType.isEmpty())
    {
        ui->assetComboBox->setCurrentText(assetType);
    }

    inited = true;
	
    updateAmountSlots();

    feeWidget=new FeeChooseWidget(UBChain::getInstance()->feeChargeInfo.transferFee.toDouble(),UBChain::getInstance()->feeType,
                                  ui->accountComboBox->currentText());
    ui->stackedWidget->addWidget(feeWidget);
    ui->stackedWidget->setCurrentWidget(feeWidget);

    connect(this,&TransferPage::usePoundage,feeWidget,&FeeChooseWidget::updatePoundageID);
    //updatePoundage();

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    connect(ui->toolButton_chooseContact,&QToolButton::clicked,this,&TransferPage::chooseContactSlots);
    //connect(ui->checkBox,&QCheckBox::stateChanged,this,&TransferPage::checkStateChangedSlots);

    connect(this,&TransferPage::feeChangeSignal,feeWidget,&FeeChooseWidget::updateFeeNumberSlots);

    connect(ui->accountComboBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(accountComboBox_currentIndexChanged(const QString &)));
    connect(ui->amountLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(amountLineEdit_textChanged(const QString&)));
    connect(ui->assetComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(assetComboBox_currentIndexChanged(int)));
    connect(ui->sendtoLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(sendtoLineEdit_textChanged(const QString &)));
    connect(ui->memoTextEdit,SIGNAL(textChanged()),this,SLOT(memoTextEdit_textChanged()));
    connect(ui->accountComboBox,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),std::bind(&FeeChooseWidget::updateAccountNameSlots,feeWidget,std::placeholders::_1,true));
    //隐藏备注
    ui->memoLabel->setVisible(false);
    ui->memoTextEdit->setVisible(false);
}

TransferPage::~TransferPage()
{
    delete ui;
}



void TransferPage::accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( !inited)  return;

    accountName = arg1;
    UBChain::getInstance()->mainFrame->setCurrentAccount(accountName);

    ui->amountLineEdit->clear();
    updateAmountSlots();
}


void TransferPage::on_sendBtn_clicked()
{
    if(!UBChain::getInstance()->ValidateOnChainOperation()) return;

    if(ui->amountLineEdit->text().size() == 0 || ui->sendtoLineEdit->text().size() == 0)
    {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr("Please enter the amount and address."));
        tipDialog.pop();
        return;
    }

    if( ui->amountLineEdit->text().toDouble()  <= 0)
    {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr("The amount can not be 0"));
        tipDialog.pop();
        return;
    }


    QString remark = ui->memoTextEdit->toPlainText();


    AddressType type = checkAddress(ui->sendtoLineEdit->text(),AccountAddress | MultiSigAddress);
    if( type == AccountAddress)
    {
        TransferConfirmDialog transferConfirmDialog( ui->sendtoLineEdit->text(),ui->amountLineEdit->text(), ui->assetComboBox->currentText(), feeWidget->GetFeeNumber(), feeWidget->GetFeeType(), remark);
        bool yOrN = transferConfirmDialog.pop();
        if( yOrN)
        {
//            if(feeWidget && !feeWidget->GetFeeID().isEmpty())
//            {
//                UBChain::getInstance()->postRPC( "id-set_guarantee_id",
//                                                 toJsonFormat( "set_guarantee_id",
//                                                               QJsonArray() << feeWidget->GetFeeID() ));
//                qDebug()<<"id-set_guarantee_id"<<toJsonFormat( "set_guarantee_id",
//                                                               QJsonArray() << feeWidget->GetFeeID() );
//            }
            emit usePoundage();
            UBChain::getInstance()->postRPC( "id-transfer_to_address-" + accountName,
                                             toJsonFormat( "transfer_to_address",
                                                           QJsonArray() << accountName << ui->sendtoLineEdit->text()
                                                           << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                                                           << remark << true ));
qDebug() << toJsonFormat( "transfer_to_address",
                          QJsonArray() << accountName << ui->sendtoLineEdit->text()
                          << ui->amountLineEdit->text() << ui->assetComboBox->currentText()
                          << remark << true );
        }

    }
}

void TransferPage::amountLineEdit_textChanged(const QString &arg1)
{

}

void TransferPage::refresh()
{

}

void TransferPage::setAmountPrecision()
{
    int pre = ASSET_PRECISION;
    foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
        if(asset.symbol == ui->assetComboBox->currentText())
        {
            pre = asset.precision;
            break;
        }
    }
    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(pre));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
}

void TransferPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    ui->sendBtn->setStyleSheet(OKBTN_STYLE);
    ui->transferRecordBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    ui->memoTextEdit->setStyleSheet("border:none;background:none;color:#5474EB;");

//    ui->toolButton_chooseContact->setIconSize(QSize(12,12));
//    ui->toolButton_chooseContact->setIcon(QIcon(":/ui/wallet_ui/tans.png"));

    setStyleSheet("QToolButton#toolButton_chooseContact{background-image:url(:/ui/wallet_ui/trans.png);border:none;\
                                                background-color:transparent;background-repeat: no-repeat;background-position: center;}"
                                                "QToolButton#toolButton_chooseContact:hover{background-color:black;");

    ui->accountComboBox->setStyleSheet(COMBOBOX_STYLE_BOTTOMBORDER);
    ui->assetComboBox->setStyleSheet(COMBOBOX_STYLE_BOTTOMBORDER);
}

//void TransferPage::updatePoundage()
//{//查询配置文件中的手续费设置
//    ui->checkBox->setChecked(false);
//    ui->checkBox->setEnabled(true);
//    ui->toolButton->setEnabled(false);

//    feeID = UBChain::getInstance()->configFile->value("/settings/feeOrderID").toString();
//    if(feeID.isEmpty())
//    {
//        ui->toolButton->setText(tr("poundage doesn't exist!"));
//    }
//    else
//    {
//        //查询承兑单
//        UBChain::getInstance()->postRPC( "id-get_guarantee_order",
//                                         toJsonFormat( "get_guarantee_order",
//                                                       QJsonArray() << feeID));

//    }
//}

QString TransferPage::getCurrentAccount()
{
    return accountName;
}

void TransferPage::setAddress(QString address)
{
    ui->sendtoLineEdit->setText(address);
}


void TransferPage::getAssets()
{
    assetUpdating = true;

    QStringList keys = UBChain::getInstance()->assetInfoMap.keys();
    foreach (QString key, keys)
    {
        ui->assetComboBox->addItem(UBChain::getInstance()->assetInfoMap.value(key).symbol);
    }


    assetUpdating = false;
}


void TransferPage::jsonDataUpdated(QString id)
{
    if( id == "id-transfer_to_address-" + accountName)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction has been sent,please wait for confirmation"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Fail to transfer!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }
        return;
    }
//    else if("id-get_guarantee_order" == id)
//    {//查询承兑单id是否还有余额
//        QString result = UBChain::getInstance()->jsonDataValue(id);

//        if(result.isEmpty() || result.startsWith("\"error"))
//        {
//            feeID.clear();
//            ui->toolButton->setText(tr("poundage doesn't exist!"));
//            return;
//        }
//        result.prepend("{");
//        result.append("}");

//        QJsonParseError json_error;
//        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1(),&json_error);
//        if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
//        {
//            feeID.clear();
//            ui->toolButton->setText(tr("poundage doesn't exist!"));
//            return;
//        }
//        QJsonObject jsonObject = parse_doucment.object().value("result").toObject();
//        std::shared_ptr<PoundageUnit> unit = std::make_shared<PoundageUnit>();
//        if(PoundageDataUtil::ParseJsonObjToUnit(jsonObject,unit))
//        {
//            if(unit->poundageFinished)
//            {
//                feeID.clear();
//                ui->toolButton->setText(tr("poundage doesn't exist!"));
//            }
//            else
//            {
//                ui->checkBox->setChecked(true);
//                double exchangeRate = unit->sourceCoinNumber/unit->targetCoinNumber;
//                ui->toolButton->setText(ui->toolButton->text().replace("@",QString::number(exchangeRate))
//                                        .replace("#",unit->chainType).replace("$",QString::number(unit->calSourceLeftNumber())));
//            }
//        }

//        qDebug()<<"承兑单余额---"<<result;

//    }
//    else if("id-set_guarantee_id" == id)
//    {
//        QString result = UBChain::getInstance()->jsonDataValue(id);

//        qDebug()<<"设置承兑单id---"<<result;
//    }

}


void TransferPage::assetComboBox_currentIndexChanged(int index)
{
    if( assetUpdating)  return;

    setAmountPrecision();

    ui->amountLineEdit->clear();
    updateAmountSlots();
}

void TransferPage::sendtoLineEdit_textChanged(const QString &arg1)
{
    if( ui->sendtoLineEdit->text().contains(" ") || ui->sendtoLineEdit->text().contains("\n"))   // 不判断就remove的话 右键菜单撤销看起来等于不能用
    {
        ui->sendtoLineEdit->setText( ui->sendtoLineEdit->text().simplified().remove(" "));
    }

//    ui->sendtoLineEdit->setText( ui->sendtoLineEdit->text().remove("\n"));
    if( ui->sendtoLineEdit->text().isEmpty()  )
    {
        ui->tipLabel4->hide();
        return;
    }

    AddressType type = checkAddress(ui->sendtoLineEdit->text(),AccountAddress | ContractAddress | MultiSigAddress);
    if( type == AccountAddress)
    {
        ui->tipLabel4->setText(tr("Valid account address."));
        ui->tipLabel4->setStyleSheet("color: rgb(43,230,131);");
        ui->tipLabel4->show();
//        calculateCallContractFee();
    }
    else if( type == ContractAddress)
    {
        ui->tipLabel4->setText(tr("Sending coins to contract address is not supported currently."));
        ui->tipLabel4->setStyleSheet("color: rgb(255,34,76);");
        ui->tipLabel4->show();
    }
    else if( type == MultiSigAddress)
    {
        if(ui->assetComboBox->currentIndex() > 0)
        {
            ui->tipLabel4->setText(tr("You can only send %1s to multisig address currently.").arg(ASSET_NAME));
            ui->tipLabel4->setStyleSheet("color: rgb(255,34,76);");
            ui->tipLabel4->show();
        }
        else
        {
            ui->tipLabel4->setText(tr("Valid multisig address."));
            ui->tipLabel4->setStyleSheet("color: rgb(43,230,131);");
            ui->tipLabel4->show();
        }
    }
    else
    {
        ui->tipLabel4->setText(tr("Invalid address."));
        ui->tipLabel4->setStyleSheet("color: rgb(255,34,76);");
        ui->tipLabel4->show();
    }


}


void TransferPage::memoTextEdit_textChanged()
{
    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(ui->memoTextEdit->toPlainText());
    //取消40个字的限制
//    if( ba.size() > 40)
//    {
//        ui->tipLabel6->show();
//    }
//    else
//    {
//        ui->tipLabel6->hide();
//    }
    double fee = static_cast<double>(ba.size())*10/1024 + UBChain::getInstance()->feeChargeInfo.transferFee.toDouble();
    emit feeChangeSignal(fee);
}

void TransferPage::on_transferRecordBtn_clicked()
{
    currentTopWidget = new TransferRecordWidget();
    currentTopWidget->setParent(this);
    currentTopWidget->move(0,0);
    currentTopWidget->show();
    static_cast<TransferRecordWidget*>(currentTopWidget)->showTransferRecord(UBChain::getInstance()->accountInfoMap.value(accountName).address);
}

void TransferPage::chooseContactSlots()
{
    ContactChooseWidget *wi = new ContactChooseWidget(this);
    if(wi->isInitSuccess())
    {
        BlurWidget *blur = new BlurWidget(this);
        connect(wi,&ContactChooseWidget::closeSignal,blur,&BlurWidget::close);
        connect(wi,&ContactChooseWidget::selectContactSignal,this,&TransferPage::selectContactSlots);

        blur->show();
        wi->move(QPoint(160,140));
        wi->show();
        wi->raise();
    }
}

void TransferPage::selectContactSlots(const QString &name, const QString &address)
{
    ui->sendtoLineEdit->setText(address);
}

void TransferPage::updateAmountSlots()
{//根据当前账户、资产类型，判断最大转账数量
    QString name = ui->accountComboBox->currentText();
    QString assetType = ui->assetComboBox->currentText();

    QString assetID ;
    int assetPrecision ;
    foreach (AssetInfo in, UBChain::getInstance()->assetInfoMap) {
        if(in.symbol == assetType){
            assetID = in.id;
            assetPrecision = in.precision;
            break;
        }
    }

    //获取数量
    bool isFindAmmount = false;
    QMapIterator<QString, AccountInfo> i(UBChain::getInstance()->accountInfoMap);
      while (i.hasNext()) {
          i.next();
          if(isFindAmmount) break;
          if(i.key() != name) continue;
          AccountInfo info = i.value();
          QMapIterator<QString,AssetAmount> am(info.assetAmountMap);
          while(am.hasNext()){
              am.next();
              if(am.key() == assetID)
              {
                  AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(am.value().assetId);
                  ui->amountLineEdit->setPlaceholderText(tr("Max: %1").arg(getBigNumberString(am.value().amount, assetInfo.precision)));
                  isFindAmmount = true;
                  break;
              }
          }
      }
     if(!isFindAmmount)
     {
         ui->amountLineEdit->setEnabled(false);
         ui->amountLineEdit->setPlaceholderText(tr("cannot find asset!"));
         return;
     }
     ui->amountLineEdit->setEnabled(true);
}

//void TransferPage::checkStateChangedSlots(int state)
//{
//    if(state == Qt::Checked)
//    {
//        ui->toolButton->setEnabled(true);

////        UBChain::getInstance()->postRPC( "id-set_guarantee_id",
////                                         toJsonFormat( "set_guarantee_id",
////                                                       QJsonArray() << "0" ));

//    }
//    else
//    {
//        ui->toolButton->setEnabled(false);
//    }
//}

void TransferPage::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.save();
    const qreal radius = 10;
    QRectF rect = QRect(QPoint(50, 95), QSize(670,340));
    QRectF rect1 = QRect(QPoint(45, 90), QSize(680,350));


    QRadialGradient radial(385, 385, 770, 385,385);
        radial.setColorAt(0, QColor(0,0,0,15));
        radial.setColorAt(1, QColor(218,255,248,15));

    painter.setBrush(radial);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect1,10,10);

    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect,10,10);


    painter.restore();


    QWidget::paintEvent(event);
}
