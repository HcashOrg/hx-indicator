#include "PublishPoundageWidget.h"
#include "ui_PublishPoundageWidget.h"

#include <QDateTime>
#include "PoundageDataUtil.h"
#include "wallet.h"

Q_DECLARE_METATYPE(AccountInfo)
Q_DECLARE_METATYPE(AssetAmount)

class PublishPoundageWidget::PublishPoundageWidgetPrivate
{
public:
    PublishPoundageWidgetPrivate()
        :poundage(std::make_shared<PoundageUnit>())
    {

    }
public:
    std::shared_ptr<PoundageUnit> poundage;
};

PublishPoundageWidget::PublishPoundageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PublishPoundageWidget),
    _p(new PublishPoundageWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

PublishPoundageWidget::~PublishPoundageWidget()
{
    delete _p;
    delete ui;
}

void PublishPoundageWidget::ConfirmPublishSlots()
{
    //发送指令，创建承税单

    QString accountName = ui->comboBox_accounts->currentText();
    QString sourceNumber = QString::number(ui->doubleSpinBox_sourceNumber->value());
    QString targetNumber = QString::number(ui->doubleSpinBox_targetNumber->value());
    QString targetCoinType = ui->comboBox_targetType->currentText();
    if(accountName.isEmpty() || targetCoinType.isEmpty()) return;
    
    UBChain::getInstance()->postRPC("id_create_guarantee_order",
                                    toJsonFormat("create_guarantee_order",
                                                 QJsonArray()<<accountName<<sourceNumber<<targetNumber<<targetCoinType<<true));
    close();
}

void PublishPoundageWidget::ChangeAccountSlots()
{
    ui->doubleSpinBox_sourceNumber->setRange(0,0);
    AccountInfo accountInfo = ui->comboBox_accounts->currentData(Qt::UserRole).value<AccountInfo>();

    if(accountInfo.assetAmountMap.empty())return;

    for(auto it = accountInfo.assetAmountMap.constBegin();it != accountInfo.assetAmountMap.constEnd();++it)
    {
        if(it.key() == "1.3.0")
        {
            ui->doubleSpinBox_sourceNumber->setRange(0,it.value().amount);
            break;
        }
    }
}

void PublishPoundageWidget::InitAccount()
{
    ui->comboBox_accounts->clear();
    ui->doubleSpinBox_sourceNumber->setRange(0,0);
    if(UBChain::getInstance()->accountInfoMap.empty()) return;

    for(auto it = UBChain::getInstance()->accountInfoMap.constBegin();it != UBChain::getInstance()->accountInfoMap.constEnd();++it)
    {
        ui->comboBox_accounts->addItem(it.key(),QVariant::fromValue<AccountInfo>(it.value()));
    }
    ui->comboBox_accounts->setCurrentIndex(0);

}

void PublishPoundageWidget::InitTargetCoin()
{
    ui->comboBox_targetType->clear();
    ui->doubleSpinBox_targetNumber->setRange(0,1e10);
    if(UBChain::getInstance()->assetInfoMap.empty()) return;
    foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
        if(asset.id == "1.3.0") continue;
        ui->comboBox_targetType->addItem(asset.symbol,asset.id);
    }

    ui->comboBox_targetType->setCurrentIndex(0);

}

void PublishPoundageWidget::InitWidget()
{
    InitStyle();

    InitAccount();
    InitTargetCoin();

    connect(ui->pushButton_confirm,&QPushButton::clicked,this,&PublishPoundageWidget::ConfirmPublishSlots);
    connect(ui->pushButton_cancel,&QPushButton::clicked,this,&PublishPoundageWidget::close);
    connect(ui->comboBox_accounts, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this,&PublishPoundageWidget::ChangeAccountSlots);
}

void PublishPoundageWidget::InitStyle()
{

}
