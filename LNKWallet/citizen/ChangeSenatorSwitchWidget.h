#ifndef CHANGESENATORSWITCHWIDGET_H
#define CHANGESENATORSWITCHWIDGET_H

#include <QWidget>

namespace Ui {
class ChangeSenatorSwitchWidget;
}

class ChangeSenatorSwitchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChangeSenatorSwitchWidget(QWidget *parent = 0);//是否显示5个常任的senator
    ~ChangeSenatorSwitchWidget();
public:
    QMap<QString,QString> getReplacePair()const;//new_acct_id old_acct_id

    void InitData(bool showPermanent = false);
private slots:
    void AddProposalSlots();
private:
    void InitWidget();
    void SetItemVisible(int n,bool vi);//n== 1,2,3
private:
    Ui::ChangeSenatorSwitchWidget *ui;
};

#endif // CHANGESENATORSWITCHWIDGET_H
