#ifndef NAMETRANSFERPAGE_H
#define NAMETRANSFERPAGE_H

#include <QWidget>

namespace Ui {
class NameTransferPage;
}

class NameTransferPage : public QWidget
{
    Q_OBJECT

public:
    explicit NameTransferPage(QWidget *parent = nullptr);
    ~NameTransferPage();

signals:
    void backBtnVisible(bool isShow);

private slots:
    void jsonDataUpdated(QString id);

    void on_typeTrxBtn_clicked();

    void on_typeSignBtn_clicked();

    void on_createTrxBtn_clicked();

    void on_trxTableWidget_cellClicked(int row, int column);

    void on_trxCodeLineEdit_textChanged(const QString &arg1);


    void on_signBtn_clicked();

private:
    Ui::NameTransferPage *ui;

    void showNameTransferTrxs();

    void paintEvent(QPaintEvent*);
};

#endif // NAMETRANSFERPAGE_H
