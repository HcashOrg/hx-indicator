#include "ContactInfoTitleWidget.h"
#include "ui_ContactInfoTitleWidget.h"

#include <QClipboard>
#include <QPainter>
#include <QMouseEvent>
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
    if(!_p->person) return;
    ui->label_name->setText(_p->person->name);
    ui->label_address->setText(_p->person->address);
    ui->label_address->setToolTip(tr("复制"));
}

void ContactInfoTitleWidget::refreshName()
{
    if(!_p->person) return;
    ui->label_name->setText(_p->person->name);
}

void ContactInfoTitleWidget::InitWidget()
{
    //setStyleSheet("background-color:#FFFFFF;");
    InitStyle();

    ui->pushButton_copy->setVisible(false);
    connect(ui->pushButton_copy,&QPushButton::clicked,this,&ContactInfoTitleWidget::copyAdressSlots);
    connect(ui->pushButton_transfer,&QPushButton::clicked,this,&ContactInfoTitleWidget::transferAccountSlots);
}

void ContactInfoTitleWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    setStyleSheet("QPushButton#pushButton_transfer{color:white;font-size:11px;font-weight:bold;\
                  border-top-left-radius:10px;  \
                  border-top-right-radius:10px; \
                  border-bottom-left-radius:10px;  \
                  border-bottom-right-radius:10px; \
                  border:none;\
                  background-color:#00D2FF;\}"
                  "QPushButton#pushButton_transfer::hover{background-color:#4861DC;}");
}

void ContactInfoTitleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.save();
    const qreal radius = 10;
    QRectF rect = QRect(QPoint(50, 5), QSize(670,70));
    QRectF rect1 = QRect(QPoint(45, 0), QSize(680,80));


    QRadialGradient radial(385, 385, 770, 385,385);
        radial.setColorAt(0, QColor(0,0,0,15));
        radial.setColorAt(1, QColor(218,255,248,15));

    painter.setBrush(radial);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect1,10,10);

    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect,10,10);


    painter.restore();


    QWidget::paintEvent(event);
}
#include<QDebug>
void ContactInfoTitleWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        qDebug()<<event->pos();
        if(ui->label_address==childAt(event->pos()))
        {
            if(_p->person)
            {
                QApplication::clipboard()->setText(_p->person->address);

            }

        }
    }
    QWidget::mousePressEvent(event);
}
