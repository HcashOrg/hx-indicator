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
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }


    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    QRegExp regx("[a-zA-Z0-9\-\ ]{0,63}");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->sendtoLineEdit->setValidator( validator );
    ui->sendtoLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->tipLabel3->hide();
    ui->tipLabel4->hide();
    ui->tipLabel6->hide();

    getAssets();
    if(!assetType.isEmpty())
    {
        ui->assetComboBox->setCurrentText(revertERCSymbol( assetType));
    }

    inited = true;
	
    updateAmountSlots();

    feeWidget=new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.transferFee.toDouble(),HXChain::getInstance()->feeType,
                                  ui->accountComboBox->currentText());
    ui->stackedWidget->addWidget(feeWidget);
    ui->stackedWidget->setCurrentWidget(feeWidget);
    ui->stackedWidget->currentWidget()->resize(ui->stackedWidget->size());

    connect(this,&TransferPage::usePoundage,feeWidget,&FeeChooseWidget::updatePoundageID);
    //updatePoundage();

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

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
    ui->memoLabel->setVisible(true);
    ui->memoTextEdit->setVisible(true);

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->widget);

    setAmountPrecision();
}

TransferPage::~TransferPage()
{
    delete ui;
}



void TransferPage::accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( !inited)  return;

    accountName = arg1;
    HXChain::getInstance()->mainFrame->setCurrentAccount(accountName);

    ui->amountLineEdit->clear();
    updateAmountSlots();
}


void TransferPage::on_sendBtn_clicked()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

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
    if( type == AccountAddress || type == MultiSigAddress)
    {
        TransferConfirmDialog transferConfirmDialog( ui->sendtoLineEdit->text(),ui->amountLineEdit->text(),getRealAssetSymbol( ui->assetComboBox->currentText()), feeWidget->GetFeeNumber(), feeWidget->GetFeeType(), remark);
        bool yOrN = transferConfirmDialog.pop();
        if( yOrN)
        {
//            if(feeWidget && !feeWidget->GetFeeID().isEmpty())
//            {
//                HXChain::getInstance()->postRPC( "id-set_guarantee_id",
//                                                 toJsonFormat( "set_guarantee_id",
//                                                               QJsonArray() << feeWidget->GetFeeID() ));
//                qDebug()<<"id-set_guarantee_id"<<toJsonFormat( "set_guarantee_id",
//                                                               QJsonArray() << feeWidget->GetFeeID() );
//            }
            emit usePoundage();
            HXChain::getInstance()->postRPC( "id-transfer_to_address-" + accountName,
                                             toJsonFormat( "transfer_to_address",
                                                           QJsonArray() << accountName << ui->sendtoLineEdit->text()
                                                           << ui->amountLineEdit->text() << getRealAssetSymbol( ui->assetComboBox->currentText())
                                                           << remark << true ));
            ui->sendBtn->setEnabled(false);
        }

    }
    else
    {
        if( ui->addressLabel->text().isEmpty() || ui->tipLabel4->text() == tr("Invalid account name!"))
        {
            CommonDialog tipDialog(CommonDialog::OkOnly);
            tipDialog.setText( tr("Invalid account name!"));
            tipDialog.pop();
        }
        else
        {
            HXChain::getInstance()->postRPC( "id-transfer_to_address-" + accountName,
                                             toJsonFormat( "transfer_to_address",
                                                           QJsonArray() << accountName << ui->addressLabel->text()
                                                           << ui->amountLineEdit->text() << getRealAssetSymbol( ui->assetComboBox->currentText())
                                                           << remark << true ));
            ui->sendBtn->setEnabled(false);
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
    AssetInfo info = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId( getRealAssetSymbol( ui->assetComboBox->currentText()) ));
    QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(info.precision));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
}

void TransferPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);

    ui->sendBtn->setStyleSheet(OKBTN_STYLE);
    ui->transferRecordBtn->setStyleSheet(TOOLBUTTON_STYLE_1);


    ui->memoTextEdit->setStyleSheet("QTextEdit{background: transparent;color: rgb(83,61,138);font: 11px \"Microsoft YaHei UI Light\";border:none;border-bottom:1px solid rgb(83,61,138);}\
                                    QTextEdit:focus{border-bottom-color:rgb(83,61,138);}\
                                    QTextEdit:disabled{color: rgb(151,151,151);}");

//    ui->toolButton_chooseContact->setIconSize(QSize(12,12));
//    ui->toolButton_chooseContact->setIcon(QIcon(":/ui/wallet_ui/tans.png"));

    setStyleSheet("QToolButton#toolButton_chooseContact{background-image:url(:/ui/wallet_ui/trans.png);border:none;\
                   background-color:transparent;background-repeat: no-repeat;background-position: center;}\
                   QToolButton#toolButton_chooseContact:hover{background-color:black;");

}

//void TransferPage::updatePoundage()
//{//查询配置文件中的手续费设置
//    ui->checkBox->setChecked(false);
//    ui->checkBox->setEnabled(true);
//    ui->toolButton->setEnabled(false);

//    feeID = HXChain::getInstance()->configFile->value("/settings/feeOrderID").toString();
//    if(feeID.isEmpty())
//    {
//        ui->toolButton->setText(tr("poundage doesn't exist!"));
//    }
//    else
//    {
//        //查询承兑单
//        HXChain::getInstance()->postRPC( "id-get_guarantee_order",
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

    QStringList keys = HXChain::getInstance()->assetInfoMap.keys();
    foreach (QString key, keys)
    {
        ui->assetComboBox->addItem(revertERCSymbol(HXChain::getInstance()->assetInfoMap.value(key).symbol));
    }


    assetUpdating = false;
}


void TransferPage::jsonDataUpdated(QString id)
{
    if( id == "id-transfer_to_address-" + accountName)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        ui->sendBtn->setEnabled(true);
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
            if(result.contains("Insufficient Balance"))
            {
                errorResultDialog.setInfoText(tr("Balance not enough!"));
            }
            else
            {
                errorResultDialog.setInfoText(tr("Fail to transfer!"));
            }
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }
        return;
    }

    if( id == "TransferPage+get_account+" + ui->sendtoLineEdit->text())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object().value("result").toObject();
            if(object.isEmpty())
            {
                ui->tipLabel4->setText(tr("Invalid account name!"));
                ui->tipLabel4->setStyleSheet("color: rgb(255,0,0);");
                ui->addressLabel->clear();
            }
            else
            {
                QString accountAddress = object.value("addr").toString();
                ui->tipLabel4->setText(tr("Valid account name."));
                ui->tipLabel4->setStyleSheet("color: rgb(0,170,0);");
                ui->addressLabel->setText(accountAddress);
            }
        }

        return;
    }

//    else if("id-get_guarantee_order" == id)
//    {//查询承兑单id是否还有余额
//        QString result = HXChain::getInstance()->jsonDataValue(id);

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
//        QString result = HXChain::getInstance()->jsonDataValue(id);

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
    ui->addressLabel->clear();

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
        ui->tipLabel4->setStyleSheet("color: rgb(0,170,0);");
        ui->tipLabel4->show();
    }
    else if( type == MultiSigAddress)
    {
        ui->tipLabel4->setText(tr("Valid multi-sig address."));
        ui->tipLabel4->setStyleSheet("color: rgb(0,170,0);");
        ui->tipLabel4->show();
    }
    else if( type == ContractAddress)
    {
        ui->tipLabel4->setText(tr("Sending coins to contract address is not supported currently."));
        ui->tipLabel4->setStyleSheet("color: rgb(255,0,0);");
        ui->tipLabel4->show();
    }
    else
    {
//        ui->tipLabel4->setText(tr("Invalid address."));
//        ui->tipLabel4->setStyleSheet("color: rgb(255,0,0);");
//        ui->tipLabel4->show();
        ui->tipLabel4->setText(tr("Checking the name..."));
        ui->tipLabel4->setStyleSheet("color: rgb(120,85,0);");
        ui->tipLabel4->show();
        HXChain::getInstance()->postRPC( "TransferPage+get_account+" + ui->sendtoLineEdit->text(),
                                         toJsonFormat( "get_account",
                                                       QJsonArray() << ui->sendtoLineEdit->text() ));
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
    double fee = static_cast<double>(ba.size())*0.01/1024 + HXChain::getInstance()->feeChargeInfo.transferFee.toDouble();
    emit feeChangeSignal(fee);
}

void TransferPage::on_transferRecordBtn_clicked()
{
    currentTopWidget = new TransferRecordWidget();
    currentTopWidget->setParent(this);
    currentTopWidget->move(0,0);
    currentTopWidget->show();
    static_cast<TransferRecordWidget*>(currentTopWidget)->showTransferRecord(HXChain::getInstance()->accountInfoMap.value(accountName).address);
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
    QString assetType = getRealAssetSymbol(ui->assetComboBox->currentText());

    QString assetID ;
    int assetPrecision ;
    foreach (AssetInfo in, HXChain::getInstance()->assetInfoMap) {
        qDebug() << in.id << in.symbol << in.precision << assetType;
        if(in.symbol == assetType){
            assetID = in.id;
            assetPrecision = in.precision;
            break;
        }
    }

    //获取数量
    bool isFindAmmount = false;
    QMapIterator<QString, AccountInfo> i(HXChain::getInstance()->accountInfoMap);
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
                  AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(am.value().assetId);
                  ui->amountLineEdit->setPlaceholderText(tr("Max: %1").arg(getBigNumberString(am.value().amount, assetInfo.precision)));
                  isFindAmmount = true;
                  break;
              }
          }
      }
     if(!isFindAmmount)
     {
         ui->amountLineEdit->setEnabled(false);
         ui->amountLineEdit->setPlaceholderText(tr("Max: %1").arg(0));
         return;
     }
     ui->amountLineEdit->setEnabled(true);
}

//void TransferPage::checkStateChangedSlots(int state)
//{
//    if(state == Qt::Checked)
//    {
//        ui->toolButton->setEnabled(true);

////        HXChain::getInstance()->postRPC( "id-set_guarantee_id",
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
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);
//    painter.save();
//    const qreal radius = 10;
//    QRectF rect = QRect(QPoint(50, 95), QSize(670,340));
//    QRectF rect1 = QRect(QPoint(45, 90), QSize(680,350));


//    QRadialGradient radial(385, 385, 770, 385,385);
//        radial.setColorAt(0, QColor(0,0,0,15));
//        radial.setColorAt(1, QColor(218,255,248,15));

//    painter.setBrush(radial);
//    painter.setPen(Qt::NoPen);
//    painter.drawRoundedRect(rect1,10,10);

//    painter.setBrush(QBrush(Qt::white));
//    painter.setPen(Qt::NoPen);
//    painter.drawRoundedRect(rect,10,10);


//    painter.restore();


    QWidget::paintEvent(event);
}

void TransferPage::on_amountLineEdit_textEdited(const QString &arg1)
{
    QString assetId = HXChain::getInstance()->getAssetId(getRealAssetSymbol(ui->assetComboBox->currentText()));
    AssetAmount assetAmount = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).assetAmountMap.value(assetId);
    QString amountStr = getBigNumberString(assetAmount.amount, HXChain::getInstance()->assetInfoMap.value(assetId).precision);

    if(ui->amountLineEdit->text().toDouble() > amountStr.toDouble())
    {
        ui->amountLineEdit->setText(amountStr);
    }
}
