#ifndef SELECTCOLDKEYFILEDIALOG_H
#define SELECTCOLDKEYFILEDIALOG_H

#include <QDialog>

namespace Ui {
class SelectColdKeyFileDialog;
}

class SelectColdKeyFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectColdKeyFileDialog(QWidget *parent = 0);
    ~SelectColdKeyFileDialog();

    void pop();
    QString filePath;
    QString pwd;
private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void on_pathBtn_clicked();

private:
    Ui::SelectColdKeyFileDialog *ui;
};

#endif // SELECTCOLDKEYFILEDIALOG_H
