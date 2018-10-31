#ifndef CREATEMULTISIGWIDGET_H
#define CREATEMULTISIGWIDGET_H

#include <QWidget>

namespace Ui {
class CreateMultisigWidget;
}

struct AccountPubKey
{
    AccountPubKey(QString _pubKey = ""):pubKey(_pubKey){}
    QString account;
    QString pubKey;

    friend bool operator == (const AccountPubKey& a1, const AccountPubKey& a2)
    {
        return a1.pubKey == a2.pubKey;
    }
};

class CreateMultisigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateMultisigWidget( QWidget *parent = 0);
    ~CreateMultisigWidget();

    QVector<AccountPubKey> accountPubKeyVector;
    void showPubKeys();

private slots:
    void jsonDataUpdated(QString id);

    void on_addPubKeyBtn_clicked();

    void on_requiredLineEdit_textEdited(const QString &arg1);

    void on_createBtn_clicked();

    void on_pubKeyTableWidget_cellClicked(int row, int column);

    void on_addLocalAccountBtn_clicked();

private:
    Ui::CreateMultisigWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // CREATEMULTISIGWIDGET_H
