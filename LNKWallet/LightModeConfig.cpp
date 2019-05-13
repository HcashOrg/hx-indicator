#include "LightModeConfig.h"
#include "ui_LightModeConfig.h"

#include "wallet.h"

LightModeConfig::LightModeConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LightModeConfig)
{
    ui->setupUi(this);

    InitWidget();
}

LightModeConfig::~LightModeConfig()
{
    delete ui;
}

void LightModeConfig::InitWidget()
{
    InitStyle();

    ui->label_version->setText(QString("v") + WALLET_VERSION);
}

void LightModeConfig::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/ui/wallet_ui/cover.png").scaled(this->size())));
    setPalette(palette);

    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(243,241,250));
    ui->label_version->setPalette(pa);

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/cover_close.png);background-repeat: no-repeat;background-position: center;border: none;}");
    ui->miniBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/cover_min.png);background-repeat: no-repeat;background-position: center;border: none;}");

    ui->enterBtn->setStyleSheet("QToolButton{font: 12px \"Microsoft YaHei UI Light\";background-color:rgb(255,255,255); border:none;border-radius:3px;color: rgb(59, 22, 136);}" \
                                );


}

void LightModeConfig::on_closeBtn_clicked()
{
    emit closeWallet();
}

void LightModeConfig::on_miniBtn_clicked()
{
    emit minimum();
}
