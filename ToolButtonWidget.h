#ifndef TOOLBUTTONWIDGET_H
#define TOOLBUTTONWIDGET_H

#include <QWidget>

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
    void setInitNone(bool non = false);
    void setInitColor(const QColor color);
    void setEnabled(bool enabled);
    void setButtonFixSize(int width, int height);
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::ToolButtonWidget *ui;
};

class ToolButtonWidgetItem : public ToolButtonWidget
{
    Q_OBJECT

public:
    explicit ToolButtonWidgetItem(int _row, int _column, QWidget *parent = 0);

    void setBtnEnabled(bool enabled = true);

signals:
    void itemClicked(int,int);

private slots:
    void onButtonClicked();

private:
    int row;
    int column;
};

#endif // TOOLBUTTONWIDGET_H
