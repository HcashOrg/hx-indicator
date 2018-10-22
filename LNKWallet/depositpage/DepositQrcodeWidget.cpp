#include "DepositQrcodeWidget.h"
#include "ui_DepositQrcodeWidget.h"

#include <QClipboard>
#include <QPaintEvent>
#include <QPainter>

#include "wallet.h"
#include "DepositDataUtil.h"

static const QMap<QString,QString>  leastDepositAmountMap = {{"HC","0.01"},{"ETH","0.025"},{"ERCBCC","0"}};

DepositQrcodeWidget::DepositQrcodeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DepositQrcodeWidget)
{
    ui->setupUi(this);
    InitWidget();
}

DepositQrcodeWidget::~DepositQrcodeWidget()
{
    delete ui;
}

void DepositQrcodeWidget::SetQRString(const QString &data)
{
    QImage image = DepositDataUtil::CreateQRcodeImage(data);
    ui->label_qrcode->setPixmap(QPixmap::fromImage(image).scaled(ui->label_qrcode->size()));
    ui->label_address->setText(data);

    if(symbol.startsWith("ERC"))
    {
        ui->label_ercTip->show();
        PostQueryTunnelMoney("ETH",data);
    }
    else
    {
        ui->label_ercTip->hide();
    }
}

void DepositQrcodeWidget::SetSymbol(const QString &_symbol)
{
    symbol = _symbol;
    ui->label_warning->setText(ui->label_warning->text().replace("BTC",symbol));

    ui->tipLabel->setText(ui->tipLabel->text().replace("BTC",symbol));
    if(leastDepositAmountMap.contains(symbol))
    {
        ui->tipLabel->setText(ui->tipLabel->text().replace("0.001",leastDepositAmountMap.value(symbol)));
    }


}

void DepositQrcodeWidget::CopySlots()
{
    QApplication::clipboard()->setText(ui->label_address->text());
}

void DepositQrcodeWidget::PostQueryTunnelMoney(const QString &symbol, const QString &tunnelAddress)
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
    httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());

}

void DepositQrcodeWidget::httpReplied(QByteArray _data, int _status)
{
    qDebug() << "DepositQrcodeWidget httpReplied " << _data << _status;

    QJsonObject object  = QJsonDocument::fromJson(_data).object().value("result").toObject();

    QString balance = object.value("balance").toString();
    ui->label_ercTip->setText(ui->label_ercTip->text().replace("0 ETH", balance + "ETH"));
}

void DepositQrcodeWidget::InitWidget()
{
    InitStyle();

    ui->label_ercTip->hide();

    connect(ui->toolButton,&QToolButton::clicked,this,&DepositQrcodeWidget::CopySlots);
    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));
}

void DepositQrcodeWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);

    QFont font("\"Microsoft YaHei UI Light\"",14,50);
    ui->label_mark->setFont(font);
    ui->label_address->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::black);
    ui->label_mark->setPalette(pa);
    ui->label_address->setPalette(pa);

    QFont font1("\"Microsoft YaHei UI Light\"",10,50);
    ui->label_warning->setFont(font1);
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_warning->setPalette(pe);
    ui->tipLabel->setFont(font1);
    ui->tipLabel->setPalette(pe);

    ui->toolButton->setStyleSheet(TOOLBUTTON_STYLE_1);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->label_back);
}

void DepositQrcodeWidget::paintEvent(QPaintEvent *event)
{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);
//    painter.save();
//    const qreal radius = 10;
//    QRectF rect = QRect(QPoint(50, 10), QSize(670,380));
//    QRectF rect1 = QRect(QPoint(45, 5), QSize(680,390));


//    QRadialGradient radial(385, 385, 770, 385,385);
//    radial.setColorAt(0, QColor(0,0,0,15));
//    radial.setColorAt(1, QColor(218,255,248,15));

//    painter.setBrush(radial);
//    painter.setPen(Qt::NoPen);
//    painter.drawRoundedRect(rect1,10,10);

//    painter.setBrush(QBrush(Qt::white));
//    painter.setPen(Qt::NoPen);
//    painter.drawRoundedRect(rect,10,10);


//    painter.restore();


    QWidget::paintEvent(event);
}
