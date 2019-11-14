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

#ifdef TEST_WALLET
    default_ip_port_vector.append(IP_Port("106.13.107.239", "19035"));
    default_ip_port_vector.append(IP_Port("127.0.0.1", "50420"));
#else
    default_ip_port_vector.append(IP_Port("106.13.107.239", "19035"));
    default_ip_port_vector.append(IP_Port("127.0.0.1", "50320"));
#endif

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

    foreach (const IP_Port& v, default_ip_port_vector)
    {
        ui->ipComboBox->addItem(QString("%1:%2").arg(v.ip).arg(v.port), QVariant::fromValue(v));
    }
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

void LightModeConfig::on_ipComboBox_currentIndexChanged(const QString &arg1)
{
    IP_Port v = ui->ipComboBox->currentData().value<IP_Port>();
    ui->ipComboBox->setCurrentText(v.ip);
    ui->portLineEdit->setText(v.port);
}
