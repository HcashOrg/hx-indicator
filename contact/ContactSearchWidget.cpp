#include "ContactSearchWidget.h"
#include "ui_ContactSearchWidget.h"

#include <QPushButton>
class ContactSearchWidget::ContactSearchWidgetPrivate
{
public:
    ContactSearchWidgetPrivate()
    {

    }
public:

};

ContactSearchWidget::ContactSearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactSearchWidget),
    _p(new ContactSearchWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ContactSearchWidget::~ContactSearchWidget()
{
    delete _p;
    delete ui;
}

void ContactSearchWidget::StartSearchSlots()
{
    emit searchSignal(ui->lineEdit->text());
}

void ContactSearchWidget::InitWidget()
{
    QPushButton *pSearchButton = new QPushButton(this);

    pSearchButton->setCursor(Qt::PointingHandCursor);
    pSearchButton->setFixedSize(23, 23);
    pSearchButton->setToolTip(tr("Search"));

    pSearchButton->setStyleSheet("QPushButton{border-image:url(:/search.png); background:transparent;}");

    //防止文本框输入内容位于按钮之下
    QMargins margins = ui->lineEdit->textMargins();
    ui->lineEdit->setTextMargins(margins.left(), margins.top(), pSearchButton->width(), margins.bottom());
    ui->lineEdit->setPlaceholderText(tr("input name or address"));

    QHBoxLayout *pSearchLayout = new QHBoxLayout();
    pSearchLayout->addStretch();
    pSearchLayout->addWidget(pSearchButton);
    pSearchLayout->setSpacing(0);
    pSearchLayout->setContentsMargins(0, 0, 0, 0);
    ui->lineEdit->setLayout(pSearchLayout);

    connect(pSearchButton, &QPushButton::clicked, this, &ContactSearchWidget::StartSearchSlots);

    connect(ui->lineEdit,&QLineEdit::editingFinished,this, &ContactSearchWidget::StartSearchSlots);

    //setStyleSheet("QWidget{border: none;background-color: rgb(40, 46, 66);}");
}
