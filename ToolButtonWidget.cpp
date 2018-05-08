#include "ToolButtonWidget.h"
#include "ui_ToolButtonWidget.h"

static const QString BUTTONSTYLESHEET="QToolButton{background-color:rgb(%1,%2,%3); border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                                      "QToolButton:hover{background-color:#00D2FF;}";
ToolButtonWidget::ToolButtonWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolButtonWidget)
{
    ui->setupUi(this);
    InitWidget();
}

ToolButtonWidget::~ToolButtonWidget()
{
    delete ui;
}

void ToolButtonWidget::setText(const QString &text)
{
    ui->toolButton->setText(text);
}

void ToolButtonWidget::setInitGray(bool gray)
{
    if(gray)
    {
        setStyleSheet("QToolButton{background-color:#E5E5E5; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                 "QToolButton:hover{background-color:#00D2FF;color:white;}");

    }
    else
    {
        setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                 "QToolButton:hover{background-color:#00D2FF;color:white;}");
    }
}

void ToolButtonWidget::setInitNone(bool non)
{
    if(non)
    {
        setStyleSheet("QToolButton{background-color:transparent; border:none;border-radius:10px;color: transparent;}"
                 "QToolButton:hover{background-color:#00D2FF;color:white;}");
    }
}

void ToolButtonWidget::setInitColor(const QColor color)
{
    setStyleSheet(BUTTONSTYLESHEET.arg(color.red()).arg(color.green()).arg(color.blue()));
}

void ToolButtonWidget::InitWidget()
{
    InitStyle();
    connect(ui->toolButton,&QToolButton::clicked,this,&ToolButtonWidget::clicked);
}

void ToolButtonWidget::InitStyle()
{
    setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
             "QToolButton:hover{background-color:#00D2FF;color:white;}");
}

ToolButtonWidgetItem::ToolButtonWidgetItem(int _row, int _column, QWidget *parent)
{
    row = _row;
    column = _column;

    connect(this,SIGNAL(clicked(bool)),this,SLOT(onButtonClicked()));
}

void ToolButtonWidgetItem::onButtonClicked()
{
    emit itemClicked(row,column);
}
