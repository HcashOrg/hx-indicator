#include "FunctionAccountWidget.h"
#include "ui_FunctionAccountWidget.h"

#include "extra/style.h"

FunctionAccountWidget::FunctionAccountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionAccountWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionAccountWidget::~FunctionAccountWidget()
{
    delete ui;
}
void FunctionAccountWidget::retranslator()
{
    ui->retranslateUi(this);
}
void FunctionAccountWidget::DefaultShow()
{
    AssetShowSlots();
}

void FunctionAccountWidget::AssetShowSlots()
{
    ui->toolButton_miner->setChecked(false);
    ui->toolButton_asset->setChecked(true);

    emit showAccountSignal();
}

void FunctionAccountWidget::MinerShowSlots()
{
    ui->toolButton_miner->setChecked(true);
    ui->toolButton_asset->setChecked(false);

    emit showMinerSignal();
}

void FunctionAccountWidget::InitWidget()
{
    InitStyle();
    ui->toolButton_asset->setCheckable(true);
    ui->toolButton_miner->setCheckable(true);

    connect(ui->toolButton_asset,&QToolButton::clicked,this,&FunctionAccountWidget::AssetShowSlots);
    connect(ui->toolButton_miner,&QToolButton::clicked,this,&FunctionAccountWidget::MinerShowSlots);
}

void FunctionAccountWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(94,116,235));
    setPalette(palette);

    setStyleSheet(FUNCTIONBAR_TOOLBUTTON_STYLE);
}
