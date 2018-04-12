#ifndef FUNCTIONACCOUNTWIDGET_H
#define FUNCTIONACCOUNTWIDGET_H

#include <QWidget>

namespace Ui {
class FunctionAccountWidget;
}

class FunctionAccountWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionAccountWidget(QWidget *parent = 0);
    ~FunctionAccountWidget();
private slots:
<<<<<<< HEAD
    void on_minerInfoBtn_clicked();

signals:
    void showMinerSignal();

=======
    void AssetShowSlots();
    void MinerShowSlots();
>>>>>>> bb3273ae82a92d28cc7d77c7cfdcc1f04d525643
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::FunctionAccountWidget *ui;
};

#endif // FUNCTIONACCOUNTWIDGET_H
