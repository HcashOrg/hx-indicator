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
    close();
}

void PasswordConfirmWidget::CancelSlots()
{
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

    UBChain::getInstance()->postRPC( "id-unlock-lockpage", toJsonFormat( "unlock", QJsonArray() << ui->lineEdit->text() ));

}

void PasswordConfirmWidget::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-lockpage")
    {
        QString  result = UBChain::getInstance()->jsonDataValue(id);
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

void PasswordConfirmWidget::InitWidget()
{
    InitStyle();

    ui->toolButton_confirm->setEnabled(false);
    ui->lineEdit->setPlaceholderText(tr("please input password..."));
    connect( UBChain::getInstance(), &UBChain::jsonDataUpdated, this, &PasswordConfirmWidget::jsonDataUpdated);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&PasswordConfirmWidget::ConfirmSlots);
    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&PasswordConfirmWidget::CancelSlots);
    connect(ui->lineEdit,&QLineEdit::textEdited,this,&PasswordConfirmWidget::passwordChangeSlots);

}

void PasswordConfirmWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,249,253));
    setPalette(palette);

    setStyleSheet("QToolButton#toolButton_confirm{color:white;\
                                      border-top-left-radius:10px;  \
                                      border-top-right-radius:10px; \
                                      border-bottom-left-radius:10px;  \
                                      border-bottom-right-radius:10px; \
                                      border:none;\
                                      background-color:#4861DC;}"
                  "QToolButton#toolButton_confirm::hover{background-color:#00D2FF;}"
                  "QToolButton#toolButton_cancel{border:none;background:transparent;color:#4861DC;}"
                  "QRadioButton{color:#4861DC;}"
                  "QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
                  );

}

void PasswordConfirmWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(255,255,255,240));//最后一位是设置透明属性（在0-255取值）
//    painter.drawRect(rect());

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,255,255,255));
    painter.drawRoundedRect(QRect(195,170,380,185),10,10);

    QRadialGradient radial(385, 385, 390, 385,385);
    radial.setColorAt(0, QColor(0,0,0,15));
    radial.setColorAt(1, QColor(218,255,248,15));
    painter.setBrush(radial);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRect(190,165,390,195),10,10);

    QWidget::paintEvent(event);
}
