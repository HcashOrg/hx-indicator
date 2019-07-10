#ifndef FUNCTIONCITIZENWIDGET_H
#define FUNCTIONCITIZENWIDGET_H

#include <QWidget>

namespace Ui {
class FunctionCitizenWidget;
}

class FunctionCitizenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionCitizenWidget(QWidget *parent = 0);
    ~FunctionCitizenWidget();
    void retranslator();
public slots:
    void DefaultShow();
private:
    void InitWidget();
    void InitStyle();

signals:
    void showCitizenAccountSignal();
    void showCitizenProposalSignal();
    void showCitizenPolicySignal();
private slots:

    void on_accountInfoBtn_clicked();

    void on_proposalBtn_clicked();

    void on_policyBtn_clicked();

private:
    Ui::FunctionCitizenWidget *ui;

    void paintEvent(QPaintEvent*);
};

#endif // FUNCTIONCITIZENWIDGET_H
