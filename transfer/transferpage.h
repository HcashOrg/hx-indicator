#ifndef TRANSFERPAGE_H
#define TRANSFERPAGE_H

#include <QWidget>

namespace Ui {
class TransferPage;
}

#define MODULE_TRAHSFER_PAGE "TRAHSFER_PAGE"

class FeeChooseWidget;
class TransferPage : public QWidget
{
    Q_OBJECT

public:
    explicit TransferPage(QString name="",QWidget *parent = 0,QString assetType = "");
    ~TransferPage();

    QString getCurrentAccount();
    void setAddress(QString);

    void getAssets();

private:
    bool contactUpdating;

signals:
    void accountChanged(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void back();

    void feeChangeSignal(double);
    void usePoundage();
public slots:
    void refresh();

private slots:

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_sendBtn_clicked();

    void on_amountLineEdit_textChanged(const QString &arg1);

    void jsonDataUpdated(QString id);

    void on_assetComboBox_currentIndexChanged(int index);

    void on_sendtoLineEdit_textChanged(const QString &arg1);

    void on_memoTextEdit_textChanged();

    void on_transferRecordBtn_clicked();

    void chooseContactSlots();
    void selectContactSlots(const QString &name,const QString &address);

    void updateAmountSlots();

    //void checkStateChangedSlots(int state);
private:
    Ui::TransferPage *ui;
    QString accountName;
    QString assetType;
    QWidget* currentTopWidget;  // 保存当前顶层窗口

    FeeChooseWidget *feeWidget;
    //QString feeID;//手续费承兑单id
    bool inited;
    bool assetUpdating;

    void setAmountPrecision();
private:
    void InitStyle();

   // void updatePoundage();//刷新手续费相关
};

#endif // TRANSFERPAGE_H
