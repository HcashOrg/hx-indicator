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
    if("id_wallet_create_crosschain_symbol" == id)
    {
        //解析返回的通道地址，并且绑定地址
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;
        result.prepend("{");
        result.append("}");
        //qDebug()<<"crossAddress"<<result;

        RefreshQRWidget(result);
    }
    else if("id_bind_tunnel_account" == id)
    {
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;
        result.prepend("{");
        result.append("}");
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
    UBChain::getInstance()->postRPC("id_wallet_create_crosschain_symbol",
                                    toJsonFormat("wallet_create_crosschain_symbol",
                                                 QJsonArray()<<_p->assetSymbol)
                                    );
}

void DepositPage::QueryDepositRecord()
{

}

void DepositPage::RefreshQRWidget(const QString &queryResult)
{
    DepositDataUtil::ParseTunnelData(queryResult,_p->tunnelData);
    _p->qrcodeWidget->SetQRString(_p->tunnelData->address);
    //绑定账户
    UBChain::getInstance()->postRPC("id_bind_tunnel_account",
                                    toJsonFormat("bind_tunnel_account",
                                                 QJsonArray()<<_p->name<<_p->tunnelData->address<<_p->assetSymbol<<true));
}

void DepositPage::InitWidget()
{
    InitStyle();

    ui->stackedWidget->addWidget(_p->qrcodeWidget);
    ui->stackedWidget->setCurrentWidget(_p->qrcodeWidget);
    connect( UBChain::getInstance(), &UBChain::jsonDataUpdated, this, &DepositPage::jsonDataUpdated);

    GenerateAddress();

    _p->qrcodeWidget->SetSymbol(_p->assetSymbol);
}

void DepositPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,249,253));
    setPalette(palette);

    QFont font("MicrosoftYaHeiLight",20,63);
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
