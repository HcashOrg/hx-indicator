#ifndef WITHDRAWRECORDWIDGET_H
#define WITHDRAWRECORDWIDGET_H

#include <QWidget>

namespace Ui {
class WithdrawRecordWidget;
}

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

private:
    Ui::WithdrawRecordWidget *ui;

    QString accountAddress;

    void paintEvent(QPaintEvent*);
};

#endif // WITHDRAWRECORDWIDGET_H
