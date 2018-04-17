#include "WithdrawInputWidget.h"
#include "ui_WithdrawInputWidget.h"

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

void WithdrawInputWidget::setMaxAmmount(double number)
{
    ui->lineEdit_ammount->setPlaceholderText(ui->lineEdit_ammount->placeholderText().replace("0",QString::number(number)));
    QDoubleValidator *validator = new QDoubleValidator(0,number,10,this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_ammount->setValidator( validator );
}

void WithdrawInputWidget::setSymbol(const QString &symbol)
{
    ui->label_symbol->setText(symbol);
}

void WithdrawInputWidget::addressChangeSlots(const QString &address)
{
    ui->lineEdit_address->setText( ui->lineEdit_address->text().remove(" "));
    ui->lineEdit_address->setText( ui->lineEdit_address->text().remove("\n"));
    if( ui->lineEdit_address->text().isEmpty())
    {
        ui->toolButton_confirm->setEnabled(false);
        return;
    }

    AddressType type = checkAddress(address,AccountAddress | ContractAddress | MultiSigAddress | ScriptAddress);
    if( type == AccountAddress)
    {
        ui->toolButton_confirm->setEnabled(true);
        return;
    }
    else
    {
        ui->toolButton_confirm->setEnabled(false);
        return;
    }
}

void WithdrawInputWidget::maxButtonSlots()
{
    ui->lineEdit_ammount->setText(QString::number(dynamic_cast<QDoubleValidator*>(const_cast<QValidator*>(ui->lineEdit_ammount->validator()))->top()));
}

void WithdrawInputWidget::confirmButtonSlots()
{
    emit withdrawSignal(ui->lineEdit_address->text(),ui->lineEdit_ammount->text().toDouble());
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
}

void WithdrawInputWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,249,253));
    setPalette(palette);

    setStyleSheet("QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
                  "Qline{color:#5474EB;background:#5474EB;}"
                  "QComboBox{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:3px;}"
                  "QToolButton#toolButton_all{border:none;background-color:transparent;border-radius:10px;color:#5474EB;font-size:9pt;}"
                  "QToolButton#toolButton_confirm::hover,QToolButton#toolButton_all::hover{background-color:#00D2FF;}"
                  "QToolButton#toolButton_confirm{border:none;background-color:#5474EB;color:white;border-radius:10px;font-size:12pt;}"
                  "QLabel{background:transparent;color:black:font-family:MicrosoftYaHeiLight;}");
}
