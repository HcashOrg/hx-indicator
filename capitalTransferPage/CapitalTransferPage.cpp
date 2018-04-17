#include "CapitalTransferPage.h"
#include "ui_CapitalTransferPage.h"

#include <QPainter>

#include "wallet.h"

class CapitalTransferPage::CapitalTransferPagePrivate
{
public:
    CapitalTransferPagePrivate(const CapitalTransferInput &data)
        :account(data.account),symbol(data.symbol)
    {

    }
public:
    QString account;
    QString symbol;

    QString tunnel_account_address;
    QString asset_max_ammount;
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

    close();
}

void CapitalTransferPage::radioChangedSlots()
{
    if(ui->radioButton_deposit->isChecked())
    {
        ui->lineEdit_address->setPlaceholderText(tr("please wait"));
        if(!_p->tunnel_account_address.isEmpty())
        {
            ui->lineEdit_address->setText(_p->tunnel_account_address);
        }
        ui->lineEdit_address->setEnabled(false);
    }
    else if(ui->radioButton_withdraw->isChecked())
    {
        ui->lineEdit_address->setPlaceholderText(tr("please input withdraw address..."));
        ui->lineEdit_address->setEnabled(true);
    }

}

void CapitalTransferPage::jsonDataUpdated(QString id)
{

}

void CapitalTransferPage::updateData()
{
    ui->label_number->setText(_p->asset_max_ammount + "  " + _p->symbol);

    if(ui->radioButton_deposit->isChecked())
    {
        ui->lineEdit_address->setPlaceholderText(tr("please wait"));
        if(!_p->tunnel_account_address.isEmpty())
        {
            ui->lineEdit_address->setText(_p->tunnel_account_address);
        }
        ui->lineEdit_address->setEnabled(false);
    }
    else if(ui->radioButton_withdraw->isChecked())
    {
        ui->lineEdit_address->setPlaceholderText(tr("please input withdraw address..."));
        ui->lineEdit_address->setEnabled(true);
    }

    QDoubleValidator *validator = new QDoubleValidator(0,_p->asset_max_ammount.toDouble(),10,this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_number->setValidator(validator);
}

void CapitalTransferPage::InitWidget()
{
    InitStyle();

    ui->label_addressTitle->setText(ui->label_addressTitle->text().replace("X",_p->symbol));
    ui->radioButton_deposit->setChecked(true);
    ui->lineEdit_address->setPlaceholderText(tr("please wait"));
    ui->lineEdit_address->setEnabled(false);
    ui->lineEdit_number->setPlaceholderText(tr("money"));

    connect( UBChain::getInstance(), &UBChain::jsonDataUpdated, this, &CapitalTransferPage::jsonDataUpdated);
    connect(ui->toolButton_close,&QToolButton::clicked,this,&CapitalTransferPage::close);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&CapitalTransferPage::ConfirmSlots);
    connect(ui->radioButton_deposit,&QRadioButton::toggled,this,&CapitalTransferPage::radioChangedSlots);
    connect(ui->radioButton_withdraw,&QRadioButton::toggled,this,&CapitalTransferPage::radioChangedSlots);
}

void CapitalTransferPage::InitStyle()
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
                  "QToolButton#toolButton_close{border:none;background:transparent;color:#4861DC;}"
                  "QRadioButton{color:#4861DC;}"
                  "QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
);
}

void CapitalTransferPage::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(255,255,255,240));//最后一位是设置透明属性（在0-255取值）
//    painter.drawRect(rect());

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,255,255,255));
    painter.drawRoundedRect(QRect(195,80,380,370),10,10);

    QRadialGradient radial(385, 385, 390, 385,385);
    radial.setColorAt(0, QColor(0,0,0,15));
    radial.setColorAt(1, QColor(218,255,248,15));
    painter.setBrush(radial);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRect(190,75,390,380),10,10);

    QWidget::paintEvent(event);
}
