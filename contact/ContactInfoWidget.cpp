#include "ContactInfoWidget.h"
#include "ui_ContactInfoWidget.h"

#include "ContactDataUtil.h"

#include "ContactInfoTitleWidget.h"

class ContactInfoWidget::ContactInfoWidgetPrivate
{
public:
    ContactInfoWidgetPrivate()
        :person(nullptr)
        ,infoTitleWidget(new ContactInfoTitleWidget())
    {

    }
public:
    std::shared_ptr<ContactPerson> person;

    ContactInfoTitleWidget *infoTitleWidget;
};

ContactInfoWidget::ContactInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactInfoWidget),
    _p(new ContactInfoWidgetPrivate)
{
    ui->setupUi(this);
    InitWidget();
}

ContactInfoWidget::~ContactInfoWidget()
{
    delete _p;
    delete ui;
}

void ContactInfoWidget::transferAccountSlots()
{
    if(!_p->person) return;
    emit gotoTransferPage(_p->person->address,_p->person->name);
}

void ContactInfoWidget::setData(const std::shared_ptr<ContactPerson> &person)
{
    _p->person = person;
    RefreshWidget();
}

void ContactInfoWidget::InitWidget()
{
    ui->scrollArea_top->setWidget(_p->infoTitleWidget);
    connect(_p->infoTitleWidget,&ContactInfoTitleWidget::transferAccountSignal,this,&ContactInfoWidget::transferAccountSlots);
}

void ContactInfoWidget::RefreshWidget()
{
    _p->infoTitleWidget->setData(_p->person);
}
