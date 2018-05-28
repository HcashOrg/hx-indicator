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
    ui->label_updatetip->setText(tr("start check!"));
    showState(true,false,false,true);
    ui->toolButton_checkUpdate->setEnabled(false);
    _p->updateProcess->checkUpdate();
}

void HelpWidget::CheckResultSlot(const QString &version)
{
    if(version.isEmpty())
    {
        //没有更新
        ui->label_updatetip->setText(tr("no new version!"));
        showState(true,false,false,true);
    }
    else
    {
        ui->label_updatetip->setText(tr("new version found! " )+ version);
        showState(true,true,false,true);
    }
    ui->toolButton_checkUpdate->setEnabled(true);
}

void HelpWidget::UpdateSlot()
{
    ui->toolButton_checkUpdate->setEnabled(false);
    ui->label_updatetip->setText(tr("updating,please wait! " ));
    showState(true,false,false,true);
    _p->updateProcess->startUpdate();
}

void HelpWidget::UpdateFinishSlot()
{
    ui->toolButton_checkUpdate->setEnabled(true);
    ui->label_updatetip->setText(tr("update finish,restart to take effect! " ));
    showState(true,false,true,true);
    UBChain::getInstance()->SetUpdateNeeded(true);
}

void HelpWidget::UpdateWrongSlot()
{
    ui->toolButton_checkUpdate->setEnabled(true);
    ui->label_updatetip->setText(tr("update wrong! " ));

    showState(true,false,false,true);

    UBChain::getInstance()->SetUpdateNeeded(false);
}

void HelpWidget::RestartSlot()
{
    UBChain::getInstance()->mainFrame->onCloseWallet();
}

void HelpWidget::InitWidget()
{
    InitStyle();
    showState(true,false,false,false);

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
                  "QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                  "QToolButton:hover{background-color:#00D2FF;}"
                  "QTextBrowser{background-color:transparent;color:#333333;border:none;font-size:12px;font-family:\"Microsoft YaHei UI Light\";}");

}

void HelpWidget::showState(bool check, bool update, bool restart, bool label_tip)
{
    ui->toolButton_checkUpdate->setVisible(check);
    ui->toolButton_update->setVisible(update);
    ui->toolButton_restart->setVisible(restart);
    ui->label_updatetip->setVisible(label_tip);
}

void HelpWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);//最后一位是设置透明属性（在0-255取值）
    painter.drawRoundedRect(rect(),10,10);

    QWidget::paintEvent(event);
}
