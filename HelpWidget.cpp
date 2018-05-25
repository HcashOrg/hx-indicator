#include "HelpWidget.h"
#include "ui_HelpWidget.h"

#include <QPainter>
#include "wallet.h"
#include "update/UpdateProcess.h"

class HelpWidget::DataPrivate
{
public:
    DataPrivate()

    {

    }

public:

};

HelpWidget::HelpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

HelpWidget::~HelpWidget()
{
    delete ui;
}

void HelpWidget::CheckUpdateSlot()
{
    connect(UBChain::getInstance()->updateProcess,&UpdateProcess::NewstVersionSignal,this,&HelpWidget::CheckResultSlot);
    UBChain::getInstance()->updateProcess->checkUpdate();
}

void HelpWidget::CheckResultSlot(const QString &version)
{
    if(version.isEmpty())
    {
        //没有更新
        ui->label_updatetip->setVisible(true);
        ui->label_updatetip->setText(tr("no new version!"));
        ui->toolButton_update->setVisible(false);
    }
    else
    {
        ui->label_updatetip->setVisible(true);
        ui->label_updatetip->setText(tr("new version found! " )+ version);
        ui->toolButton_update->setVisible(true);
    }
}

void HelpWidget::UpdateSlot()
{
    ui->label_updatetip->setVisible(true);
    ui->label_updatetip->setText(tr("updating,please wait! " ));
    UBChain::getInstance()->updateProcess->startUpdate();
}

void HelpWidget::UpdateFinishSlot()
{
    ui->label_updatetip->setVisible(true);
    ui->label_updatetip->setText(tr("update finish,restart to take effect! " ));
}

void HelpWidget::InitWidget()
{
    InitStyle();
    ui->toolButton_update->setVisible(false);
    ui->label_updatetip->setVisible(false);

#ifdef WIN64
    ui->label_version->setText(QString("windows 64bit v") + WALLET_VERSION);
#elif defined(TARGET_OS_MAC)
    ui->label_version->setText(QString("mac v") + WALLET_VERSION);
#else
    ui->label_version->setText(QString("windows 32bit v") + WALLET_VERSION);
#endif

    UBChain::getInstance()->updateProcess->InitServerURL(UBChain::getInstance()->middlewarePath);

    connect(UBChain::getInstance()->updateProcess,&UpdateProcess::updateFinish,this,&HelpWidget::UpdateFinishSlot);
    connect(ui->toolButton_checkUpdate,&QToolButton::clicked,this,&HelpWidget::CheckUpdateSlot);
    connect(ui->toolButton_update,&QToolButton::clicked,this,&HelpWidget::UpdateSlot);
}

void HelpWidget::InitStyle()
{
    setStyleSheet(
                  "QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                  "QToolButton:hover{background-color:#00D2FF;}"
                  "QTextBrowser{background-color:transparent;color:#333333;border:none;font-size:12px;font-family:\"Microsoft YaHei UI Light\";}");

}

void HelpWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);//最后一位是设置透明属性（在0-255取值）
    painter.drawRoundedRect(rect(),10,10);

    QWidget::paintEvent(event);
}
