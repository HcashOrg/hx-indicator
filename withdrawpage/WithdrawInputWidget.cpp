#include "WithdrawInputWidget.h"
#include "ui_WithdrawInputWidget.h"

#include <QPainter>
#include <QDoubleValidator>
#include "wallet.h"

WithdrawInputWidget::WithdrawInputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawInputWidget)
{
    ui->setupUi(this);
    InitWidget();
}

WithdrawInputWidget::~WithdrawInputWidget()
{
    delete ui;
}

void WithdrawInputWidget::InitData(const QString &number, const QString &symbol)
{
    ui->lineEdit_ammount->setPlaceholderText(ui->lineEdit_ammount->placeholderText().replace("0",number));
    int pre = 5;
    foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
        if(asset.symbol == symbol)
        {
            pre = asset.precision;
            break;
        }
    }

    QDoubleValidator *validator = new QDoubleValidator(0,number.toDouble(),pre,this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_ammount->setValidator( validator );

    ui->label_symbol->setText(symbol);
}

void WithdrawInputWidget::addressChangeSlots(const QString &address)
{
    if(validateAddress(address))
    {
        ui->toolButton_confirm->setEnabled(true);
    }
    else
    {
        ui->toolButton_confirm->setEnabled(false);
    }
}

void WithdrawInputWidget::maxButtonSlots()
{
    ui->lineEdit_ammount->setText(QString::number(dynamic_cast<QDoubleValidator*>(const_cast<QValidator*>(ui->lineEdit_ammount->validator()))->top()));
}

void WithdrawInputWidget::confirmButtonSlots()
{
    emit withdrawSignal(ui->lineEdit_address->text(),ui->lineEdit_ammount->text());
}

bool WithdrawInputWidget::validateAddress(const QString &address)
{
    return !address.isEmpty();
}

void WithdrawInputWidget::InitWidget()
{
    InitStyle();

    QDoubleValidator *validator = new QDoubleValidator(0,0,0,ui->lineEdit_ammount);
    ui->lineEdit_ammount->setValidator( validator );

    ui->toolButton_confirm->setEnabled(false);
    connect(ui->lineEdit_address,&QLineEdit::textEdited,this,&WithdrawInputWidget::addressChangeSlots);
    connect(ui->toolButton_all,&QToolButton::clicked,this,&WithdrawInputWidget::maxButtonSlots);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&WithdrawInputWidget::confirmButtonSlots);

    ui->lineEdit_address->setFocus();
}

void WithdrawInputWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    setStyleSheet("QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
                  "Qline{color:#5474EB;background:#5474EB;}"
                  "QComboBox{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:3px;}"
                  "QToolButton#toolButton_all{border:none;background-color:transparent;border-radius:10px;color:#5474EB;font-size:9pt;}"
                  "QToolButton#toolButton_confirm::hover,QToolButton#toolButton_all::hover{background-color:#00D2FF;}"
                  "QToolButton#toolButton_confirm{border:none;background-color:#5474EB;color:white;border-radius:10px;font-size:12pt;}"
                  "QLabel{background:transparent;color:black:font-family:Microsoft YaHei UI Light;}");
}
void WithdrawInputWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.save();
    const qreal radius = 10;
    QPainterPath path;
    QRectF rect = QRect(50, 10, 680, 220);
    QRectF rect1 = QRect(45,5,690,230);

    QRadialGradient radial(375, 200, 375, 375,200);
    radial.setColorAt(0, QColor(0,0,0,15));
    radial.setColorAt(1, QColor(218,255,248,15));

    painter.setBrush(radial);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect1,radius,radius);

    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect,radius,radius);

    painter.restore();
    QWidget::paintEvent(event);
}
