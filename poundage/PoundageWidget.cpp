#include "PoundageWidget.h"
#include "ui_PoundageWidget.h"

#include "PublishPoundageWidget.h"

class PoundageWidget::PoundageWidgetPrivate
{
public:
    PoundageWidgetPrivate()
    {

    }
public:

};

PoundageWidget::PoundageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PoundageWidget),
    _p(new PoundageWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

PoundageWidget::~PoundageWidget()
{
    delete _p;
    delete ui;
}

void PoundageWidget::PublishPoundageSlots()
{
    PublishPoundageWidget *publishWidget = new PublishPoundageWidget(this);
    publishWidget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog);
    publishWidget->setWindowModality(Qt::WindowModal);
    publishWidget->setAttribute(Qt::WA_DeleteOnClose);
    publishWidget->show();
}

void PoundageWidget::InitWidget()
{
    InitStyle();

    connect(ui->pushButton_publishPoundage,&QPushButton::clicked,this,&PoundageWidget::PublishPoundageSlots);
}

void PoundageWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(40,46,66));
    setPalette(palette);
}
