#include "commontip.h"
#include "ui_commontip.h"
#include "debug_log.h"

CommonTip::CommonTip( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommonTip)
{
    
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    
}

CommonTip::~CommonTip()
{
    
    delete ui;
    
}

void CommonTip::setText(QString text)
{
    ui->label->setText( text);
    ui->label->adjustSize();
    ui->label->move(0,0);
    setFixedSize( ui->label->size());
}
