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

    connect(ui->toolButton,&QToolButton::clicked,this,&ContactBriefWidget::addNewContactSignal);
}

void ContactBriefWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);


//    ui->pushButton->setIconSize(QSize(18,18));
//    ui->pushButton->setIcon(QIcon(":/ui/wallet_ui/addContact.png"));

//    setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/addContact_1.png);background-repeat: no-repeat;background-position: center;background-color:transparent;border:none;}"   \
//                                  "QToolButton:hover{background-image:url(:/ui/wallet_ui/addContact.png);");


}
