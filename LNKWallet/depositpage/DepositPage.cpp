#include "DepositPage.h"
#include "ui_DepositPage.h"

#include "wallet.h"
#include "DepositQrcodeWidget.h"
#include "DepositRecrdWideget.h"
#include "DepositDataUtil.h"
#include "FeeChargeWidget.h"
#include "commondialog.h"
#include "dialog/backupwalletdialog.h"

class DepositPage::DepositPagePrivate
{
public:
    DepositPagePrivate(const DepositDataInput & data)
        :address(data.accountAddress),name(data.accountName),assetSymbol(data.assetType)
        ,assetID(data.assetID)
        ,qrcodeWidget(new DepositQrcodeWidget())
        ,recordWidget(nullptr)
        ,tunnelData(std::make_shared<TunnelData>())
    {
    }
public:
    QString address;//地址
    QString name;
    QString assetSymbol;
    QString assetID;

    std::shared_ptr<TunnelData> tunnelData;//通道账户信息

    DepositQrcodeWidget *qrcodeWidget;
    DepositRecrdWideget *recordWidget;

    QString tunnle_address;
    FeeChargeWidget *fee ;
};

DepositPage::DepositPage(const DepositDataInput & data,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DepositPage),
    _p(new DepositPagePrivate(data))
{
    ui->setupUi(this);
    _p->recordWidget = new DepositRecrdWideget(this);
    InitWidget();

    //HXChain::getInstance()->ShowBubbleMessage("fdfd","fdsfds",5);
}

DepositPage::~DepositPage()
{
    delete _p;
    delete ui;
}

void DepositPage::jsonDataUpdated(QString id)
{
    if("deposit_get_binding_account" == id)
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");
    //提取通道账户地址
        _p->tunnle_address = DepositDataUtil::parseTunnelAddress(result);
        if(_p->tunnle_address.isEmpty())
        {
            //生成通道账户
            GenerateAddress();
        }
        else
        {
            _p->qrcodeWidget->SetQRString(_p->tunnle_address);
        }

    }
    else if("deposit_create_crosschain_symbol" == id)
    {
        //解析返回的通道地址，并且绑定地址
        QString result = HXChain::getInstance()->jsonDataValue( id);
        qDebug() << id << result;
        if( result.isEmpty() || result.startsWith("\"error"))
        {
            _p->qrcodeWidget->SetQRString(tr("cannot generate tunnel account"));
            return;
        }

        HXChain::getInstance()->autoSaveWalletFile();

        result.prepend("{");
        result.append("}");
        //qDebug()<<"crossAddress"<<result;

        DepositDataUtil::ParseTunnelData(result,_p->tunnelData);
        //_p->fee->isHidden()?_p->fee->show():0;
        //绑定通道账户
        BindTunnelAccount();
    }
    else if("deposit_bind_tunnel_account" == id)
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        qDebug() << id << result;
        if( result.isEmpty() || result.startsWith("\"error"))
        {
            _p->qrcodeWidget->SetQRString(tr("cannot bind tunnel account"));
            return;
        }
        result.prepend("{");
        result.append("}");
        _p->qrcodeWidget->SetQRString(_p->tunnelData->address);
        //qDebug()<<"tunnelrunnel"<<result;
        //提示保存信息
        HXChain::getInstance()->configFile->setValue("/settings/backupNeeded",true);
        HXChain::getInstance()->IsBackupNeeded = true;
        CommonDialog dia(CommonDialog::YesOrNo);
        dia.setText(tr("Wallet data updated! Please backup your wallet!"));
        if(dia.pop())
        {
            BackupWalletDialog backupWalletDialog;
            backupWalletDialog.pop();
        }

    }
}

void DepositPage::on_depositRecordBtn_clicked()
{
    //ui->stackedWidget->setCurrentWidget(_p->recordWidget);
    _p->recordWidget->setParent(this);
    _p->recordWidget->move(0,0);
    _p->recordWidget->show();
    _p->recordWidget->showDepositRecord(_p->tunnle_address);
}

void DepositPage::GetBindTunnelAccount()
{
    HXChain::getInstance()->postRPC( "deposit_get_binding_account",
                                     toJsonFormat( "get_binding_account", QJsonArray()
                                     << _p->name<<_p->assetSymbol ));

}

void DepositPage::GenerateAddress()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation())
    {
        emit backBtnVisible(false);
        close();
        return;
    }

    HXChain::getInstance()->postRPC("deposit_create_crosschain_symbol",
                                    toJsonFormat("create_crosschain_symbol",
                                                 QJsonArray()<<_p->assetSymbol)
                                    );
}

void DepositPage::BindTunnelAccount()
{
    //绑定账户
    if(!HXChain::getInstance()->ValidateOnChainOperation())
    {

        emit backBtnVisible(false);
        close();
        return;
    }
    HXChain::getInstance()->postRPC("deposit_bind_tunnel_account",
                                    toJsonFormat("bind_tunnel_account",
                                                 QJsonArray()<<_p->name<<_p->tunnelData->address<<_p->assetSymbol<<true));
}

void DepositPage::InitWidget()
{
    InitStyle();

    ui->stackedWidget->addWidget(_p->qrcodeWidget);
    ui->stackedWidget->addWidget(_p->recordWidget);
    ui->stackedWidget->setCurrentWidget(_p->qrcodeWidget);
    connect( HXChain::getInstance(), &HXChain::jsonDataUpdated, this, &DepositPage::jsonDataUpdated);


    _p->fee = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.tunnelBindFee.toDouble(),"HX",_p->name,
                                                    HXChain::getInstance()->mainFrame);
    _p->fee->raise();
    connect(_p->fee,&FeeChargeWidget::cancelSignal,this,&DepositPage::close);
    connect(_p->fee,&FeeChargeWidget::confirmSignal,this,&DepositPage::BindTunnelAccount);

    //查询通道账户，
    GetBindTunnelAccount();

    _p->qrcodeWidget->SetSymbol(_p->assetSymbol);
}

void DepositPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);


    ui->depositRecordBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
}
