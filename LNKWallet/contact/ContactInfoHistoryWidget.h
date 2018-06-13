#ifndef CONTACTINFOHISTORYWIDGET_H
#define CONTACTINFOHISTORYWIDGET_H

#include <QWidget>

namespace Ui {
class ContactInfoHistoryWidget;
}

class ContactInfoHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactInfoHistoryWidget(QWidget *parent = 0);
    ~ContactInfoHistoryWidget();

    void showTransferRecord(QString _accountAddress, QString _assetId = "ALL");

private slots:
    void assetComboBox_currentIndexChanged(const QString &arg1);

    void pageChangeSlot(unsigned int page);
    void on_transferRecordTableWidget_cellPressed(int row, int column);

private:
    void InitWidget();
    void InitStyle();
private:
    Ui::ContactInfoHistoryWidget *ui;
private:
    class ContactInfoHistoryWidgetPrivate;
    ContactInfoHistoryWidgetPrivate *_p;

    QString accountAddress;
};

#endif // CONTACTINFOHISTORYWIDGET_H
