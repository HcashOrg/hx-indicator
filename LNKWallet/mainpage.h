#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include <QMap>
#include <QTimer>

namespace Ui {
class MainPage;
}

#define MODULE_MAIN_PAGE "MAIN_PAGE"
class BlankDefaultWidget;
class CaptialNotify;
class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = 0);
    ~MainPage();

    void retranslator(QString language);


public slots:
    void refresh();

    void init();

    void updateAccountList();


signals:
    void showShadowWidget();
    void hideShadowWidget();
    void showTransferPage(QString,QString);//账户名，资产类型
    void newAccount(QString);

    void backBtnVisible(bool isShow);
private slots:
    void on_addAccountBtn_clicked();

    void on_importAccountBtn_clicked();

    void on_accountTableWidget_cellClicked(int row, int column);

    void on_accountTableWidget_cellEntered(int row, int column);

    void jsonDataUpdated(QString id);


//    void showDetailWidget(QString name);

//    void hideDetailWidget();


    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_copyBtn_clicked();
    void on_copyBtn2_clicked();

    void on_qrcodeBtn_clicked();

    void on_allTransactionBtn_clicked();

    void on_backupBtn_clicked();


    void on_registerBtn_clicked();

    //自动划转提醒
    void activeTunnelMoneyNotify();
private:
    void InitStyle();
private:
    Ui::MainPage *ui;
    bool inited = false;


    void paintEvent(QPaintEvent*);
    bool eventFilter(QObject *watched, QEvent *e);

    BlankDefaultWidget *blankWidget;
    CaptialNotify *captialNotify;
};

#endif // MAINPAGE_H
