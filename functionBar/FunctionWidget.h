#ifndef FUNCTIONWIDGET_H
#define FUNCTIONWIDGET_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>左侧工具栏 </summary>
///
///<remarks> 2018.04.02 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class FunctionWidget;
}

class FunctionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionWidget(QWidget *parent = 0);
    ~FunctionWidget();
public:
    void retranslator();
signals:
    void showMainPage();
    void showAccountPage();
    void showSmartContractPage();
    void showTransferPage();
    void showContactPage();
    void showFeedPage();
    void showMultiSigPage();

    void assetChanged(int);

    void lock();

signals:
    void ShrinkSignal();
    void RestoreSignal();
private slots:
    void ShowContactWidgetSlots();
    void ShowAccountWidgetSlots();
    void ShowAdvanceWidgetSlots();

    void ShowSettingWidgetSlots();
private:
    void InitWidget();
    void InitStyle();
protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::FunctionWidget *ui;
private:
    class FunctionWidgetPrivate;
    FunctionWidgetPrivate *_p;
};

#endif // FUNCTIONWIDGET_H
