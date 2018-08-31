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
    palette.setColor(QPalette::Window, QColor(229,226,240));
    setPalette(palette);

    QFont font("\"Microsoft YaHei UI Light\"",14,63);
    font.setPixelSize(20);
    ui->title_label->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::black);
    ui->title_label->setPalette(pa);
    ui->toolButton->setIconSize(QSize(26,26));
    ui->toolButton->setIcon(QIcon(":/ui/wallet_ui/addContact.png"));
    ui->toolButton->setStyleSheet("QToolButton{border:none;background:transparent;}");

//    setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/addContact_1.png);background-repeat: no-repeat;background-position: center;background-color:transparent;border:none;}"   \
//                                  "QToolButton:hover{background-image:url(:/ui/wallet_ui/addContact.png);");


}
