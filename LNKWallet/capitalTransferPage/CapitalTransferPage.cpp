#include "CapitalTransferPage.h"
#include "ui_CapitalTransferPage.h"

#include <QPainter>
#include <QDebug>

#include "PasswordConfirmWidget.h"
#include "CapitalTransferDataUtil.h"
#include "CapitalConfirmWidget.h"
#include "wallet.h"

#include "extra/HttpManager.h"
#include "commondialog.h"
#include "crossmark/crosscapitalmark.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"

static const QMap<QString,double> dust_number = {{"BTC",0.00001},{"LTC",0.001},{"HC",0.001}};
//static const QMap<QString,QString> fee_charge = {{"BTC","0.001"},{"LTC","0.001"},{"HC","0.01"},{"ETH","0.001"}};
class CapitalTransferPage::CapitalTransferPagePrivate
{
public:
    CapitalTransferPagePrivate(const CapitalTransferInput &data)
        :account_name(data.account_name),account_address(data.account_address),symbol(data.symbol)
//        ,fee(fee_charge.value(data.symbol))
        ,actualNumber("0")
        ,precision(5)
    {
        foreach(AssetInfo asset,HXChain::getInstance()->assetInfoMap){
            if(asset.symbol == symbol)
            {
                precision = asset.precision;
                break;
            }
        }
    }
public:
    QString account_name;
    QString account_address;
    QString symbol;

    QString tunnel_account_address;
    QString asset_max_ammount;
//    QString fee;

    int precision;

    QString multisig_address;

    QString actualNumber;
    QJsonObject trade_detail;
    HttpManager httpManager;////用于查询通道账户余额
};

CapitalTransferPage::CapitalTransferPage(const CapitalTransferInput &data,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CapitalTransferPage),
    _p(new CapitalTransferPagePrivate(data))
{
    ui->setupUi(this);
    InitWidget();
}

CapitalTransferPage::~CapitalTransferPage()
{
    delete _p;
    delete ui;
}

void CapitalTransferPage::ConfirmSlots()
{
    hide();
    QString actualShow = _p->actualNumber + " "+_p->symbol;
    QString feeShow = ui->label_fee->text();
    QString totalShow = ui->lineEdit_number->text() + " "+_p->symbol;
    CapitalConfirmWidget *confirmWidget = new CapitalConfirmWidget(CapitalConfirmWidget::CapitalConfirmInput(
                                                                   ui->radioButton_deposit->isChecked()?_p->account_address:ui->lineEdit_address->text(),
                                                                   actualShow,feeShow,totalShow),HXChain::getInstance()->mainFrame);
    connect(confirmWidget,&CapitalConfirmWidget::ConfirmSignal,this,&CapitalTransferPage::passwordConfirmSlots);
    connect(confirmWidget,&CapitalConfirmWidget::CancelSignal,this,&CapitalTransferPage::passwordCancelSlots);
    //PasswordConfirmWidget *confirmWidget = new PasswordConfirmWidget(HXChain::getInstance()->mainFrame);
    //connect(confirmWidget,&PasswordConfirmWidget::confirmSignal,this,&CapitalTransferPage::passwordConfirmSlots);
    //connect(confirmWidget,&PasswordConfirmWidget::cancelSignal,this,&CapitalTransferPage::passwordCancelSlots);
    //confirmWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    //confirmWidget->setWindowModality(Qt::WindowModal);
    //confirmWidget->setAttribute(Qt::WA_DeleteOnClose);
    //confirmWidget->move(mapToGlobal(QPoint(-190,-50)));
    confirmWidget->show();
    emit backBtnVisible(false);
}

void CapitalTransferPage::radioChangedSlots()
{
    if(ui->radioButton_deposit->isChecked())
    {
        ui->lineEdit_address->setPlaceholderText(tr("select withdraw to input address"));
        ui->lineEdit_address->clear();
        if(!_p->tunnel_account_address.isEmpty())
        {
            //ui->lineEdit_address->setText(_p->tunnel_account_address);
        }
        ui->lineEdit_address->setEnabled(false);
    }
    else if(ui->radioButton_withdraw->isChecked())
    {
        ui->lineEdit_address->setPlaceholderText(tr("please input withdraw address..."));
        ui->lineEdit_address->setEnabled(true);
        ui->lineEdit_address->clear();
    }

    ui->lineEdit_number->clear();
//    ui->toolButton_confirm->setVisible(false);
    ui->toolButton_confirm->setEnabled(false);
}

void CapitalTransferPage::jsonDataUpdated(QString id)
{
    if("captial-get_binding_account" == id)
    {//获取到当前账户绑定的通道账户
        QString result = HXChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() || result.startsWith("\"error"))
        {
            ui->radioButton_deposit->setEnabled(false);
            ui->radioButton_withdraw->setEnabled(false);
            ui->lineEdit_number->setEnabled(false);
            ui->label_tunnelAddress->setText(tr("Can not find tunnel address!"));
            return;
        }
        result.prepend("{");
        result.append("}");
    //提取通道账户地址
        _p->tunnel_account_address = CapitalTransferDataUtil::parseTunnelAddress(result);
        if( _p->tunnel_account_address.isEmpty())
        {
            ui->radioButton_deposit->setEnabled(false);
            ui->radioButton_withdraw->setEnabled(false);
            ui->lineEdit_number->setEnabled(false);
            ui->label_tunnelAddress->setText(tr("Can not find tunnel address!"));
            return;
        }
        if(ui->radioButton_deposit->isChecked())
        {
            ui->label_tunnelAddress->setText(_p->tunnel_account_address);
        }
        //查询tunnel地址的余额
        PostQueryTunnelMoney(_p->symbol,_p->tunnel_account_address);
    }
    else if("captial-get_current_multi_address" == id)
    {//获取到多签地址
        QString result = HXChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() || result.startsWith("\"error"))
        {
            ui->radioButton_deposit->setEnabled(false);
            ui->radioButton_withdraw->setEnabled(false);
            ui->lineEdit_number->setEnabled(false);
            ui->label_tunnelAddress->setText(tr("Can not find multi-address!"));
            return;
        }
        result.prepend("{");
        result.append("}");
    //提取多签地址
        _p->multisig_address = CapitalTransferDataUtil::parseMutiAddress(result);\
        if( _p->multisig_address.isEmpty())
        {
            ui->radioButton_deposit->setEnabled(false);
            ui->radioButton_withdraw->setEnabled(false);
            ui->lineEdit_number->setEnabled(false);
            ui->label_tunnelAddress->setText(tr("Can not find multi-address!"));
            return;
        }

        qDebug()<<"多签地址"<<_p->multisig_address;

    }
    else if("captial-createrawtransaction" == id)
    {//获取到创建交易信息
        QString result = HXChain::getInstance()->jsonDataValue( id);
        qDebug()<<"transaction"<<result;

        if( result.isEmpty() || result.startsWith("\"error"))
        {
//            ui->toolButton_confirm->setVisible(false);
            ui->toolButton_confirm->setEnabled(false);
            return;
        }
        result.prepend("{");
        result.append("}");
        _p->trade_detail = CapitalTransferDataUtil::parseTradeDetail(result);

        //创建交易成功后，显示确认按钮
//        ui->toolButton_confirm->setVisible(true);
        ui->toolButton_confirm->setEnabled(true);

        qDebug()<<"detail"<<_p->trade_detail;

    }
    else if("captial-signrawtransaction" == id)
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");

        qDebug()<<"signature"<<HXChain::getInstance()->jsonDataValue( id);

        if( result.startsWith("{\"result\":"))             // 成功
        {
            HXChain::getInstance()->mainFrame->crossMark->TransactionInput(result,_p->symbol,_p->account_name,ui->lineEdit_number->text().toDouble());

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

//        CommonDialog dia(CommonDialog::OkOnly);
//        dia.setText(HXChain::getInstance()->jsonDataValue( id));
//        dia.pop();
        close();
    }
}

void CapitalTransferPage::httpReplied(QByteArray _data, int _status)
{
//    qDebug() << "auto--http-- " << _data << _status;

    QJsonObject object  = QJsonDocument::fromJson(_data).object().value("result").toObject();

    double balance = jsonValueToDouble(object.value("balance"));
    _p->asset_max_ammount = QString::number(std::max<double>(0,balance),'f',_p->precision) ;
    ui->label_number->setText(_p->asset_max_ammount + " "+_p->symbol);

    QDoubleValidator *validator = new QDoubleValidator(0,_p->asset_max_ammount.toDouble(),_p->precision,this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_number->setValidator(validator);
    ui->lineEdit_number->setPlaceholderText(tr("max:")+QString::number(validator->top(),'f',_p->precision));

}

void CapitalTransferPage::passwordConfirmSlots()
{//提交充值或提现指令
    if(ui->radioButton_deposit->isChecked())
    {//充值，对已有交易进行签名
        HXChain::getInstance()->postRPC( "captial-signrawtransaction",
                                         toJsonFormat( "signrawtransaction", QJsonArray()
                                         <<_p->tunnel_account_address<<_p->symbol<<_p->trade_detail<<true ));
    }
    else if(ui->radioButton_withdraw->isChecked())
    {//提现
        HXChain::getInstance()->postRPC( "captial-signrawtransaction",
                                         toJsonFormat( "signrawtransaction", QJsonArray()
                                         <<_p->tunnel_account_address<<_p->symbol<<_p->trade_detail<<true ));
    }
}

void CapitalTransferPage::passwordCancelSlots()
{
    show();
    emit backBtnVisible(true);
}

void CapitalTransferPage::numberChangeSlots(const QString &number)
{
    //修正数值
    QDoubleValidator* via = dynamic_cast<QDoubleValidator*>(const_cast<QValidator*>(ui->lineEdit_number->validator()));
    if(!via)
    {
        return;
    }
    if(ui->lineEdit_number->text().toDouble() > via->top())
    {
        ui->lineEdit_number->setText(ui->lineEdit_number->text().remove(ui->lineEdit_number->text().length()-1,1));
        return;
    }

    CreateTransaction();
}

void CapitalTransferPage::addressChangeSlots(const QString &address)
{
    ui->lineEdit_address->setText( ui->lineEdit_address->text().remove(" "));
    ui->lineEdit_address->setText( ui->lineEdit_address->text().remove("\n"));
    if( ui->lineEdit_address->text().isEmpty() || !validateAddress(address) )
    {
        ui->lineEdit_address->setStyleSheet("color:red");
    }
    else
    {
        ui->lineEdit_address->setStyleSheet("color:#5474EB");
    }

    CreateTransaction();
}

bool CapitalTransferPage::validateAddress(const QString &address)
{
    return !address.isEmpty();
}

void CapitalTransferPage::PostQueryTunnelMoney(const QString &symbol, const QString &tunnelAddress)
{
    if(symbol.isEmpty() || tunnelAddress.isEmpty()) return;
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Address.GetBalance");
    QJsonObject paramObject;
    paramObject.insert("chainId",symbol);
    paramObject.insert("addr",tunnelAddress);
    object.insert("params",paramObject);
    _p->httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}

void CapitalTransferPage::CreateTransaction()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    if(_p->tunnel_account_address.isEmpty() || _p->multisig_address.isEmpty() ||
       _p->symbol.isEmpty() || _p->actualNumber.isEmpty() || ui->lineEdit_number->text().isEmpty())
    {
        _p->actualNumber = "0";
        ui->label_tip->setVisible(false);
        ui->toolButton_confirm->setEnabled(false);
        return;
    }

    AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(_p->symbol));
    if(_p->symbol == "ETH")
    {
        _p->actualNumber = QString::number(ui->lineEdit_number->text().toDouble() - 0.00021,'f',_p->precision);
        if(ui->lineEdit_number->text().toDouble() < 0.001)
        {
            ui->label_tip->setText(tr("number cannot less than ") + "0.001");
            ui->label_tip->setVisible(true);
            ui->toolButton_confirm->setEnabled(false);
            return;
        }
    }
    else if(_p->symbol.startsWith("ERC"))
    {
        _p->actualNumber = ui->lineEdit_number->text().toDouble();
        if(ui->lineEdit_number->text().toDouble() < 0.001)
        {
            ui->label_tip->setText(tr("number cannot less than ") + "0.001");
            ui->label_tip->setVisible(true);
            ui->toolButton_confirm->setEnabled(false);
            return;
        }
    }
    else
    {
        _p->actualNumber = QString::number(ui->lineEdit_number->text().toDouble() - getBigNumberString( assetInfo.fee, assetInfo.precision).toDouble(),'f',_p->precision);
        double extraNumber = _p->asset_max_ammount.toDouble() - ui->lineEdit_number->text().toDouble();
        //qDebug()<<_p->actualNumber<<extraNumber<<dust_number[_p->symbol];
        if(_p->actualNumber.toDouble() < dust_number[_p->symbol])
        {
            ui->label_tip->setText(tr("number cannot less than ") + QString::number(dust_number[_p->symbol] + getBigNumberString( assetInfo.fee, assetInfo.precision).toDouble(),'f',_p->precision));
            ui->label_tip->setVisible(true);
            ui->toolButton_confirm->setEnabled(false);
            return;
        }
        else if(extraNumber > 1e-10 && extraNumber < dust_number[_p->symbol])
        {
            ui->label_tip->setText(tr("balance left should not less than ")+QString::number(dust_number[_p->symbol],'f',_p->precision));
            ui->label_tip->setVisible(true);
            ui->toolButton_confirm->setEnabled(false);
            return;
        }
    }

    ui->label_tip->setVisible(false);
    if(ui->radioButton_deposit->isChecked())
    {//充值修改，发送充值指令问题
        HXChain::getInstance()->postRPC( "captial-createrawtransaction",
                                         toJsonFormat( "createrawtransaction", QJsonArray()
                                         << _p->tunnel_account_address<<_p->multisig_address<<_p->actualNumber<<_p->symbol ));
    }
    else if(ui->radioButton_withdraw->isChecked())
    {//划转，账户不能时自己的账户
        if(_p->tunnel_account_address == ui->lineEdit_address->text())
        {
            //提示不能给自己划转

        }
        else
        {
            HXChain::getInstance()->postRPC( "captial-createrawtransaction",
                                             toJsonFormat( "createrawtransaction", QJsonArray()
                                             << _p->tunnel_account_address<<ui->lineEdit_address->text()<<_p->actualNumber<<_p->symbol ));
        }
    }

}

void CapitalTransferPage::getMarkNumber()
{
    QString tip = tr(" %1 is pending!");
    double number = HXChain::getInstance()->mainFrame->crossMark->CalTransaction(_p->account_name,_p->symbol);
    qDebug()<<"get---get---"<<number;
    if(number > dust_number[_p->symbol])
    {//说明有划出去钱，显示提示
        ui->label_marktip->setText(tip.arg(QString::number(number,'g',_p->precision)));
        ui->label_marktip->setVisible(true);
    }
    else
    {
        ui->label_marktip->setVisible(false);
    }
}

void CapitalTransferPage::InitWidget()
{
    InitStyle();

    ui->label_tip->setWordWrap(true);
    ui->label_tip->setVisible(false);


    if(_p->symbol == "ETH")
    {
        ui->label_fee->setText( tr("about") + " 0.00021 ETH");
    }
    else if(_p->symbol.startsWith("ERC"))
    {
        ui->label_fee->setText( tr("about") + " 0.00045 ETH");
    }
    else
    {
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(_p->symbol));
        ui->label_fee->setText( getBigNumberString(assetInfo.fee, assetInfo.precision) + " " +_p->symbol);
    }


//    ui->toolButton_confirm->setVisible(false);
    ui->toolButton_confirm->setEnabled(false);
    ui->radioButton_deposit->setChecked(true);
    ui->lineEdit_address->setPlaceholderText(tr("select withdraw to input address"));
    ui->lineEdit_address->setEnabled(false);
    ui->lineEdit_number->setPlaceholderText(tr("please wait"));

    connect( HXChain::getInstance(), &HXChain::jsonDataUpdated, this, &CapitalTransferPage::jsonDataUpdated);
    connect(&_p->httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    connect(ui->toolButton_close,&QToolButton::clicked,std::bind(&CapitalTransferPage::backBtnVisible,this,false));
    connect(ui->toolButton_close,&QToolButton::clicked,this,&CapitalTransferPage::close);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&CapitalTransferPage::ConfirmSlots);
    connect(ui->radioButton_deposit,&QRadioButton::toggled,this,&CapitalTransferPage::radioChangedSlots);
    connect(ui->radioButton_withdraw,&QRadioButton::toggled,this,&CapitalTransferPage::radioChangedSlots);

    connect(ui->lineEdit_number,&QLineEdit::textEdited,this,&CapitalTransferPage::numberChangeSlots);
    connect(ui->lineEdit_address,&QLineEdit::textEdited,this,&CapitalTransferPage::addressChangeSlots);

    //记录标志
    ui->label_marktip->setVisible(false);
    connect(HXChain::getInstance()->mainFrame->crossMark,&CrossCapitalMark::updateMark,this,&CapitalTransferPage::getMarkNumber);

    //获取通道账户，获取多签地址，用于充值使用
    HXChain::getInstance()->postRPC( "captial-get_binding_account",
                                     toJsonFormat( "get_binding_account", QJsonArray()
                                     << _p->account_name<<_p->symbol ));
    HXChain::getInstance()->postRPC( "captial-get_current_multi_address",
                                     toJsonFormat( "get_current_multi_address", QJsonArray()
                                     << _p->symbol));
    //查询缓存提现,HC目前解析交易有问题（decoderawtransaction），不进行查询
    if("HC" != _p->symbol)
    {
        HXChain::getInstance()->mainFrame->crossMark->checkUpData(_p->account_name,_p->symbol);
    }


}

void CapitalTransferPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);

//    ui->toolButton_close->setIconSize(QSize(12,12));
//    ui->toolButton_close->setIcon(QIcon(":/ui/wallet_ui/close.png"));

    ui->toolButton_close->setStyleSheet(CLOSEBTN_STYLE);
    ui->toolButton_confirm->setStyleSheet(OKBTN_STYLE);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->label_back);
//    setStyleSheet("QToolButton#toolButton_confirm{color:white;\
//                                      border-top-left-radius:10px;  \
//                                      border-top-right-radius:10px; \
//                                      border-bottom-left-radius:10px;  \
//                                      border-bottom-right-radius:10px; \
//                                      border:none;\
//                                      background-color:#4861DC;}"
//                  "QToolButton#toolButton_close::hover,QToolButton#toolButton_confirm::hover{background-color:#00D2FF;}"
//                  "QToolButton#toolButton_close{border:none;background:transparent;color:#4861DC;}"
//                  "QRadioButton{color:#4861DC;}"
//                  "QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
//                  );
}

void CapitalTransferPage::paintEvent(QPaintEvent *event)
{
//    QPainter painter(this);

////    painter.setPen(Qt::NoPen);
////    painter.setBrush(QColor(255,255,255,240));//最后一位是设置透明属性（在0-255取值）
////    painter.drawRect(rect());

//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(255,255,255,255));
//    painter.drawRoundedRect(QRect(195,80,380,370),10,10);

//    QRadialGradient radial(385, 385, 390, 385,385);
//    radial.setColorAt(0, QColor(0,0,0,15));
//    radial.setColorAt(1, QColor(218,255,248,15));
//    painter.setBrush(radial);
//    painter.setPen(Qt::NoPen);
//    painter.drawRoundedRect(QRect(190,75,390,380),10,10);

    QWidget::paintEvent(event);
}
