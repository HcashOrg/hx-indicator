#ifndef TRANSFERPAGE_H
#define TRANSFERPAGE_H

#include <QWidget>

namespace Ui {
class TransferPage;
}

#define MODULE_TRAHSFER_PAGE "TRAHSFER_PAGE"

class TransferPage : public QWidget
{
    Q_OBJECT

public:
    explicit TransferPage(QString name,QWidget *parent = 0);
    ~TransferPage();

    QString getCurrentAccount();
    void setAddress(QString);
    void setContact(QString contactRemark);

    void getAssets();

    void updateTransactionFee();

private:
    QStringList contactsList;
    bool contactUpdating;
    void getContactsList();

signals:
    void accountChanged(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void showAccountPage(QString);
    void back();

public slots:
    void refresh();

private slots:

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_sendBtn_clicked();

    void on_amountLineEdit_textChanged(const QString &arg1);

    void contactSelected(QString remark, QString contact);

    void jsonDataUpdated(QString id);

    void on_assetComboBox_currentIndexChanged(int index);

    void on_sendtoLineEdit_textEdited(const QString &arg1);

    void on_memoTextEdit_textChanged();

    void on_contactComboBox_currentIndexChanged(int index);

    void on_transferRecordBtn_clicked();

private:
    Ui::TransferPage *ui;
    QString accountName;
    QWidget* currentTopWidget;  // 保存当前顶层窗口

    bool inited;
    bool assetUpdating;

    void paintEvent(QPaintEvent*);
//    void addContact(QString,QString);

    void setAmountPrecision();
};

#endif // TRANSFERPAGE_H
