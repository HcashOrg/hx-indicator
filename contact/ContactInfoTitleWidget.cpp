#include "ContactInfoTitleWidget.h"
#include "ui_ContactInfoTitleWidget.h"

#include <QClipboard>
#include "ContactDataUtil.h"

class ContactInfoTitleWidget::ContactInfoTitleWidgetPrivate
{
public:
    ContactInfoTitleWidgetPrivate()
        :person(nullptr)
    {

    }
public:
    std::shared_ptr<ContactPerson> person;
};

ContactInfoTitleWidget::ContactInfoTitleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactInfoTitleWidget),
    _p(new ContactInfoTitleWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ContactInfoTitleWidget::~ContactInfoTitleWidget()
{
    delete _p;
    delete ui;
}

void ContactInfoTitleWidget::copyAdressSlots()
{
    if(!_p->person) return;

    QApplication::clipboard()->setText(_p->person->address);
}

void ContactInfoTitleWidget::transferAccountSlots()
{
    if(!_p->person) return;
    emit transferAccountSignal();
}

void ContactInfoTitleWidget::setData(const std::shared_ptr<ContactPerson> &person)
{
    _p->person = person;
    ui->label_name->setText(_p->person->name);
    ui->label_address->setText(_p->person->address);
}

void ContactInfoTitleWidget::InitWidget()
{
    connect(ui->pushButton_copy,&QPushButton::clicked,this,&ContactInfoTitleWidget::copyAdressSlots);
    connect(ui->pushButton_transfer,&QPushButton::clicked,this,&ContactInfoTitleWidget::transferAccountSlots);
}
