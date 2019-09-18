#include <QDebug>
#include <QMovie>

#include "titlebar.h"
#include "ui_titlebar.h"

#include "wallet.h"
#include "commondialog.h"

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBar)
{

    ui->setupUi(this);

    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/minimizeBtn.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                              "QToolButton:hover{background-image:url(:/ui/wallet_ui/minimizeBtn_hover.png);}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/closeBtn.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/ui/wallet_ui/closeBtn_hover.png);}");
    ui->refreshBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/refresh.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                  "QToolButton:hover{background-image:url(:/ui/wallet_ui/refresh_hover.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    gif = new QMovie(":/ui/wallet_ui/refresh.gif");
    connect(gif,SIGNAL(frameChanged(int)),this,SLOT( gifPlayOnce(int)));
    ui->gifLabel->setMovie(gif);
    ui->gifLabel->hide();

//    ui->divLineLabel->setPixmap(QPixmap("pic2/divLine.png"));
//    ui->divLineLabel->setScaledContents(true);
    ui->backBtn->setVisible(false);
    ui->backBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/back.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));
    connect(ui->backBtn,&QToolButton::clicked,this,&TitleBar::back);

#ifndef LIGHT_MODE
    ui->refreshBtn->hide();
#endif
}

TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::on_minBtn_clicked()
{
    if( HXChain::getInstance()->minimizeToTray)
    {

        emit tray();
    }
    else
    {
        emit minimum();
    }
}

void TitleBar::on_closeBtn_clicked()
{
    if( HXChain::getInstance()->closeToMinimize)
    {

        emit tray();
    }
    else
    {
        HXChain::getInstance()->mainFrame->hideKLineWidget();

        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText( tr( "Sure to close the Wallet?"));
        bool choice = commonDialog.pop();

        if( choice)
        {
            emit closeWallet();
        }
        else
        {
            return;
        }

    }
}

void TitleBar::retranslator()
{
    ui->retranslateUi(this);
}

void TitleBar::backBtnVis(bool isVisible)
{
    ui->backBtn->setVisible(isVisible);

}

void TitleBar::extendToWidth(int _width)
{
    setGeometry(this->x(), this->y(), _width, this->height());
    ui->minBtn->move(_width - 64, 0);
    ui->closeBtn->move(_width - 32, 0);
}

void TitleBar::jsonDataUpdated(QString id)
{

}


void TitleBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(243,241,250));
    painter.setPen(QColor(215,211,229));
    painter.drawRect( -1, -1, this->width() + 2, this->height());

}

void TitleBar::on_refreshBtn_clicked()
{
    ui->gifLabel->show();
    ui->refreshBtn->hide();
    gif->start();

    if(!refreshing)
    {
        if(!refreshTimer)   refreshTimer = new QTimer;
        refreshing = true;
        refreshTimer->singleShot(10000, [this](){
            refreshing = false;
        });

        Q_EMIT refresh();
    }
}

void TitleBar::gifPlayOnce(int num)
{
    if( gif->frameCount() == num + 1)
    {
        gif->stop();
        ui->gifLabel->hide();
        ui->refreshBtn->show();
    }
}
