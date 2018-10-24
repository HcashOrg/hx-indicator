#include "WithdrawInputWidget.h"
#include "ui_WithdrawInputWidget.h"

#include <QPainter>
#include <QDoubleValidator>
#include "wallet.h"
#include "commondialog.h"

#include "crossmark/AddressValidate.h"

class WithdrawInputWidget::DataPrivate
{
public:
    DataPrivate()
    {

    }
public:
    QString chainType;
    AddressValidate addrValid;
};

WithdrawInputWidget::WithdrawInputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawInputWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

WithdrawInputWidget::~WithdrawInputWidget()
{
    delete _p;
    delete ui;
}

void WithdrawInputWidget::InitData(const QString &number, const QString &symbol)
{
    _p->chainType = symbol;
    ui->lineEdit_ammount->setPlaceholderText(tr("Max: %1").arg(number));
    AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value( HXChain::getInstance()->getAssetId(symbol));
    QDoubleValidator *validator = new QDoubleValidator(getBigNumberString(assetInfo.withdrawLimit, assetInfo.precision).toDouble(),
                                                       number.toDouble(), assetInfo.precision, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_ammount->setValidator( validator );
    ui->label_symbol->setText(symbol);
    ui->label_tipNumber->setText(tr("Amount limits: %1 to %2").arg(validator->bottom()).arg(number));
}

void WithdrawInputWidget::addressChangeSlots(const QString &address)
{
    _p->addrValid.startValidateAddress(_p->chainType,address);

    checkConfirmBtnEnabled();
}

void WithdrawInputWidget::numberChangeSlots(const QString &number)
{
    checkConfirmBtnEnabled();

    QDoubleValidator* via = dynamic_cast<QDoubleValidator*>(const_cast<QValidator*>(ui->lineEdit_ammount->validator()));
    if(!via)
    {
        return;
    }
    if(number.isEmpty())
    {
        ui->label_tipNumber->setVisible(false);
    }
    else if(ui->lineEdit_ammount->text().toDouble() > via->top())
    {
        ui->lineEdit_ammount->setText(ui->lineEdit_ammount->text().remove(ui->lineEdit_ammount->text().length()-1,1));
        ui->label_tipNumber->setVisible(true);
    }
    else if(ui->lineEdit_ammount->text().toDouble() < via->bottom())
    {
        ui->label_tipNumber->setVisible(true);
    }
    else
    {
        ui->label_tipNumber->setVisible(false);
    }

}

void WithdrawInputWidget::maxButtonSlots()
{
    QDoubleValidator* vali = dynamic_cast<QDoubleValidator*>(const_cast<QValidator*>(ui->lineEdit_ammount->validator()));
    if(vali)
    {
        ui->lineEdit_ammount->setText(QString::number(vali->top(),'f',vali->decimals()));
    }
}

void WithdrawInputWidget::confirmButtonSlots()
{ 
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    QDoubleValidator* via = dynamic_cast<QDoubleValidator*>(const_cast<QValidator*>(ui->lineEdit_ammount->validator()));
    if(!via)
    {
        return;
    }
    if(ui->lineEdit_ammount->text().toDouble() < via->bottom()
       )
    {
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("withdraw amount shouldn't < %1!").arg(via->bottom()).toStdString().c_str());
        dia.pop();
    }
    else if(ui->lineEdit_ammount->text().toDouble() > via->top())
    {
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("withdraw amount shouldn't > %1!").arg(via->top()).toStdString().c_str());
        dia.pop();
    }
    else
    {
        emit withdrawSignal(ui->lineEdit_address->text(),ui->lineEdit_ammount->text());
    }

}

void WithdrawInputWidget::addressValidateSlot(bool va)
{
    qDebug()<<va;
    if(va)
    {
        ui->label_tipAddr->setVisible(false);
    }
    else
    {
        ui->label_tipAddr->setVisible(true);
    }


}

bool WithdrawInputWidget::validateAddress(const QString &address)
{
    return !address.isEmpty();
}

void WithdrawInputWidget::checkConfirmBtnEnabled()
{
    if(!ui->lineEdit_address->text().isEmpty() && ui->lineEdit_ammount->text().toDouble() > 0)
    {
        ui->toolButton_confirm->setEnabled(true);
    }
    else
    {
        ui->toolButton_confirm->setEnabled(false);
    }
}


void WithdrawInputWidget::InitWidget()
{
    InitStyle();
    ui->label_tipNumber->setVisible(false);
    ui->label_tipAddr->setVisible(false);

    QRegExp regx("[a-zA-Z0-9]+$");
    QValidator *validator1 = new QRegExpValidator(regx, this);
    ui->lineEdit_address->setValidator( validator1 );

    QDoubleValidator *validator = new QDoubleValidator(0,0,0,ui->lineEdit_ammount);
    ui->lineEdit_ammount->setValidator( validator );

    connect(ui->lineEdit_address,&QLineEdit::textEdited,this,&WithdrawInputWidget::addressChangeSlots);
    connect(ui->toolButton_all,&QToolButton::clicked,this,&WithdrawInputWidget::maxButtonSlots);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&WithdrawInputWidget::confirmButtonSlots);

    connect(ui->lineEdit_ammount,&QLineEdit::textChanged,this,&WithdrawInputWidget::numberChangeSlots);
    ui->lineEdit_address->setFocus();

    connect(&_p->addrValid,&AddressValidate::AddressValidateSignal,this,&WithdrawInputWidget::addressValidateSlot);

    checkConfirmBtnEnabled();
}

void WithdrawInputWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);
    ui->toolButton_confirm->setStyleSheet(OKBTN_STYLE);
    HXChain::getInstance()->mainFrame->installBlurEffect(ui->label_back);
}
void WithdrawInputWidget::paintEvent(QPaintEvent *event)
{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);
//    painter.save();
//    const qreal radius = 10;
//    QPainterPath path;
//    QRectF rect = QRect(50, 10, 680, 220);
//    QRectF rect1 = QRect(45,5,690,230);

//    QRadialGradient radial(375, 200, 375, 375,200);
//    radial.setColorAt(0, QColor(0,0,0,15));
//    radial.setColorAt(1, QColor(218,255,248,15));

//    painter.setBrush(radial);
//    painter.setPen(Qt::NoPen);
//    painter.drawRoundedRect(rect1,radius,radius);

//    painter.setBrush(QBrush(Qt::white));
//    painter.setPen(Qt::NoPen);
//    painter.drawRoundedRect(rect,radius,radius);

//    painter.restore();
    QWidget::paintEvent(event);
}

