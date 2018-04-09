#ifndef POUNDAGEWIDGET_H
#define POUNDAGEWIDGET_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>手续费承税单界面 </summary>
///
///<remarks> 2018.04.08 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class PoundageWidget;
}

class PoundageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PoundageWidget(QWidget *parent = 0);
    ~PoundageWidget();
private slots:
    void PublishPoundageSlots();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::PoundageWidget *ui;
private:
    class PoundageWidgetPrivate;
    PoundageWidgetPrivate *_p;
};

#endif // POUNDAGEWIDGET_H
