#ifndef CONTACTTREEWIDGET_H
#define CONTACTTREEWIDGET_H

#include <memory>
#include <QString>
#include <QTreeWidget>

//////////////////////////////////////////////////////////////////////////
///<summary>联系人左侧树类 </summary>
///
///<remarks> 2018.03.28 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
class ContactSheet;
class ContactPerson;
class ContactGroup;
class QTreeWidgetItem;
class QMouseEvent;
class QContextMenuEvent;
namespace Ui {
    class ContactTreeWidget;
}
class ContactTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ContactTreeWidget(QWidget *parent = 0);
    ~ContactTreeWidget();
signals:
    //显示联系人信息事件
    void ShowContactPerson(const QString &address);

    //组修改信号（增删改）
    void GroupModifyFinishedSignal();

    //联系人名称修改信号
    void PersonModifyFinishedSignal();

    //保存联系人
    void SaveContact();
public slots:
    //执行查询（会选中符合查询的联系人）
    void QueryPersonSlots(const QString &queryString);

    //添加联系人（外部添加联系人，自动查询所属分组，添加节点）
    void AddPersonSlots(const QString &address);
private:
    bool addGroup(const QString &groupName = "NewGroup");
    bool editGroup(QTreeWidgetItem *topItem);
    bool delGroup(QTreeWidgetItem *topItem);

    void addPerson(const std::shared_ptr<ContactPerson> &person = nullptr,QTreeWidgetItem *groupItem = nullptr);
    bool editPerson(QTreeWidgetItem *personItem);
    void delPerson(QTreeWidgetItem *personItem);

    void moveToGroup(QTreeWidgetItem *sourceItem,QTreeWidgetItem *targetItem);

private slots:
    void addGroupSlots(bool checked);
    void editGroupSlots(bool checked);
    void delGroupSlots(bool checked);

    void editPersonSlots(bool checked);
    void delPersonSlots(bool checked);

    void moveToGroupSlots(bool checked);

    void editGroupFinishSlots(/*QTreeWidgetItem *groupItem*/);
    void editPersonFinishSlots();
public:
    //////////////////////////////////////////////////////////////////////////
    ///<summary>设置联系人数据 </summary>
    ///
    ///<remarks> 2018.03.28 --朱正天  </remarks>/////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    void initContactSheet(std::shared_ptr<ContactSheet> data);
private:
    //////////////////////////////////////////////////////////////////////////
    ///<summary>初始化数据，界面 </summary>
    ///
    ///<remarks> 2018.03.28 --朱正天  </remarks>/////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    void initTreeData();
    void initTreeStyle();

    //////////////////////////////////////////////////////////////////////////
    ///<summary>初始化右键菜单 </summary>
    ///
    ///<remarks> 2018.03.28 --朱正天  </remarks>/////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    void initContextMenu();
protected:
    void contextMenuEvent(QContextMenuEvent *event);

    void mousePressEvent(QMouseEvent *event);
    //void paintEvent(QPaintEvent *event);
private:
    //////////////////////////////////////////////////////////////////////////
    ///<summary>根据数据生成节点 </summary>
    ///
    ///<remarks> 2018.03.29 --朱正天  </remarks>/////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    QTreeWidgetItem *createItemWithPerson(const std::shared_ptr<ContactPerson> &person)const;
    QTreeWidgetItem *createItemWithGroup(const std::shared_ptr<ContactGroup> &group)const;
private:
    class ContactTreeWidgetPrivate;
    ContactTreeWidgetPrivate *_p;
private:
    Ui::ContactTreeWidget *ui;
};

#endif // CONTACTTREEWIDGET_H
