#include "DepositQrcodeWidget.h"
#include "ui_DepositQrcodeWidget.h"

#include <QClipboard>
#include <QPaintEvent>
#include <QPainter>

#include "wallet.h"
#include "DepositDataUtil.h"

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
}

void DepositQrcodeWidget::SetSymbol(const QString &symbol)
{
    ui->label_warning->setText(ui->label_warning->text().replace("BTC",symbol));
    ui->tipLabel->setText(ui->tipLabel->text().replace("BTC",symbol));
}

void DepositQrcodeWidget::CopySlots()
{
    QApplication::clipboard()->setText(ui->label_address->text());
}

void DepositQrcodeWidget::InitWidget()
{
    InitStyle();

    connect(ui->toolButton,&QToolButton::clicked,this,&DepositQrcodeWidget::CopySlots);
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
