#ifndef PUBLISHPOUNDAGEWIDGET_H
#define PUBLISHPOUNDAGEWIDGET_H

#include <QWidget>

namespace Ui {
class PublishPoundageWidget;
}

class PublishPoundageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PublishPoundageWidget(QWidget *parent = 0);
    ~PublishPoundageWidget();
private slots:
    void ConfirmPublishSlots();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::PublishPoundageWidget *ui;
public:
    class PublishPoundageWidgetPrivate;
    PublishPoundageWidgetPrivate *_p;
};

#endif // PUBLISHPOUNDAGEWIDGET_H
