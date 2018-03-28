#ifndef SINGLECONTRACTWIDGET_H
#define SINGLECONTRACTWIDGET_H

#include <QWidget>

#include "control/mycheckbtn.h"

namespace Ui {
class SingleContractWidget;
}

class SingleContractWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SingleContractWidget(QWidget *parent = 0);
    ~SingleContractWidget();

    QString contractAddress;
    void setContract(QString _contractAddress);

signals:
    void showDetail(QString contractAddress);
    void refresh();

private slots:
    void on_detailBtn_clicked();

    void onFollowBtnPressed();

private:
    Ui::SingleContractWidget *ui;
    MyCheckBtn* followBtn;

    bool eventFilter(QObject *watched, QEvent *e);
};

#endif // SINGLECONTRACTWIDGET_H
