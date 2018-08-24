#ifndef CREATETOKENDIALOG_H
#define CREATETOKENDIALOG_H

#include <QDialog>

namespace Ui {
class CreateTokenDialog;
}

class FeeChooseWidget;

class CreateTokenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateTokenDialog(QWidget *parent = 0);
    ~CreateTokenDialog();

    void pop();

    void init();

    bool newTokenAdded = false;

private slots:
    void jsonDataUpdated(QString id);

    void on_registerBtn_clicked();

    void on_closeBtn_clicked();

    void on_tokenNameLineEdit_textEdited(const QString &arg1);

    void on_tokenSymbolLineEdit_textEdited(const QString &arg1);

    void on_precisionSpinBox_valueChanged(int arg1);

    void on_totalSupplyLineEdit_textEdited(const QString &arg1);

    void on_initBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::CreateTokenDialog *ui;
    int stepCount = 0;
    FeeChooseWidget* registerFeeWidget = NULL;
    QTimer* timerForRegister = NULL;
    void calculateRegisterFee();

    FeeChooseWidget* initFeeWidget = NULL;
    void calculateInitFee();

    QString intToPrecisionString(int precision);    //将 0-8 转换成1,10,... 100000000等格式
    QString addPrecisionString(QString supply, int precision);     //  比如 20000,2  转换成 2000000

    void setTotalSupplyValidator();
};

#endif // CREATETOKENDIALOG_H
