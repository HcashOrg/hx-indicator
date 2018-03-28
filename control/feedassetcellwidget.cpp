#include "feedassetcellwidget.h"
#include "ui_feedassetcellwidget.h"

#define FEEDASSETCELLWIDGET_STYLE_UNCHECKED     "background:transparent;border:1px solid rgb(70,82,113);border-radius:12px;color:rgb(192,196,212);"
#define FEEDASSETCELLWIDGET_STYLE_CHECKED       "background:rgb(70,82,113);border:1px solid rgb(70,82,113);border-radius:12px;color:white;"

FeedAssetCellWidget::FeedAssetCellWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeedAssetCellWidget)
{
    ui->setupUi(this);

    setChecked(false);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget{background-color:rgb(40,46,66);}");

}

FeedAssetCellWidget::~FeedAssetCellWidget()
{
    delete ui;
}

void FeedAssetCellWidget::setAssetName(QString _assetName)
{
    assetName = _assetName;
    ui->label->setText(assetName);
}

void FeedAssetCellWidget::setChecked(bool _isChecked)
{
    if(_isChecked)
    {
        ui->label->setStyleSheet(FEEDASSETCELLWIDGET_STYLE_CHECKED);
    }
    else
    {
        ui->label->setStyleSheet(FEEDASSETCELLWIDGET_STYLE_UNCHECKED);
    }
}
