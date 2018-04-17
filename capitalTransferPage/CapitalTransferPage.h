#ifndef CAPITALTRANSFERPAGE_H
#define CAPITALTRANSFERPAGE_H

#include <QWidget>

namespace Ui {
class CapitalTransferPage;
}

class CapitalTransferPage : public QWidget
{
    Q_OBJECT
public:
    struct CapitalTransferInput{
        CapitalTransferInput(const QString &_account,const QString &_symbol)
            :account(_account),symbol(_symbol)
        {

        }
        QString account;
        QString symbol;
    };

public:
    explicit CapitalTransferPage(const CapitalTransferInput &data,QWidget *parent = 0);
    ~CapitalTransferPage();
private slots:
    void ConfirmSlots();

    void radioChangedSlots();

    void jsonDataUpdated(QString id);
private:
    void updateData();
protected:
    void paintEvent(QPaintEvent *event);
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::CapitalTransferPage *ui;
private:
    class CapitalTransferPagePrivate;
    CapitalTransferPagePrivate *_p;
};

#endif // CAPITALTRANSFERPAGE_H
