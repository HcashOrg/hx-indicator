#ifndef CONTRACTDETAILWIDGET_H
#define CONTRACTDETAILWIDGET_H

#include <QWidget>

#include "control/mycheckbtn.h"

namespace Ui {
class ContractDetailWidget;
}

class ContractDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContractDetailWidget(QWidget *parent = 0);
    ~ContractDetailWidget();

    QString contractAddress;
    void setContract(QString _contractAddress);

signals:
    void back();
    void refresh();

private slots:
    void on_backBtn_clicked();

    void on_websiteLabel_linkActivated(const QString &link);

    void onFollowBtnPressed();

    void on_manageBtn_clicked();

private:
    Ui::ContractDetailWidget *ui;
    MyCheckBtn* followBtn;

    bool eventFilter(QObject *watched, QEvent *e);
    void paintEvent(QPaintEvent* e);
};

#endif // CONTRACTDETAILWIDGET_H
