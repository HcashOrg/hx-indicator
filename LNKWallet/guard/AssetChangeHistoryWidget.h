#ifndef ASSETCHANGEHISTORYWIDGET_H
#define ASSETCHANGEHISTORYWIDGET_H

#include <QWidget>

namespace Ui {
class AssetChangeHistoryWidget;
}

class AssetChangeHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssetChangeHistoryWidget(QWidget *parent = 0);
    ~AssetChangeHistoryWidget();

    void setAsset(QString _assetSymbol);

private slots:
    void jsonDataUpdated(QString id);

private:
    Ui::AssetChangeHistoryWidget *ui;
    QString assetSymbol;
};

#endif // ASSETCHANGEHISTORYWIDGET_H
