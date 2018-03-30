#include "ContactWidget.h"
#include "ui_ContactWidget.h"

#include "ContactDataUtil.h"
#include "ContactTreeWidget.h"
#include "ContactSearchWidget.h"
#include "ContactBriefWidget.h"
#include "ContactAddWidget.h"
#include "ContactInfoWidget.h"

class ContactWidget::ContactWidgetPrivate
{
public:
    ContactWidgetPrivate()
        :contactTreeWidget(new ContactTreeWidget())
        ,contactSearchWidget(new ContactSearchWidget())
        ,contactBriefWidget(new ContactBriefWidget())
        ,contactAddWidget(nullptr)
        ,contactInfoWidget(nullptr)
        ,contactSheet(nullptr)
    {

    }
public:
    ContactTreeWidget *contactTreeWidget;
    ContactSearchWidget *contactSearchWidget;
    ContactBriefWidget *contactBriefWidget;
    ContactAddWidget *contactAddWidget;
    ContactInfoWidget *contactInfoWidget;

    std::shared_ptr<ContactSheet> contactSheet;
};

ContactWidget::ContactWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactWidget),
    _p(new ContactWidgetPrivate())
{
    ui->setupUi(this);

    InitWidget();
    InitData();
}

ContactWidget::~ContactWidget()
{
    delete _p;
    delete ui;
}

void ContactWidget::AddNewContactSlots()
{
    if(!_p->contactAddWidget)
    {
        disconnect(_p->contactAddWidget,&ContactAddWidget::addContactFinishedSignal,_p->contactTreeWidget,&ContactTreeWidget::AddPersonSlots);
        disconnect(_p->contactTreeWidget,&ContactTreeWidget::GroupModifyFinishedSignal,_p->contactAddWidget,&ContactAddWidget::groupModifySlots);
    }
    _p->contactAddWidget = new ContactAddWidget(_p->contactSheet);
    ui->scrollArea_rightBottom->setWidget(_p->contactAddWidget);
    connect(_p->contactAddWidget,&ContactAddWidget::addContactFinishedSignal,_p->contactTreeWidget,&ContactTreeWidget::AddPersonSlots);
    connect(_p->contactTreeWidget,&ContactTreeWidget::GroupModifyFinishedSignal,_p->contactAddWidget,&ContactAddWidget::groupModifySlots);
}

void ContactWidget::ShowContactInfoSlots(const QString &address)
{
    _p->contactInfoWidget = new ContactInfoWidget();
    _p->contactInfoWidget->setData(_p->contactSheet->findPerson(address));
    ui->scrollArea_rightBottom->setWidget(_p->contactInfoWidget);
}

void ContactWidget::InitWidget()
{
    ui->scrollArea_leftBottom->setWidget(_p->contactTreeWidget);
    ui->scrollArea_leftTop->setWidget(_p->contactSearchWidget);
    ui->scrollArea_rightTop->setWidget(_p->contactBriefWidget);

    connect(_p->contactTreeWidget,&ContactTreeWidget::ShowContactPerson,this,&ContactWidget::ShowContactInfoSlots);
    connect(_p->contactSearchWidget,&ContactSearchWidget::searchSignal,_p->contactTreeWidget,&ContactTreeWidget::QueryPersonSlots);
    connect(_p->contactBriefWidget,&ContactBriefWidget::addNewContactSignal,this,&ContactWidget::AddNewContactSlots);
}

void ContactWidget::InitData()
{
     ContactDataUtil::readContactSheetFromPath("E:\\Project\\json2.txt",_p->contactSheet);
     _p->contactTreeWidget->initContactSheet(_p->contactSheet);
}

void ContactWidget::SaveData()
{
    ContactDataUtil::writeContactSheetToPath("E:\\Project\\json2.txt",_p->contactSheet);
}

void ContactWidget::closeEvent(QCloseEvent *event)
{
    SaveData();
    QWidget::closeEvent(event);
}
