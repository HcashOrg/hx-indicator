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
    on_accountInfoBtn_clicked();
}

void FunctionGuardWidget::InitWidget()
{
    InitStyle();
    ui->accountInfoBtn->setCheckable(true);
    ui->myIncomeBtn->setCheckable(true);
    ui->assetInfoBtn->setCheckable(true);
    ui->keyManageBtn->setCheckable(true);
    ui->proposalBtn->setCheckable(true);
    ui->withdrawConfirmBtn->setCheckable(true);
    ui->feedPriceBtn->setCheckable(true);
    ui->coldHotTransferBtn->setCheckable(true);
}

void FunctionGuardWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(84,116,235));
    setPalette(palette);

    setStyleSheet(FUNCTIONBAR_TOOLBUTTON_STYLE);
}

void FunctionGuardWidget::on_accountInfoBtn_clicked()
{
    ui->accountInfoBtn->setChecked(true);
    ui->myIncomeBtn->setChecked(false);
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(false);
    ui->feedPriceBtn->setChecked(false);
    ui->coldHotTransferBtn->setChecked(false);
    showGuardAccountSignal();
}


void FunctionGuardWidget::on_myIncomeBtn_clicked()
{
    ui->accountInfoBtn->setChecked(false);
    ui->myIncomeBtn->setChecked(true);
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(false);
    ui->feedPriceBtn->setChecked(false);
    ui->coldHotTransferBtn->setChecked(false);
    showGuardIncomeSignal();;
}

void FunctionGuardWidget::on_assetInfoBtn_clicked()
{
    ui->accountInfoBtn->setChecked(false);
    ui->myIncomeBtn->setChecked(false);
    ui->assetInfoBtn->setChecked(true);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(false);
    ui->feedPriceBtn->setChecked(false);
    ui->coldHotTransferBtn->setChecked(false);
    showAssetSignal();
}

void FunctionGuardWidget::on_keyManageBtn_clicked()
{
    ui->accountInfoBtn->setChecked(false);
    ui->myIncomeBtn->setChecked(false);
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(true);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(false);
    ui->feedPriceBtn->setChecked(false);
    ui->coldHotTransferBtn->setChecked(false);
    showKeyManageSignal();
}

void FunctionGuardWidget::on_proposalBtn_clicked()
{
    ui->accountInfoBtn->setChecked(false);
    ui->myIncomeBtn->setChecked(false);
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(true);
    ui->withdrawConfirmBtn->setChecked(false);
    ui->feedPriceBtn->setChecked(false);
    ui->coldHotTransferBtn->setChecked(false);
    showProposalSignal();
}

void FunctionGuardWidget::on_withdrawConfirmBtn_clicked()
{
    ui->accountInfoBtn->setChecked(false);
    ui->myIncomeBtn->setChecked(false);
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(true);
    ui->feedPriceBtn->setChecked(false);
    ui->coldHotTransferBtn->setChecked(false);
    showWithdrawConfirmSignal();
}

void FunctionGuardWidget::on_feedPriceBtn_clicked()
{
    ui->accountInfoBtn->setChecked(false);
    ui->myIncomeBtn->setChecked(false);
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(false);
    ui->feedPriceBtn->setChecked(true);
    ui->coldHotTransferBtn->setChecked(false);
    showFeedPriceSignal();
}

void FunctionGuardWidget::on_coldHotTransferBtn_clicked()
{
    ui->accountInfoBtn->setChecked(false);
    ui->myIncomeBtn->setChecked(false);
    ui->assetInfoBtn->setChecked(false);
    ui->keyManageBtn->setChecked(false);
    ui->proposalBtn->setChecked(false);
    ui->withdrawConfirmBtn->setChecked(false);
    ui->feedPriceBtn->setChecked(false);
    ui->coldHotTransferBtn->setChecked(true);
    showColdHotTransferSignal();
}


