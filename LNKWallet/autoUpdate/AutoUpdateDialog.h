#ifndef AUTOUPDATEDIALOG_H
#define AUTOUPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class AutoUpdateDialog;
}

class AutoUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoUpdateDialog(QWidget *parent = nullptr);
    ~AutoUpdateDialog();
public:
    bool pop();
public:
    void setVersion(const QString &version,bool isForceUpdate);
    void setUpdateLog(const QString &updateLog);
private slots:
    void confirmUpdateSlots();
private:
    void InitWidget();
private:
    Ui::AutoUpdateDialog *ui;
    bool YesOrNo = false;
};

#endif // AUTOUPDATEDIALOG_H
