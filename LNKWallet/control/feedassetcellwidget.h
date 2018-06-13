#ifndef FEEDASSETCELLWIDGET_H
#define FEEDASSETCELLWIDGET_H

#include <QWidget>

namespace Ui {
class FeedAssetCellWidget;
}

class FeedAssetCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FeedAssetCellWidget(QWidget *parent = 0);
    ~FeedAssetCellWidget();

    QString assetName;
    void setAssetName(QString _assetName);
    void setChecked(bool _isChecked);

private:
    Ui::FeedAssetCellWidget *ui;
};

#endif // FEEDASSETCELLWIDGET_H
