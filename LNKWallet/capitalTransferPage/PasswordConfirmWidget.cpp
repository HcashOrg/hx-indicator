#include "PasswordConfirmWidget.h"
#include "ui_PasswordConfirmWidget.h"

#include <QPainter>
#include "wallet.h"
PasswordConfirmWidget::PasswordConfirmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PasswordConfirmWidget)
{
    ui->setupUi(this);
    InitWidget();
}

PasswordConfirmWidget::~PasswordConfirmWidget()
{
    delete ui;
}

void PasswordConfirmWidget::ConfirmSlots()
{
    emit confirmSignal();
    close();
}

void PasswordConfirmWidget::CancelSlots()
{
    emit cancelSignal();
    close();
}

void PasswordConfirmWidget::passwordChangeSlots(const QString &address)
{
    ui->toolButton_confirm->setEnabled(false);
    if( ui->lineEdit->text().size() < 8)
    {
        ui->lineEdit->setStyleSheet("color:red");
        return;
    }

    HXChain::getInstance()->postRPC( "captial_unlock-lockpage", toJsonFormat( "unlock", QJsonArray() << ui->lineEdit->text() ));

}

void PasswordConfirmWidget::jsonDataUpdated(QString id)
{
    if( id == "captial_unlock-lockpage")
    {
        QString  result = HXChain::getInstance()->jsonDataValue(id);
        if( result == "\"result\":null")
        {
            ui->lineEdit->setStyleSheet("color:#5474EB");
            ui->toolButton_confirm->setEnabled(true);
        }
        else
        {
            ui->lineEdit->setStyleSheet("color:red");
            ui->toolButton_confirm->setEnabled(false);
        }
    }
}

void PasswordConfirmWidget::passwordReturnPressed()
{
    if(ui->toolButton_confirm->isEnabled())
    {
        ConfirmSlots();
    }
}

void PasswordConfirmWidget::InitWidget()
{
    InitStyle();

    ui->toolButton_confirm->setEnabled(false);
    ui->lineEdit->setPlaceholderText(tr("please input password..."));
    connect( HXChain::getInstance(), &HXChain::jsonDataUpdated, this, &PasswordConfirmWidget::jsonDataUpdated);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&PasswordConfirmWidget::ConfirmSlots);
    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&PasswordConfirmWidget::CancelSlots);
    connect(ui->lineEdit,&QLineEdit::textEdited,this,&PasswordConfirmWidget::passwordChangeSlots);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&PasswordConfirmWidget::passwordReturnPressed);

}

void PasswordConfirmWidget::InitStyle()
{
    setAttribute(Qt::WA_TranslucentBackground, true);

//    setAutoFillBackground(true);
//    QPalette palette;
//    palette.setColor(QPalette::Window, QColor(10,10,10,100));
//    setPalette(palette);

    ui->toolButton_confirm->setStyleSheet(OKBTN_STYLE);
    ui->toolButton_cancel->setStyleSheet(CLOSEBTN_STYLE);
    setStyleSheet(
                  "QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
                  );

}

void PasswordConfirmWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(10,10,10,100));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(rect());

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,255,255,255));
    painter.drawRoundedRect(QRect(385,220,380,185),10,10);

    QRadialGradient radial(385, 385, 390, 385,385);
    radial.setColorAt(0, QColor(0,0,0,15));
    radial.setColorAt(1, QColor(218,255,248,15));
    painter.setBrush(radial);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRect(380,215,390,195),10,10);

    QWidget::paintEvent(event);
}
