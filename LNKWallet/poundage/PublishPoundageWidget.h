#ifndef PUBLISHPOUNDAGEWIDGET_H
#define PUBLISHPOUNDAGEWIDGET_H

#include <QWidget>

namespace Ui {
class PublishPoundageWidget;
}
class QLineEdit;
class PublishPoundageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PublishPoundageWidget(QWidget *parent = 0);
    ~PublishPoundageWidget();
private slots:
    void ConfirmPublishSlots();
    void ChangeAccountSlots();//修改账户项
    void ChangeAssetSlots();//修改承兑的币种
    void on_lineEdit_source_textEdited(const QString &arg1);

signals:
    void backBtnVisible(bool);
private:
    void InitAccount();
    void InitTargetCoin();
    void RefreshAllData();//初始化comboBox等必须项
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::PublishPoundageWidget *ui;
public:
    class PublishPoundageWidgetPrivate;
    PublishPoundageWidgetPrivate *_p;
protected:
    void paintEvent(QPaintEvent *event);
private:
    void installDoubleValidator(QLineEdit *editor,double min,double max,int pre=5);
};

#endif // PUBLISHPOUNDAGEWIDGET_H
