#include "ToolButtonWidget.h"
#include "ui_ToolButtonWidget.h"

#include <QDebug>

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
        ui->toolButton->setStyleSheet("QToolButton{background-color:#E5E5E5; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                                      "QToolButton:pressed{background-color:#465FBF;color:white;}");

    }
    else
    {
        ui->toolButton->setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                                      "QToolButton:pressed{background-color:#465FBF;color:white;}");
    }
}


void ToolButtonWidget::setEnabled(bool enabled)
{
    ui->toolButton->setEnabled(enabled);
}

void ToolButtonWidget::setButtonFixSize(int width, int height)
{
    ui->toolButton->setFixedWidth(width);
    ui->toolButton->setFixedHeight(height);
}

void ToolButtonWidget::setBackgroundColor(QString color)
{
    setStyleSheet(QString("#widget{background-color:%1;}").arg(color));
}

void ToolButtonWidget::setButtonStyle(const QString &style)
{
    ui->toolButton->setStyleSheet(style);
}

void ToolButtonWidget::setRedpointVisiable(bool is)
{
    redLabel->setVisible(is);
}

void ToolButtonWidget::InitWidget()
{
    InitStyle();
    connect(ui->toolButton,&QToolButton::clicked,this,&ToolButtonWidget::clicked);

    redLabel = new QLabel(ui->toolButton);
    redLabel->resize(8,8);
    redLabel->setPixmap(QPixmap(":/ui/wallet_ui/redpoint.png"));
    redLabel->raise();
    redLabel->setVisible(false);
}

void ToolButtonWidget::InitStyle()
{
    ui->toolButton->setStyleSheet("QToolButton{font: 11px \"Microsoft YaHei UI Light\";background:transparent; border:none;color: rgb(51, 51, 51);}"
                                  "QToolButton:hover{color:rgb(84,116,235);}"
                                  "QToolButton:disabled{color:rgb(200,200,200);}"
                                  );
    setButtonFixSize(60,20);
    setBackgroundColor("white");
}

void ToolButtonWidget::resizeEvent(QResizeEvent *event)
{
    ui->toolButton->setGeometry((this->width() - ui->toolButton->width()) / 2, (this->height() - ui->toolButton->height()) / 2,
                                ui->toolButton->width(), ui->toolButton->height());
    redLabel->move(ui->toolButton->width()-redLabel->width(),0);
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
