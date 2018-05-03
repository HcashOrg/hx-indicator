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
    explicit FeeChooseWidget(double feeNumber=20,const QString &coinType="LNK",QWidget *parent = 0);
    ~FeeChooseWidget();
public:
    const QString &GetFeeID()const;//获取手续费承兑单id--若为空，则表示为选择或者没有承兑单
    const QString &GetFeeType()const;//获取手续费类型
    QString GetFeeNumber()const;//获取手续费数量--供显示用，所以为string
public slots:
    void updateFeeNumberSlots(double feeNumber);
private slots:
    void jsonDataUpdated(QString id);

    void coinTypeChanged();

    void feeTypeChanged();
private:
    void QueryPoundage(const QString &type);

    void ParsePoundage(const std::shared_ptr<PoundageUnit> &poundage);

    void refreshUI();
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
