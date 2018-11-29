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
public slots:
    void CheckUpdateSlot();
private slots:
    void CheckResultSlot(const QString &version);

    void UpdateSlot();

    void UpdateFinishSlot();
    void UpdateWrongSlot();

    void RestartSlot();
private:
    void InitWidget();
    void InitStyle();

    void showButtonState(int type);//0--检查升级  1--升级 2--重启
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::HelpWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // HELPWIDGET_H
