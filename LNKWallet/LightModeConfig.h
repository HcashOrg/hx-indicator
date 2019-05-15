#ifndef LIGHTMODECONFIG_H
#define LIGHTMODECONFIG_H

#include <QWidget>

namespace Ui {
class LightModeConfig;
}

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

signals:
    void minimum();
    void closeWallet();
    void enter();
private:
    void InitWidget();
    void InitStyle();

private:
    Ui::LightModeConfig *ui;
};

#endif // LIGHTMODECONFIG_H
