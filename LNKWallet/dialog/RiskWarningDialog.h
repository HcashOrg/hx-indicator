#ifndef RISKWARNINGDIALOG_H
#define RISKWARNINGDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class RiskWarningDialog;
}

class RiskWarningDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RiskWarningDialog(QWidget *parent = nullptr);
    ~RiskWarningDialog();

    void setInfoText(QString _text);
    void setReadingTime(int _seconds);
    void pop();
private slots:
    void on_okBtn_clicked();

    void onTimer();

private:
    Ui::RiskWarningDialog *ui;
    int seconds = 3;
    QTimer* timer = NULL;
};

#endif // RISKWARNINGDIALOG_H
