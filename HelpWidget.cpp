#include "HelpWidget.h"
#include "ui_HelpWidget.h"

HelpWidget::HelpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
    InitWidget();
}

HelpWidget::~HelpWidget()
{
    delete ui;
}

void HelpWidget::InitWidget()
{
    InitStyle();
}

void HelpWidget::InitStyle()
{
    setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
             "QToolButton:hover{background-color:#00D2FF;}"
                  "QTextBrowser{background-color:transparent;color:#333333;border:none;font-size:12px;font-family:Microsoft YaHei UI Light;}");

}
