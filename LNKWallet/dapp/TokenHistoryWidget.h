#ifndef TOKENHISTORYWIDGET_H
#define TOKENHISTORYWIDGET_H

#include <QWidget>

#include "ContractTokenPage.h"

namespace Ui {
class TokenHistoryWidget;
}

struct ContractEvent
{
    QString eventId;
    QString contractAddress;
    QString eventName;
    QString eventArg;
    QString trxId;
    int block = -1;
    int opNum = 0;
};

class TokenHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TokenHistoryWidget( ContractTokenPage *parent = 0);
    ~TokenHistoryWidget();

    void init();

    void setAccount(QString _accountName);
    void setToken(QString _tokenSymbol);

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_tokenComboBox_currentIndexChanged(const QString &arg1);

    void on_tokenRecordTableWidget_cellClicked(int row, int column);

private:
    Ui::TokenHistoryWidget *ui;
    ContractTokenPage* page = NULL;

    QMap<QString,QVector<ContractEvent>> contractEvents;
    void fetchContractEvents();
    void showContractEvents();
    QVector<ContractEvent> filterAccountContractEvents(QString accountAddress, QString contractId);
};

#endif // TOKENHISTORYWIDGET_H
