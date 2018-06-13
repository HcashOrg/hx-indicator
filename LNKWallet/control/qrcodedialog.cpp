#include "qrcodedialog.h"
#include "ui_qrcodedialog.h"

#include "qrcodewidget.h"

QRCodeDialog::QRCodeDialog(QString address, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QRCodeDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);

    setObjectName("qrcodeDialog");
    setStyleSheet("#qrcodeDialog{border:1px solid rgb(173,173,173);background-color:white;}");

    QRCodeWidget* qrCodeWidget = new QRCodeWidget(this);
    qrCodeWidget->setGeometry(6,6,87,87);
    qrCodeWidget->setString(address);
}


QRCodeDialog::~QRCodeDialog()
{
    delete ui;
}
