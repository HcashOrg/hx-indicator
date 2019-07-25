#include "CoverWidget.h"
#include "ui_CoverWidget.h"

#include "wallet.h"
#include <QPushButton>

CoverWidget::CoverWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoverWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(229,226,240));
    setPalette(palette);

    m_progressBar = new MyProgressBar(this);
    m_progressBar->setGeometry(185,290,400,24);
    m_progressBar->setValue(0);
    m_progressBar->setFormat(QString("%p%"));

    ui->progressBar->hide();
}

CoverWidget::~CoverWidget()
{
    delete ui;
}

void CoverWidget::retranslator()
{
    ui->retranslateUi(this);

}

void CoverWidget::updateData()
{
    ui->heightLabel->setText(QString::number( HXChain::getInstance()->walletInfo.blockHeight));
    ui->timeLabel->setText(HXChain::getInstance()->walletInfo.blockAge);

    if(HXChain::getInstance()->walletInfo.targetBlockHeight == 0 ||
            ((HXChain::getInstance()->walletInfo.targetBlockHeight == HXChain::getInstance()->walletInfo.blockHeight) && !HXChain::getInstance()->walletInfo.blockAge.contains("second")))
    {
        m_progressBar->setFormat(tr("Calculating..."));
        m_progressBar->setValue(0);
    }
    else
    {
        m_progressBar->setFormat(QString("%p%"));
        m_progressBar->setMinimum(0);
        m_progressBar->setMaximum(HXChain::getInstance()->walletInfo.targetBlockHeight);
        m_progressBar->setValue(HXChain::getInstance()->walletInfo.blockHeight);
    }

    if(HXChain::getInstance()->GetBlockSyncFinish() && firstDisplay)
    {
//        hide();
        QTimer::singleShot(5000,this,[this](){this->hide();});
        firstDisplay = false;
        ui->tipLabel->hide();
    }
}

void CoverWidget::on_hideBtn_clicked()
{
    hide();
    firstDisplay = false;
}
