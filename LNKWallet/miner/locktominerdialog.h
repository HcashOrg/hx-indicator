#ifndef LOCKTOMINERDIALOG_H
#define LOCKTOMINERDIALOG_H

#include <QDialog>

namespace Ui {
class LockToMinerDialog;
}

class LockToMinerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LockToMinerDialog( QString _accountName, QWidget *parent = 0);
    ~LockToMinerDialog();
    QString m_accountName;

    void  pop();

    void init();

    void setMiner(QString _minerName);
    void setAsset(QString _assetName);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

    void on_closeBtn_clicked();

    void on_amountLineEdit_textEdited(const QString &arg1);

private:
    Ui::LockToMinerDialog *ui;
};

#endif // LOCKTOMINERDIALOG_H
