#ifndef POUNDAGESHOWWIDGET_H
#define POUNDAGESHOWWIDGET_H

#include <memory>
#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>承税单查看页面 </summary>
///
///<remarks> 2018.04.10 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
class PoundageSheet;
class QContextMenuEvent;
namespace Ui {
class PoundageShowWidget;
}

class PoundageShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PoundageShowWidget(QWidget *parent = 0);
    ~PoundageShowWidget();
public:
    void InitData(const std::shared_ptr<PoundageSheet> &data);

    void EnableContextMenu(bool enable = false);
    void EnableDeleteAction(bool enable = false);
    void EnalbeDefaultAction(bool enable = false);
signals:
    void DeletePoundageSignal(const QString &orderID,const QString &accountName);
    void SetDefaultPoundageSignal(const QString &orderID);
private slots:
    void DeletePoundageSlots();
    void SetDefaultPoundageSlots();
private:
    void RefreshNumber();
private slots:
    void changeCurrentPageSlots(unsigned int currentPage);
    void on_tableView_clicked(const QModelIndex &index);

private:
    void InitWidget();
    void InitStyle();
    void InitContextMenu();
    void RefreshMenu();

    void setButtonWidget();//设置最后一列为删除按钮列
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    Ui::PoundageShowWidget *ui;
private:
    class PoundageShowWidgetPrivate;
    PoundageShowWidgetPrivate  *_p;
};

#endif // POUNDAGESHOWWIDGET_H
