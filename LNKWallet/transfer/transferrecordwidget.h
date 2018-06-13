#ifndef TRANSFERRECORDWIDGET_H
#define TRANSFERRECORDWIDGET_H

#include <QWidget>

namespace Ui {
class TransferRecordWidget;
}

class PageScrollWidget;
class BlankDefaultWidget;
class TransferRecordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransferRecordWidget(QWidget *parent = 0);
    ~TransferRecordWidget();

    void init();

    void showTransferRecord(QString _accountAddress, QString _assetId = "ALL");

private slots:
    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void pageChangeSlot(unsigned int page);
    void on_transferRecordTableWidget_cellPressed(int row, int column);

private:
    Ui::TransferRecordWidget *ui;

    QString accountAddress;

    void paintEvent(QPaintEvent*);

    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
};

#endif // TRANSFERRECORDWIDGET_H
