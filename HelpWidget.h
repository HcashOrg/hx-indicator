#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QWidget>

namespace Ui {
class HelpWidget;
}

class HelpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HelpWidget(QWidget *parent = 0);
    ~HelpWidget();
private slots:
    void CheckUpdateSlot();
    void CheckResultSlot(const QString &version);

    void UpdateSlot();

    void UpdateFinishSlot();
private:
    void InitWidget();
    void InitStyle();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::HelpWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // HELPWIDGET_H
