#include "PublishPoundageWidget.h"
#include "ui_PublishPoundageWidget.h"

#include <QDateTime>
#include "PoundageDataUtil.h"

class PublishPoundageWidget::PublishPoundageWidgetPrivate
{
public:
    PublishPoundageWidgetPrivate()
        :poundage(std::make_shared<PoundageUnit>())
    {

    }
public:
    std::shared_ptr<PoundageUnit> poundage;
};

PublishPoundageWidget::PublishPoundageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PublishPoundageWidget),
    _p(new PublishPoundageWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

PublishPoundageWidget::~PublishPoundageWidget()
{
    delete _p;
    delete ui;
}

void PublishPoundageWidget::ConfirmPublishSlots()
{
    if(!_p->poundage) return;
    if(_p->poundage->targetCoinType.isEmpty()) return;

    _p->poundage->publishTime = QDateTime::currentDateTime().toString("yyyy/M/d HH:mm");

    close();
}

void PublishPoundageWidget::InitWidget()
{
    InitStyle();

    connect(ui->pushButton_confirm,&QPushButton::clicked,this,&PublishPoundageWidget::ConfirmPublishSlots);
    connect(ui->pushButton_cancel,&QPushButton::clicked,this,&PublishPoundageWidget::close);
}

void PublishPoundageWidget::InitStyle()
{

}
