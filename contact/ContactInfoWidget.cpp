#include "ContactInfoWidget.h"
#include "ui_ContactInfoWidget.h"

#include <QPainter>
#include "ContactDataUtil.h"

#include "ContactInfoTitleWidget.h"
#include "ContactInfoHistoryWidget.h"

class ContactInfoWidget::ContactInfoWidgetPrivate
{
public:
    ContactInfoWidgetPrivate()
        :person(nullptr)
        ,infoTitleWidget(new ContactInfoTitleWidget())
        ,infoHistoryWidget(new ContactInfoHistoryWidget())
    {

    }
public:
    std::shared_ptr<ContactPerson> person;

    ContactInfoTitleWidget *infoTitleWidget;
    ContactInfoHistoryWidget *infoHistoryWidget;
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

void ContactInfoWidget::PersonModifyFinishedSlots()
{
    _p->infoTitleWidget->refreshName();
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
   // _p->infoTitleWidget->setStyleSheet("background-color:rgba(10, 10, 10,100);");

    ui->scrollArea_top->setWidget(_p->infoTitleWidget);
    ui->scrollArea_bottom->setWidget(_p->infoHistoryWidget);
    connect(_p->infoTitleWidget,&ContactInfoTitleWidget::transferAccountSignal,this,&ContactInfoWidget::transferAccountSlots);
}

void ContactInfoWidget::RefreshWidget()
{
    _p->infoTitleWidget->setData(_p->person);
    _p->infoHistoryWidget->showTransferRecord(_p->person->address);
}

void ContactInfoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(rect());

    QWidget::paintEvent(event);
}
