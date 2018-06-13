#include "HelpWidget.h"
#include "ui_HelpWidget.h"

#include <QPainter>
#include "wallet.h"
#include "update/UpdateProcess.h"

class HelpWidget::DataPrivate
{
public:
    DataPrivate()
        :updateProcess(new UpdateProcess())
    {

    }

public:
    UpdateProcess *updateProcess;
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
    delete _p;
    delete ui;
}

void HelpWidget::CheckUpdateSlot()
{
    UBChain::getInstance()->SetUpdateNeeded(false);

    ui->label_updatetip->setText(tr("Checking updates..."));
    ui->label_updatetip->setVisible(true);
    showButtonState(-1);
    _p->updateProcess->checkUpdate();
}

void HelpWidget::CheckResultSlot(const QString &version)
{
    if(version.isEmpty())
    {
        //没有更新
        ui->label_updatetip->setText(tr("No new version!"));

        showButtonState(0);
    }
    else
    {
        ui->label_updatetip->setText(tr("New version found! " )+ version);

        showButtonState(1);
    }
}

void HelpWidget::UpdateSlot()
{
    ui->label_updatetip->setText(tr("Downloading... Please wait a moment! " ));

    showButtonState(-1);
    _p->updateProcess->startUpdate();
}

void HelpWidget::UpdateFinishSlot()
{
    ui->label_updatetip->setText(tr("Update finished. Restart and it will take effect! " ));
    showButtonState(2);
    UBChain::getInstance()->SetUpdateNeeded(true);
}

void HelpWidget::UpdateWrongSlot()
{
    ui->label_updatetip->setText(tr("Update error! " ));
    showButtonState(0);

    UBChain::getInstance()->SetUpdateNeeded(false);
}

void HelpWidget::RestartSlot()
{
    UBChain::getInstance()->mainFrame->onCloseWallet();
}

void HelpWidget::InitWidget()
{
    InitStyle();
    showButtonState(0);
    ui->label_updatetip->setVisible(false);

#ifdef WIN64
    ui->label_version->setText(QString("windows 64bit v") + WALLET_VERSION);
#elif defined(TARGET_OS_MAC)
    ui->label_version->setText(QString("mac v") + WALLET_VERSION);
#else
    ui->label_version->setText(QString("windows 32bit v") + WALLET_VERSION);
#endif

    _p->updateProcess->InitServerURL(UBChain::getInstance()->middlewarePath);

    connect(_p->updateProcess,&UpdateProcess::updateFinish,this,&HelpWidget::UpdateFinishSlot);
    connect(_p->updateProcess,&UpdateProcess::updateWrong,this,&HelpWidget::UpdateWrongSlot);
    connect(_p->updateProcess,&UpdateProcess::NewstVersionSignal,this,&HelpWidget::CheckResultSlot);

    connect(ui->toolButton_checkUpdate,&QToolButton::clicked,this,&HelpWidget::CheckUpdateSlot);
    connect(ui->toolButton_update,&QToolButton::clicked,this,&HelpWidget::UpdateSlot);
    connect(ui->toolButton_restart,&QToolButton::clicked,this,&HelpWidget::RestartSlot);
}

void HelpWidget::InitStyle()
{
    setStyleSheet(
                  OKBTN_STYLE
                  "QTextBrowser{background-color:transparent;color:#333333;border:none;font-size:12px;font-family:\"Microsoft YaHei UI Light\";}");

}

void HelpWidget::showButtonState(int type)
{
    ui->toolButton_checkUpdate->setVisible(0 == type);
    ui->toolButton_update->setVisible(1 == type);
    ui->toolButton_restart->setVisible(2 == type);
}

void HelpWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);//最后一位是设置透明属性（在0-255取值）
    painter.drawRoundedRect(rect(),10,10);
    painter.drawRect(0,0,10,10);

    QWidget::paintEvent(event);
}
