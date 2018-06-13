#ifndef WITHDRAWRECORDWIDGET_H
#define WITHDRAWRECORDWIDGET_H

#include <QWidget>

namespace Ui {
class WithdrawRecordWidget;
}

class PageScrollWidget;
class BlankDefaultWidget;
class WithdrawRecordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WithdrawRecordWidget(QWidget *parent = 0);
    ~WithdrawRecordWidget();

    void init();

    void showWithdrawRecord(QString _accountAddress, QString _assetId = "ALL");

private slots:
    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void pageChangeSlot(unsigned int page);
    void on_withdrawRecordTableWidget_cellPressed(int row, int column);

private:
    Ui::WithdrawRecordWidget *ui;

    QString accountAddress;

    void paintEvent(QPaintEvent*);
    PageScrollWidget *pageWidget;
    BlankDefaultWidget *blankWidget;
};

#endif // WITHDRAWRECORDWIDGET_H
