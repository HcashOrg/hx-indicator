#include "PublishPoundageWidget.h"
#include "ui_PublishPoundageWidget.h"

#include <QDateTime>
#include "PoundageDataUtil.h"
#include "wallet.h"
#include "commondialog.h"

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
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    //发送指令，创建承税单

    QString accountName = ui->comboBox_accounts->currentText();
    QString sourceNumber = ui->lineEdit_source->text();
    QString targetNumber = ui->lineEdit_target->text();
    QString targetCoinType = ui->comboBox_targetType->currentText();
    if(accountName.isEmpty() || targetCoinType.isEmpty()) return;
    
    //限定数量大于0
    if(sourceNumber.toDouble() < 1e-20 || targetNumber.toDouble() < 1e-20)
    {
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("number cannot be zero!"));
        dia.pop();
        return;
    }
    qDebug()<<"publish_create_create----";
    HXChain::getInstance()->postRPC("publish_create_guarantee_order",
                                    toJsonFormat("create_guarantee_order",
                                                 QJsonArray()<<accountName<<sourceNumber<<targetNumber<<targetCoinType<<true));
    emit backBtnVisible(false);
    close();
}

void PublishPoundageWidget::ChangeAccountSlots()
{
    ui->lineEdit_source->clear();
//    AccountInfo accountInfo = ui->comboBox_accounts->currentData(Qt::UserRole).value<AccountInfo>();
    AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(ui->comboBox_accounts->currentText());

    if(accountInfo.assetAmountMap.isEmpty())
    {
        ui->lineEdit_source->setEnabled(false);
        ui->lineEdit_source->setPlaceholderText(tr("0 LNK"));
        return;
    }
    for(auto it = accountInfo.assetAmountMap.constBegin();it != accountInfo.assetAmountMap.constEnd();++it)
    {
        if(it.key() == "1.3.0")
        {
//            installDoubleValidator(ui->lineEdit_source,0,it.value().amount/pow(10.,ASSET_PRECISION),ASSET_PRECISION);

            QString amountStr = getBigNumberString(it.value().amount, ASSET_PRECISION);
            ui->lineEdit_source->setPlaceholderText(tr("Max: %1 %2").arg(amountStr).arg(ASSET_NAME) );

            QRegExp rx1(QString("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(ASSET_PRECISION));
            QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
            ui->lineEdit_source->setValidator(pReg1);
            ui->lineEdit_source->clear();

            ui->lineEdit_source->setEnabled(true);
            if(it.value().amount < 1)
            {
                ui->lineEdit_source->setEnabled(false);
                ui->lineEdit_source->setPlaceholderText(tr("0 LNK"));
            }
            break;
        }
    }
}

void PublishPoundageWidget::ChangeAssetSlots()
{
    if(ui->comboBox_targetType->currentText().isEmpty())
    {
        ui->lineEdit_target->clear();
        ui->lineEdit_target->setEnabled(false);
        ui->lineEdit_target->setPlaceholderText(tr("no chains!"));
        return;
    }
    ui->lineEdit_target->setEnabled(true);
    ui->lineEdit_target->clear();
    int pre = ui->comboBox_targetType->currentData(Qt::UserRole).value<int>();
    installDoubleValidator(ui->lineEdit_target,0,1e20,pre);
    ui->lineEdit_target->setPlaceholderText(tr("input ")+ui->comboBox_targetType->currentText()+tr(" number"));
    //ui->doubleSpinBox_targetNumber->setDecimals(pre);
}

void PublishPoundageWidget::InitAccount()
{
    ui->comboBox_accounts->clear();
    if(HXChain::getInstance()->accountInfoMap.isEmpty()) return;

    for(auto it = HXChain::getInstance()->accountInfoMap.constBegin();it != HXChain::getInstance()->accountInfoMap.constEnd();++it)
    {
        ui->comboBox_accounts->addItem(it.key(),QVariant::fromValue<AccountInfo>(it.value()));
    }
    ui->comboBox_accounts->setCurrentIndex(0);
}

void PublishPoundageWidget::InitTargetCoin()
{
    ui->comboBox_targetType->clear();
    if(HXChain::getInstance()->assetInfoMap.isEmpty()) return;
    foreach(AssetInfo asset,HXChain::getInstance()->assetInfoMap){
        if(asset.id == "1.3.0") continue;

        ui->comboBox_targetType->addItem(asset.symbol,asset.precision);
    }

    ui->comboBox_targetType->setCurrentIndex(0);

}

void PublishPoundageWidget::InitWidget()
{
    InitStyle();

    InitAccount();
    InitTargetCoin();

    connect(ui->okBtn,&QToolButton::clicked,this,&PublishPoundageWidget::ConfirmPublishSlots);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&PublishPoundageWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,std::bind(&PublishPoundageWidget::backBtnVisible,this,false));
    connect(ui->comboBox_accounts, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this,&PublishPoundageWidget::ChangeAccountSlots);
    connect(ui->comboBox_targetType,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this,&PublishPoundageWidget::ChangeAssetSlots);

    ChangeAccountSlots();
    ChangeAssetSlots();

    ui->label_fee->setText("  "+HXChain::getInstance()->feeChargeInfo.poundagePublishFee+" LNK");

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->label_back);
}

void PublishPoundageWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    //ui->label_fee->setFrameShape(QFrame::Box);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
}

void PublishPoundageWidget::paintEvent(QPaintEvent *event)
{
//    QPainter painter(this);

//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(255,255,255,255));
//    painter.drawRoundedRect(QRect(50,120,674,287),10,10);

//    QRadialGradient radial(385, 265, 380, 385,265);
//    radial.setColorAt(0, QColor(0,0,0,15));
//    radial.setColorAt(1, QColor(218,255,248,15));
//    painter.setBrush(radial);
//    painter.setPen(Qt::NoPen);
//    painter.drawRoundedRect(QRect(45,115,684,297),10,10);

    QWidget::paintEvent(event);
}

void PublishPoundageWidget::installDoubleValidator(QLineEdit *editor, double min, double max, int pre)
{
    if(!editor) return;

    QDoubleValidator *validator = new QDoubleValidator(min,max,pre,this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    editor->setValidator(validator);
    editor->setPlaceholderText(tr("max:")+QString::number(validator->top(),'f',pre));
}

void PublishPoundageWidget::on_lineEdit_source_textEdited(const QString &arg1)
{

    QString assetId = HXChain::getInstance()->getAssetId(ASSET_NAME);
    AssetAmount assetAmount = HXChain::getInstance()->accountInfoMap.value(ui->comboBox_accounts->currentText()).assetAmountMap.value(assetId);
    QString amountStr = getBigNumberString(assetAmount.amount, ASSET_PRECISION);

    if(ui->lineEdit_source->text().toDouble() > amountStr.toDouble())
    {
        ui->lineEdit_source->setText(amountStr);
    }
}
