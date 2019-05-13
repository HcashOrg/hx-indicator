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

private slots:
    void on_closeBtn_clicked();

    void on_miniBtn_clicked();

signals:
    void minimum();
    void closeWallet();
private:
    void InitWidget();
    void InitStyle();

private:
    Ui::LightModeConfig *ui;
};

#endif // LIGHTMODECONFIG_H
