#include "contacteditdialog.h"
#include "ui_contacteditdialog.h"

ContactEditDialog::ContactEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactEditDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
}

ContactEditDialog::~ContactEditDialog()
{
    delete ui;
}

void ContactEditDialog::on_editBtn_clicked()
{
    choice = 1;
    close();
}

void ContactEditDialog::on_deleteBtn_clicked()
{
    choice = 2;
    close();
}
