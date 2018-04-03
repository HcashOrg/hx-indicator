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
//    setAutoFillBackground(true);
//    QPalette palette;
//    palette.setColor(QPalette::Background, QColor(40,46,66));
//    setPalette(palette);

    //setStyleSheet("QWidget{background-color:rgb(24,28,45);border:none;}");

    connect(ui->pushButton,&QPushButton::clicked,[this](){
       this->emit addNewContactSignal();
    });
}

//void ContactBriefWidget::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
//    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
//    painter.drawRect(rect());
//}
