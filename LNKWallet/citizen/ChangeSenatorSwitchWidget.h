#ifndef CHANGESENATORSWITCHWIDGET_H
#define CHANGESENATORSWITCHWIDGET_H

#include <QWidget>
class QComboBox;
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
    void RemoveProposalSlots();
    void httpReplied(QByteArray _data, int _status);
private:
    void InitWidget();
    void SetItemVisible(int n,bool vi);//n== 2,3
    void queryWhiteList();
private:
    Ui::ChangeSenatorSwitchWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CHANGESENATORSWITCHWIDGET_H
