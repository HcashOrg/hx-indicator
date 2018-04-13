#include "FunctionAccountWidget.h"
#include "ui_FunctionAccountWidget.h"

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

void FunctionAccountWidget::AssetShowSlots()
{
    ui->toolButton_miner->setChecked(false);
    ui->toolButton_asset->setChecked(true);
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
    palette.setColor(QPalette::Background, QColor(94,116,235));
    setPalette(palette);

    QFont font("MicrosoftYaHeiLight",14,63);
    ui->label_normal->setFont(font);
    ui->label_advance->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(248,249,254));
    ui->label_normal->setPalette(pa);
    ui->label_advance->setPalette(pa);

    setStyleSheet("QToolButton{background:rgb(94,116,235);border:none;}\
                   QToolButton:hover{background-color: rgb(0,210, 255);}\
                   QToolButton:pressed{background-color: rgb(130,157, 255);}\
                   QToolButton:checked{background-color: rgb(130,157, 255);}");
}
