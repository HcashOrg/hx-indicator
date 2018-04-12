#ifndef FORECLOSEDIALOG_H
#define FORECLOSEDIALOG_H

#include <QDialog>

namespace Ui {
class ForecloseDialog;
}

class ForecloseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ForecloseDialog(QWidget *parent = 0);
    ~ForecloseDialog();

    QString  pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::ForecloseDialog *ui;
    bool yesOrNo;
};

#endif // FORECLOSEDIALOG_H
