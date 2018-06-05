#include "ErrorResultDialog.h"
#include "ui_ErrorResultDialog.h"

ErrorResultDialog::ErrorResultDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorResultDialog)
{
    ui->setupUi(this);
}

ErrorResultDialog::~ErrorResultDialog()
{
    delete ui;
}
