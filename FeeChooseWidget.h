#ifndef FEECHOOSEWIDGET_H
#define FEECHOOSEWIDGET_H

#include <memory>
#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>手续费承兑单选择界面 </summary>
///
///<remarks> 2018.05.02 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class FeeChooseWidget;
}

class PoundageUnit;
class FeeChooseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FeeChooseWidget(double feeNumber=20,const QString &coinType="LNK",const QString &accountName = "",QWidget *parent = 0);
    ~FeeChooseWidget();
public:
    const QString &GetFeeID()const;//获取手续费承兑单id--若为空，则表示未选择或者没有合适的承兑单
    const QString &GetFeeType()const;//获取手续费类型
    QString GetFeeNumber()const;//获取手续费数量--供显示用，所以为string
public slots:
    void updateFeeNumberSlots(double feeNumber);
    void updatePoundageID();
    void updateAccountNameSlots(const QString &accountName,bool refreshTip = false);
private slots:
    void jsonDataUpdated(QString id);

    void coinTypeChanged();

    void feeTypeChanged();
private:
    void QueryPoundage(const QString &type);


    void ParsePoundage(const std::shared_ptr<PoundageUnit> &poundage);

    void refreshUI();

    bool checkAccountBalance()const;//足够返回true，否则返回false
    void ResetAccountBalance();
    void InitCoinType();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::FeeChooseWidget *ui;
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // FEECHOOSEWIDGET_H
