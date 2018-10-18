#ifndef MULTISIGPAGE_H
#define MULTISIGPAGE_H

#include <QWidget>

namespace Ui {
class MultiSigPage;
}

class MultiSigPage : public QWidget
{
    Q_OBJECT

public:
    explicit MultiSigPage(QWidget *parent = 0);
    ~MultiSigPage();

signals:
    void backBtnVisible(bool isShow);

private slots:
    void jsonDataUpdated(QString id);

    void on_createMultiSigBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::MultiSigPage *ui;

    void init();
    void paintEvent(QPaintEvent*);
};

#endif // MULTISIGPAGE_H
