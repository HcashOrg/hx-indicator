#ifndef MULTISIGPAGE_H
#define MULTISIGPAGE_H

#include <QWidget>

namespace Ui {
class MultiSigPage;
}

class MultiSigPage : public QWidget
{
    Q_OBJECT

public:
    explicit MultiSigPage(QWidget *parent = 0);
    ~MultiSigPage();

    void refresh();

    void updateMultiSigList();

signals:
    void goToTransferPage(QString,QString);
    void showMultiSigTransactionPage(QString);

private slots:
    void jsonDataUpdated(QString id);

    void on_importMultiSigBtn_clicked();

    void on_multiSigTableWidget_cellPressed(int row, int column);

    void on_createMultiSigBtn_clicked();

    void on_signMultiSigBtn_clicked();

private:
    Ui::MultiSigPage *ui;

    void paintEvent(QPaintEvent* e);
};

#endif // MULTISIGPAGE_H
