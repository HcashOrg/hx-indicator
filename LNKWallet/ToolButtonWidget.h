#ifndef TOOLBUTTONWIDGET_H
#define TOOLBUTTONWIDGET_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class ToolButtonWidget;
}

class ToolButtonWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolButtonWidget(QWidget *parent = 0);
    ~ToolButtonWidget();
signals:
    void clicked(bool checked=false);
public:
    void setText(const QString &text);
    void setInitGray(bool gray = false);
    void setEnabled(bool enabled);
    void setButtonFixSize(int width, int height);
    void setBackgroundColor(QString color);
    void setButtonStyle(const QString &style);

    void setRedpointVisiable(bool is = false);
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::ToolButtonWidget *ui;

    virtual void resizeEvent(QResizeEvent *event);
    QLabel *redLabel;
};

class ToolButtonWidgetItem : public ToolButtonWidget
{
    Q_OBJECT

public:
    explicit ToolButtonWidgetItem(int _row, int _column, QWidget *parent = 0);

signals:
    void itemClicked(int,int);

private slots:
    void onButtonClicked();

private:
    int row;
    int column;
};

#endif // TOOLBUTTONWIDGET_H
