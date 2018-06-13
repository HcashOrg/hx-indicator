#ifndef NEWORIMPORTWALLETWIDGET_H
#define NEWORIMPORTWALLETWIDGET_H

#include <QWidget>

namespace Ui {
class NewOrImportWalletWidget;
}

class NewOrImportWalletWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewOrImportWalletWidget(QWidget *parent = 0);
    ~NewOrImportWalletWidget();

signals:
    void closeWallet();
    void enter();

private slots:
    void on_newWalletBtn_clicked();

    void on_importWalletBtn_clicked();

    void on_closeBtn_clicked();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::NewOrImportWalletWidget *ui;

    void paintEvent(QPaintEvent*e);
};

#endif // NEWORIMPORTWALLETWIDGET_H
