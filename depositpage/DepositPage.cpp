#include "DepositPage.h"
#include "ui_DepositPage.h"

#include "wallet.h"
#include "DepositQrcodeWidget.h"
#include "DepositRecrdWideget.h"
#include "DepositDataUtil.h"

class DepositPage::DepositPagePrivate
{
public:
    DepositPagePrivate(const DepositDataInput & data)
        :address(data.accountAddress),name(data.accountName),assetSymbol(data.assetType)
        ,assetID(data.assetID)
        ,qrcodeWidget(new DepositQrcodeWidget())
        ,recordWidget(new DepositRecrdWideget())
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
};

DepositPage::DepositPage(const DepositDataInput & data,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DepositPage),
    _p(new DepositPagePrivate(data))
{
    ui->setupUi(this);
    InitWidget();
}

DepositPage::~DepositPage()
{
    delete _p;
    delete ui;
}

void DepositPage::jsonDataUpdated(QString id)
{
    if("id_get_binding_account" == id)
    {
        QString result = UBChain::getInstance()->jsonDataValue( id);
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
    else if("id_create_crosschain_symbol" == id)
    {
        //解析返回的通道地址，并且绑定地址
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() || result.startsWith("\"error"))
        {
            _p->qrcodeWidget->SetQRString(tr("cannot generate tunnel account"));
            return;
        }
        result.prepend("{");
        result.append("}");
        //qDebug()<<"crossAddress"<<result;

        DepositDataUtil::ParseTunnelData(result,_p->tunnelData);
        //绑定账户
        UBChain::getInstance()->postRPC("id_bind_tunnel_account",
                                        toJsonFormat("bind_tunnel_account",
                                                     QJsonArray()<<_p->name<<_p->tunnelData->address<<_p->assetSymbol<<true));
    }
    else if("id_bind_tunnel_account" == id)
    {
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() || result.startsWith("\"error"))
        {
            _p->qrcodeWidget->SetQRString(tr("cannot bind tunnel account"));
            return;
        }
        result.prepend("{");
        result.append("}");
        _p->qrcodeWidget->SetQRString(_p->tunnelData->address);
        //qDebug()<<"tunnelrunnel"<<result;
    }
}

void DepositPage::ShowRecordSlots()
{
    QueryDepositRecord();
    ui->stackedWidget->setCurrentWidget(_p->recordWidget);
}

void DepositPage::GenerateAddress()
{
    UBChain::getInstance()->postRPC("id_create_crosschain_symbol",
                                    toJsonFormat("create_crosschain_symbol",
                                                 QJsonArray()<<_p->assetSymbol)
                                    );
}

void DepositPage::QueryDepositRecord()
{

}

void DepositPage::InitWidget()
{
    InitStyle();

    ui->stackedWidget->addWidget(_p->qrcodeWidget);
    ui->stackedWidget->setCurrentWidget(_p->qrcodeWidget);
    connect( UBChain::getInstance(), &UBChain::jsonDataUpdated, this, &DepositPage::jsonDataUpdated);

    //查询通道账户，
    UBChain::getInstance()->postRPC( "id_get_binding_account",
                                     toJsonFormat( "get_binding_account", QJsonArray()
                                     << _p->name<<_p->assetSymbol ));


    _p->qrcodeWidget->SetSymbol(_p->assetSymbol);
}

void DepositPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    QFont font("Microsoft YaHei UI Light",20,63);
    ui->label->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::black);
    ui->label->setPalette(pa);

    ui->toolButton->setStyleSheet("QToolButton{color:white;\
                                  border-top-left-radius:10px;  \
                                  border-top-right-radius:10px; \
                                  border-bottom-left-radius:10px;  \
                                  border-bottom-right-radius:10px; \
                                  border:none;\
                                  background-color:#00D2FF;\}"
                                  "QToolButton::hover{background-color:#4861DC;}");
}
