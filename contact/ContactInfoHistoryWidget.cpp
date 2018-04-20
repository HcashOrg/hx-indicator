#include "ContactInfoHistoryWidget.h"
#include "ui_ContactInfoHistoryWidget.h"

class ContactInfoHistoryWidget::ContactInfoHistoryWidgetPrivate
{
public:
    ContactInfoHistoryWidgetPrivate()
    {

    }
public:

};

ContactInfoHistoryWidget::ContactInfoHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactInfoHistoryWidget),
    _p(new ContactInfoHistoryWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ContactInfoHistoryWidget::~ContactInfoHistoryWidget()
{
    delete _p;
    delete ui;
}

void ContactInfoHistoryWidget::InitWidget()
{
    InitStyle();
}

void ContactInfoHistoryWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

}
