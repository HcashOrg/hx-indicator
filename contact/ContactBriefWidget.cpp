#include "ContactBriefWidget.h"
#include "ui_ContactBriefWidget.h"

#include <QPainter>

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
    InitStyle();

    connect(ui->pushButton,&QPushButton::clicked,this,&ContactBriefWidget::addNewContactSignal);
}

void ContactBriefWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);


    ui->pushButton->setIconSize(QSize(18,18));
    ui->pushButton->setIcon(QIcon(":/ui/wallet_ui/addContact.png"));
    ui->pushButton->setStyleSheet("QPushButton{border-top-left-radius:10px;  \
                                  border-top-right-radius:10px; \
                                  border-bottom-left-radius:10px;  \
                                  border-bottom-right-radius:10px; \
                                  border:none;\
                                  background-color:#00D2FF;\}"
                                  "QPushButton::hover{background-color:#4861DC;}");
}
