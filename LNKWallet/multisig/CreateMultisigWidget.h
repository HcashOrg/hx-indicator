#ifndef CREATEMULTISIGWIDGET_H
#define CREATEMULTISIGWIDGET_H

#include <QWidget>

namespace Ui {
class CreateMultisigWidget;
}

struct AccountPubKey
{
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
    explicit CreateMultisigWidget( QString _account, QString _pubKey, QWidget *parent = 0);
    ~CreateMultisigWidget();

    QString account;
    QVector<AccountPubKey> accountPubKeyVector;
    void showPubKeys();

private slots:
    void on_addPubKeyBtn_clicked();

    void on_requiredLineEdit_textEdited(const QString &arg1);

    void on_createBtn_clicked();

    void on_pubKeyTableWidget_cellClicked(int row, int column);

private:
    Ui::CreateMultisigWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // CREATEMULTISIGWIDGET_H
