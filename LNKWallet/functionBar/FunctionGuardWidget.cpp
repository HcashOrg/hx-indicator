#include "FunctionGuardWidget.h"
#include "ui_FunctionGuardWidget.h"

#include "extra/style.h"

FunctionGuardWidget::FunctionGuardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionGuardWidget)
{
    ui->setupUi(this);
}

FunctionGuardWidget::~FunctionGuardWidget()
{
    delete ui;
}

void FunctionGuardWidget::retranslator()
{
    ui->retranslateUi(this);
}

void FunctionGuardWidget::DefaultShow()
{

}

void FunctionGuardWidget::InitWidget()
{
    InitStyle();
    ui->assetInfoBtn->setCheckable(true);
}

void FunctionGuardWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(84,116,235));
    setPalette(palette);

    setStyleSheet(FUNCTIONBAR_TOOLBUTTON_STYLE);

}

void FunctionGuardWidget::on_assetInfoBtn_clicked()
{
    ui->assetInfoBtn->setChecked(true);
    showAssetSignal();
}
