#include "FunctionGuardWidget.h"
#include "ui_FunctionGuardWidget.h"

#include "extra/style.h"

FunctionGuardWidget::FunctionGuardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionGuardWidget)
{
    ui->setupUi(this);

    InitWidget();
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
    ui->keyManageBtn->setCheckable(true);
    ui->proposalBtn->setCheckable(true);
    ui->withdrawConfirmBtn->setCheckable(true);
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
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(false);
    showAssetSignal();
}

void FunctionGuardWidget::on_keyManageBtn_clicked()
{
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(true);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(false);
    showKeyManageSignal();
}

void FunctionGuardWidget::on_proposalBtn_clicked()
{
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(true);
    ui->withdrawConfirmBtn->setChecked(false);
    showProposalSignal();
}

void FunctionGuardWidget::on_withdrawConfirmBtn_clicked()
{
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(true);
    showWithdrawConfirmSignal();
}
