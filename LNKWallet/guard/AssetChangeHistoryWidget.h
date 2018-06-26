#ifndef ASSETCHANGEHISTORYWIDGET_H
#define ASSETCHANGEHISTORYWIDGET_H

#include <QWidget>
#include <QMap>

namespace Ui {
class AssetChangeHistoryWidget;
}

class AssetChangeHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssetChangeHistoryWidget(QWidget *parent = 0);
    ~AssetChangeHistoryWidget();

    void init();

    void setAsset(QString _assetSymbol);

private slots:
    void jsonDataUpdated(QString id);

    void on_changeHistoryTableWidget_cellPressed(int row, int column);

    void on_changeHistoryTableWidget_cellClicked(int row, int column);

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::AssetChangeHistoryWidget *ui;
    bool inited = false;
    QString assetSymbol;

    void fetchMultisigAccountPair();

    QMap<QString,QJsonObject>   crosschainKeyObjectMap;
    void fetchCrosschainPrivateKey(QString address);
    void refreshKeyState();
};

#endif // ASSETCHANGEHISTORYWIDGET_H
