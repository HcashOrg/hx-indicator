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

    ui->doubleSpinBox_sourceNumber->setToolTip("maxNumber: 0");
    for(auto it = accountInfo.assetAmountMap.constBegin();it != accountInfo.assetAmountMap.constEnd();++it)
    {
        if(it.key() == "1.3.0")
        {
            ui->doubleSpinBox_sourceNumber->setRange(0,it.value().amount/100000.);
            ui->doubleSpinBox_sourceNumber->setToolTip("maxNumber: "+QString::number(it.value().amount/100000.));
            break;
        }
    }
}

void PublishPoundageWidget::InitAccount()
{
    ui->comboBox_accounts->clear();
    ui->doubleSpinBox_sourceNumber->setRange(0,0);
    ui->doubleSpinBox_sourceNumber->setToolTip("maxNumber: 0");
    if(UBChain::getInstance()->accountInfoMap.empty()) return;

    for(auto it = UBChain::getInstance()->accountInfoMap.constBegin();it != UBChain::getInstance()->accountInfoMap.constEnd();++it)
    {
        AccountInfo dd = it.value();
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


    ChangeAccountSlots();
}

void PublishPoundageWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    QFont font("Microsoft YaHei UI Light",12,50);
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0,0,0));
    ui->label->setPalette(pa);
    ui->label->setFont(font);
    ui->label_2->setPalette(pa);
    ui->label_2->setFont(font);
    ui->label_3->setPalette(pa);
    ui->label_3->setFont(font);
    ui->label_4->setPalette(pa);
    ui->label_4->setFont(font);

    QPalette feepa;
    feepa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_fee->setPalette(feepa);


    setStyleSheet("QPushButton#pushButton_confirm{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                  "QPushButton#pushButton_confirm:hover{background-color:#00D2FF;}"

                  "QPushButton#pushButton_cancel{background-color:#E5E5E5; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                  "QPushButton#pushButton_cancel:hover{background-color:#00D2FF;}"

                  "QDoubleSpinBox::up-button {width:0;height:0;}"
                  "QDoubleSpinBox::down-button {width:0;height:0;}"
                  "QDoubleSpinBox::up-arrow {width:0;height:0;}"
                  "QDoubleSpinBox::down-arrow {width:0;height:0;}"
                  "QDoubleSpinBox{background-color: transparent;border:none;color:black;font-size:12pt;}"
                  "QDoubleSpinBox:focus{border:none;}"
                  "QDoubleSpinBox:disabled{background:transparent;color: rgb(83,90,109);border:none;}"
                  "QComboBox{border: none;background:transparent;font-size: 12pt;font-family: MicrosoftYaHei;\
                             background-position: center left;color: black;selection-background-color: darkgray;}"

                  "QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
                  "QLineEdit:focus{border:none;}"
                );
}

void PublishPoundageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(255,255,255,240));//最后一位是设置透明属性（在0-255取值）
//    painter.drawRect(rect());

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,255,255,255));
    painter.drawRoundedRect(QRect(50,120,674,287),10,10);

    QRadialGradient radial(385, 265, 380, 385,265);
    radial.setColorAt(0, QColor(0,0,0,15));
    radial.setColorAt(1, QColor(218,255,248,15));
    painter.setBrush(radial);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRect(45,115,684,297),10,10);

    QWidget::paintEvent(event);
}
