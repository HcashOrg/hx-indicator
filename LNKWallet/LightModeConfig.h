#ifndef LIGHTMODECONFIG_H
#define LIGHTMODECONFIG_H

#include <QWidget>
#include <QVariant>

namespace Ui {
class LightModeConfig;
}

struct IP_Port
{
public:
    IP_Port(QString _ip = "", QString _port = "")
    {
        ip = _ip;
        port = _port;
    }

    IP_Port(const IP_Port& c)
    {
        ip = c.ip;
        port = c.port;
    }

    QString ip;
    QString port;
};
Q_DECLARE_METATYPE(IP_Port);

class LightModeConfig : public QWidget
{
    Q_OBJECT

public:
    explicit LightModeConfig(QWidget *parent = nullptr);
    ~LightModeConfig();

    QString getIP();
    QString getPort();

private slots:
    void on_closeBtn_clicked();

    void on_miniBtn_clicked();

    void on_enterBtn_clicked();

    void on_ipComboBox_currentIndexChanged(const QString &arg1);

signals:
    void minimum();
    void closeWallet();
    void enter();
private:
    void InitWidget();
    void InitStyle();

private:
    Ui::LightModeConfig *ui;

    QVector<IP_Port> default_ip_port_vector;
};

#endif // LIGHTMODECONFIG_H
