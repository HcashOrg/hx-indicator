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
        ,assetID(data.assetID),actualAssetSymbol(data.assetType)
        ,qrcodeWidget(new DepositQrcodeWidget())
        ,recordWidget(nullptr)
        ,tunnelData(std::make_shared<TunnelData>())
    {
        if(actualAssetSymbol.startsWith("ERC"))
        {
            actualAssetSymbol = "ETH";
        }
    }
public:
    QString address;//地址
    QString name;//账户名
    QString assetSymbol;//资产名
    QString assetID;//资产ID
    QString actualAssetSymbol;//实际资产名，例如ERC开头的资产，实际资产是ETH

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
            if(_p->actualAssetSymbol == _p->assetSymbol)
            {
                QString tunn = HXChain::getInstance()->configFile->value(QString("/tunnel/%1%2").arg(_p->address).arg(_p->actualAssetSymbol)).toString();
                //如果本地没有缓存，则新建，否则生成一个
                if(tunn.isEmpty())
                {
                    //生成通道账户
                    GenerateAddress();
                }
                else
                {
                    //发送绑定信号
                    _p->tunnelData->address = tunn;

                    //绑定通道账户
                    CommonDialog dia(CommonDialog::OkOnly);
                    dia.setText(tr("tunnel account checked!please update sys-time!"));
                    dia.pop();

                    BindTunnelAccount();
                }
            }
            else
            {
                //如果是ERC这种资产//先查询有没有ETHtunnel
                GetActualBindTunnelAccount();

            }
        }
        else
        {

            //检测用户是否有对应tunnel的私钥
            checkTunnelPriKey();
        }

    }
    else if("deposit-dump_crosschain_private_keys" == id)
    {
        //寻找到是否有该地址对应的私钥
        QString result = HXChain::getInstance()->jsonDataValue( id);
        if(result.contains(_p->tunnle_address))
        {
            _p->qrcodeWidget->SetQRString(_p->tunnle_address);
        }
        else
        {
            //提醒用户私钥丢失
            _p->qrcodeWidget->SetQRString(tr("private-key-missing!"));
            CommonDialog dia(CommonDialog::OkOnly);
            dia.setText(tr("Cann't find pri-key in your wallet!Please contact official team for supportting!"));
            dia.pop();
        }

    }
    else if("deposit_create_crosschain_symbol" == id)
    {
        //解析返回的通道地址，并且绑定地址
        QString result = HXChain::getInstance()->jsonDataValue( id);
//        qDebug() << id << result;
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
        if(_p->assetSymbol != _p->actualAssetSymbol)
        {
            BindActualTunnelAccount();
        }
        BindTunnelAccount();
    }
    else if(id.startsWith("deposit_bind_tunnel_account_"))
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        qDebug() << id << result;
        QString bindSymbol = id.mid(QString("deposit_bind_tunnel_account_").length());
        if( result.isEmpty() || result.startsWith("\"error"))
        {
            _p->qrcodeWidget->SetQRString(tr("cannot bind tunnel address.%1").arg(_p->tunnelData->address));
            return;
        }
        HXChain::getInstance()->configFile->setValue(QString("/tunnel/%1%2").arg(_p->address).arg(_p->actualAssetSymbol),_p->tunnelData->address);
        if(bindSymbol != _p->assetSymbol)
        {
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
    else if(id == "deposit_get_actual_binding_account")
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");
        //提取通道账户地址
        _p->tunnle_address = DepositDataUtil::parseTunnelAddress(result);
        if(_p->tunnle_address.isEmpty())
        {
            QString tunn = HXChain::getInstance()->configFile->value(QString("/tunnel/%1%2").arg(_p->address).arg(_p->actualAssetSymbol)).toString();
            if(tunn.isEmpty())
            {
                //生成实际资产
                GenerateAddress();
            }
            else
            {
                //发送绑定信号
                _p->tunnelData->address = tunn;
                //既然已经记录过，但是实际未绑定，可能是时间导致的
                CommonDialog dia(CommonDialog::OkOnly);
                dia.setText(tr("tunnel account checked!please update sys-time!"));
                dia.pop();
                //绑定tunnel
                BindActualTunnelAccount();
                BindTunnelAccount();
            }
        }
        else
        {
            //发送绑定信号
            _p->tunnelData->address = _p->tunnle_address;
            //绑定tunnel
            BindTunnelAccount();
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

void DepositPage::GetActualBindTunnelAccount()
{
    HXChain::getInstance()->postRPC( "deposit_get_actual_binding_account",
                                     toJsonFormat( "get_binding_account", QJsonArray()
                                     << _p->name<<_p->actualAssetSymbol ));
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
                                                 QJsonArray()<<_p->actualAssetSymbol)
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
    HXChain::getInstance()->postRPC("deposit_bind_tunnel_account_"+_p->assetSymbol,
                                    toJsonFormat("bind_tunnel_account",
                                                 QJsonArray()<<_p->name<<_p->tunnelData->address<<_p->assetSymbol<<true));
}

void DepositPage::BindActualTunnelAccount()
{
    //绑定账户
    if(!HXChain::getInstance()->ValidateOnChainOperation())
    {

        emit backBtnVisible(false);
        close();
        return;
    }

    HXChain::getInstance()->postRPC("deposit_bind_tunnel_account_"+_p->actualAssetSymbol,
                                        toJsonFormat("bind_tunnel_account",
                                                     QJsonArray()<<_p->name<<_p->tunnelData->address<<_p->actualAssetSymbol<<true));
}

void DepositPage::checkTunnelPriKey()
{//开始查找私钥
    HXChain::getInstance()->postRPC( "deposit-dump_crosschain_private_keys", toJsonFormat( "dump_crosschain_private_keys", QJsonArray()));

}

void DepositPage::InitWidget()
{
    InitStyle();

    ui->stackedWidget->addWidget(_p->qrcodeWidget);
    ui->stackedWidget->addWidget(_p->recordWidget);
    ui->stackedWidget->setCurrentWidget(_p->qrcodeWidget);
    connect( HXChain::getInstance(), &HXChain::jsonDataUpdated, this, &DepositPage::jsonDataUpdated);


    _p->fee = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.tunnelBindFee.toDouble(),ASSET_NAME,_p->name,
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
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);


    ui->depositRecordBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
}
