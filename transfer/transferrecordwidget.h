#ifndef TRANSFERRECORDWIDGET_H
#define TRANSFERRECORDWIDGET_H

#include <QWidget>

namespace Ui {
class TransferRecordWidget;
}

class TransferRecordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransferRecordWidget(QWidget *parent = 0);
    ~TransferRecordWidget();

    void showTransferRecord(QString _accountAddress);

private:
    Ui::TransferRecordWidget *ui;

    QString accountAddress;
};

#endif // TRANSFERRECORDWIDGET_H
