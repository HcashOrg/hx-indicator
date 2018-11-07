#ifndef MULTISIGTRANSFERWIDGET_H
#define MULTISIGTRANSFERWIDGET_H

#include <QWidget>

namespace Ui {
class MultiSigTransferWidget;
}

class FeeChooseWidget;
class MultiSigTransferWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MultiSigTransferWidget(QWidget *parent = 0);
    ~MultiSigTransferWidget();

    void setFromAddress(QString address);
    void setAsset(QString asset);

signals:
    void usePoundage();

private slots:
    void jsonDataUpdated(QString id);

    void on_sendBtn_clicked();

    void onAssetComboBoxCurrentIndexChanged(const QString &arg1);

    void on_toolButton_chooseContact_clicked();

    void selectContactSlots(const QString &name,const QString &address);

    void on_memoTextEdit_textChanged();

private:
    Ui::MultiSigTransferWidget *ui;
    FeeChooseWidget* feeWidget = NULL;

    void InitStyle();
};

#endif // MULTISIGTRANSFERWIDGET_H
