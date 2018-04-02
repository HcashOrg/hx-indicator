#include "ContactBriefWidget.h"
#include "ui_ContactBriefWidget.h"

class ContactBriefWidget::ContactBriefWidgetPrivate
{
public:
    ContactBriefWidgetPrivate()
    {

    }
public:

};

ContactBriefWidget::ContactBriefWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactBriefWidget),
    _p(new ContactBriefWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ContactBriefWidget::~ContactBriefWidget()
{
    delete _p;
    delete ui;
}

void ContactBriefWidget::InitWidget()
{
    connect(ui->pushButton,&QPushButton::clicked,[this](){
       this->emit addNewContactSignal();
    });
}
