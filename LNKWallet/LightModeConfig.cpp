#include "LightModeConfig.h"
#include "ui_LightModeConfig.h"

#include "wallet.h"
#include "commondialog.h"
#include "extra/RegularExpression.h"

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

QString LightModeConfig::getIP()
{
    return ui->ipComboBox->currentText();
}

QString LightModeConfig::getPort()
{
    return ui->portLineEdit->text();
}

void LightModeConfig::InitWidget()
{
    InitStyle();

    ui->label_version->setText(QString("v") + WALLET_VERSION);

    ui->ipComboBox->setValidator( new QRegExpValidator(RegularExpression::getRegExp_IPV4()));
    ui->portLineEdit->setValidator( new QRegExpValidator(RegularExpression::getRegExp_port()));
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

void LightModeConfig::on_enterBtn_clicked()
{
    if(!RegularExpression::testRegExp_IPV4(ui->ipComboBox->currentText()))
    {
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("Wrong IP!"));
        dia.pop();

        return;
    }

    if(!RegularExpression::testRegExp_port(ui->portLineEdit->text()))
    {
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("Wrong port!"));
        dia.pop();

        return;
    }

    emit enter();
}
