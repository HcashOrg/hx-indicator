#ifndef SELECTWALLETPATHWIDGET_H
#define SELECTWALLETPATHWIDGET_H

#include <QWidget>
#include <QDomDocument>

namespace Ui {
class SelectWalletPathWidget;
}

class SelectWalletPathWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectWalletPathWidget(QWidget *parent = 0);
    ~SelectWalletPathWidget();

private slots:
    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void on_pathBtn_clicked();

    void onLanguageChanged(const QString &arg1);

signals:
    void enter();
    void newOrImportWallet();
    void closeWallet();

private:
    Ui::SelectWalletPathWidget *ui;

    void paintEvent(QPaintEvent*);
private:
    void InitWidget();
    void InitStyle();

};

int compareVersion( QString a, QString b);

#endif // SELECTWALLETPATHWIDGET_H
