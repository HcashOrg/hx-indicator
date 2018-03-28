#ifndef SELECTWALLETPATHWIDGET_H
#define SELECTWALLETPATHWIDGET_H

#include <QWidget>
#include <QtNetwork>
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
    void on_selectPathBtn_clicked();

    void on_okBtn_clicked();

    void on_closeBtn_clicked();

    void httpFinished();

    void getUpdateExeFinished();

    void getUpdateExeReadyRead();

signals:
    void enter();
    void newOrImportWallet();
    void closeWallet();

private:
    Ui::SelectWalletPathWidget *ui;
    QNetworkReply *reply;
    QDomDocument localXml;
    QDomDocument serverXml;
    QNetworkAccessManager qnam;
    bool updateOrNot;
    QFile* updateExe;

    void getUpdateXml();
    QString getXmlElementText(QDomDocument& doc, QString name);

    void paintEvent(QPaintEvent*);
};

int compareVersion( QString a, QString b);

#endif // SELECTWALLETPATHWIDGET_H
