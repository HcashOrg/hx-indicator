#include "ContactTreeWidget.h"
#include "ui_ContactTreeWidget.h"


#include <QTreeWidgetItem>
#include <QMenu>
#include <QLineEdit>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>

#include "ContactDataUtil.h"
#include "extra/style.h"

Q_DECLARE_METATYPE(std::shared_ptr<ContactGroup>);
Q_DECLARE_METATYPE(std::shared_ptr<ContactPerson>);
Q_DECLARE_METATYPE(QTreeWidgetItem*);

class ContactTreeWidget::ContactTreeWidgetPrivate
{
public:
    ContactTreeWidgetPrivate()
        :contactSheet(std::make_shared<ContactSheet>())
        ,contextMenu(nullptr)
    {

    }
public:
    std::shared_ptr<ContactSheet> contactSheet;//联系人表数据
    QMenu *contextMenu;//右键菜单

    QAction *addGroupAction;//添加组动作
    QAction *editGroupAction;//修改组动作
    QAction *delGroupAction;//删除组动作

    QAction *editPersonAction;//修改人员信息动作
    QAction *delPersonAction;//删除人员动作

    QMenu *movePersonMenu;//移动人员菜单
    std::vector<QAction *> movePersonAction;//移动人员动作（每次变化）
};

ContactTreeWidget::ContactTreeWidget(QWidget *parent)
    :QTreeWidget(parent)
    ,ui(new Ui::ContactTreeWidget)
    ,_p(new ContactTreeWidgetPrivate())
{
    ui->setupUi(this);
    initTreeStyle();
    initContextMenu();
}

ContactTreeWidget::~ContactTreeWidget()
{
    delete _p;
}

void ContactTreeWidget::QueryPersonSlots(const QString &queryString)
{
    if(queryString.isEmpty()) return;
    clearSelection();
    setSelectionMode(QAbstractItemView::MultiSelection);
    QList<QTreeWidgetItem*> findItems;
    QTreeWidgetItemIterator it(this);
    while(*it)
    {
        std::shared_ptr<ContactPerson> person = (*it)->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>();
        if(!(*it)->parent() || !person)
        {
            ++it;
            continue;
        }

        if(-1 != (*it)->text(0).indexOf(queryString,0,Qt::CaseInsensitive) || queryString == person->address)
        {
            findItems.push_back(*it);
            (*it)->parent()->setExpanded(true);
            (*it)->setSelected(true);
            //qDebug()<<(*it)->text(0);
        }
        ++it;
    }
    if(!findItems.isEmpty())
    {
        emit ShowContactPerson(findItems.front()->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>()->address);
    }
}

void ContactTreeWidget::AddPersonSlots(const QString &address)
{
    //查询具体信息，定位分组节点
    std::shared_ptr<ContactPerson> person = _p->contactSheet->findPerson(address);
    if(!person) return;

    //找到分组
    QTreeWidgetItem *groupItem = nullptr;
    for(int i = 0;i < topLevelItemCount();++i)
    {
        if(std::shared_ptr<ContactGroup> group = topLevelItem(i)->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>())
        {
            if(group->findPerson(address))
            {
                groupItem = topLevelItem(i);
                break;
            }
        }
    }
    if(!groupItem) return;
    groupItem->setExpanded(true);
    QTreeWidgetItem *perItem = createItemWithPerson(person);
    groupItem->addChild(perItem);

    selectionModel()->clearSelection();
    setSelectionMode(QAbstractItemView::SingleSelection);
    perItem->setSelected(true);
    emit SaveContact();
    emit ShowContactPerson(address);
}

bool ContactTreeWidget::addGroup(const QString &groupName)
{
    std::shared_ptr<ContactGroup> group = std::make_shared<ContactGroup>();
    group->groupType = ContactGroup::GroupType_Normal;
    group->groupName = groupName;
    group->number = _p->contactSheet->groups.size();
    _p->contactSheet->addGroup(group);

    QTreeWidgetItem *topItem = createItemWithGroup(group);
    addTopLevelItem(topItem);

    emit SaveContact();
    return editGroup(topItem);
}

bool ContactTreeWidget::editGroup(QTreeWidgetItem *topItem)
{
    if(!topItem) return false;
    std::shared_ptr<ContactGroup> group = topItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
    if(!group) return false;
    QLineEdit *lineEdit = new QLineEdit(group->groupName);
    lineEdit->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    lineEdit->setGeometry(QRect(mapToGlobal(pos())+visualRect(currentIndex()).topLeft(),
                                visualRect(currentIndex()).size()));
    lineEdit->show();
    lineEdit->selectAll();
    lineEdit->setFocus(Qt::OtherFocusReason);
    connect(lineEdit,&QLineEdit::editingFinished,this,&ContactTreeWidget::editGroupFinishSlots);
    return true;
}

bool ContactTreeWidget::delGroup(QTreeWidgetItem *topItem)
{
    if(!topItem) return false;

    std::shared_ptr<ContactGroup> sourceGroup = topItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
    if(!sourceGroup) return false;

    //找到默认分组（组类型为Default的）
    QTreeWidgetItem *targetGroupItem = nullptr;
    std::shared_ptr<ContactGroup> targetGroup = nullptr;
    for(int i = 0; i < topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *topItem = topLevelItem(i);
        std::shared_ptr<ContactGroup> group = topItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
        if(!group || group->groupType != ContactGroup::GroupType_Default) continue;

        targetGroupItem = topItem;
        targetGroup = group;
        break;
    }
    if(!targetGroup || !targetGroupItem) return false;

    //源分组内的的人员添加到目标分组
    targetGroup->groupPeople.insert(targetGroup->groupPeople.end(),sourceGroup->groupPeople.begin(),sourceGroup->groupPeople.end());
    foreach(std::shared_ptr<ContactPerson> person,sourceGroup->groupPeople)
    {
        targetGroupItem->addChild(createItemWithPerson(person));
    }
    //删除源分组
    _p->contactSheet->removeGroup(sourceGroup);
    takeTopLevelItem(indexOfTopLevelItem(topItem));

    emit SaveContact();
    return true;
}

void ContactTreeWidget::addPerson(const std::shared_ptr<ContactPerson> &person,QTreeWidgetItem *groupItem)
{

}

bool ContactTreeWidget::editPerson(QTreeWidgetItem *personItem)
{
    if(!personItem) return false;

    std::shared_ptr<ContactPerson> person = personItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>();
    if(!person) return false;
    QLineEdit *lineEdit = new QLineEdit(person->name);
    lineEdit->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    lineEdit->setGeometry(QRect(mapToGlobal(pos())+visualRect(currentIndex()).topLeft(),
                                visualRect(currentIndex()).size()));
    lineEdit->show();
    lineEdit->selectAll();
    lineEdit->setFocus(Qt::OtherFocusReason);
    connect(lineEdit,&QLineEdit::editingFinished,this,&ContactTreeWidget::editPersonFinishSlots);

    return true;

}

void ContactTreeWidget::delPerson(QTreeWidgetItem *personItem)
{
    if(!personItem || !personItem->parent()) return;
    std::shared_ptr<ContactPerson> person = personItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>();
    std::shared_ptr<ContactGroup> group = personItem->parent()->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
    if(!person || !group) return;

    group->removePerson(person);
    emit SaveContact();

    //界面上移除本item，选择下一个item
    QTreeWidgetItem *parentItem = personItem->parent();
    if(!parentItem) return;
    parentItem->removeChild(personItem);

    QTreeWidgetItem * currItem = nullptr;
    if(parentItem->childCount() >= 1)
    {
        currItem = parentItem->child(0);
    }
    else
    {
        for(int i = 0;i < topLevelItemCount();++i)
        {
            if(topLevelItem(i)->childCount() >= 1)
            {
                currItem = topLevelItem(i)->child(0);
                break;
            }
        }
    }

    if(!currItem)
    {
        emit ShowContactPerson("");
        return;
    }
    selectionModel()->clearSelection();
    setSelectionMode(QAbstractItemView::SingleSelection);
    currItem->setSelected(true);
    emit ShowContactPerson(currItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>()->address);
}

void ContactTreeWidget::moveToGroup(QTreeWidgetItem *sourceItem, QTreeWidgetItem *targetItem)
{
    if(!sourceItem || !targetItem) return;

    std::shared_ptr<ContactPerson> person = sourceItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>();
    std::shared_ptr<ContactGroup> soureceGroup = sourceItem->parent()->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
    std::shared_ptr<ContactGroup> targetGroup = targetItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
    if(!person || !soureceGroup || !targetGroup) return;

    //从数据源中删除
    soureceGroup->removePerson(person);

    //加入到目标源中
    targetGroup->groupPeople.push_back(person);

    //从树中删除
    sourceItem->parent()->removeChild(sourceItem);
    QTreeWidgetItem * item = createItemWithPerson(person);
    if(item)
    {
        clearSelection();
        targetItem->addChild(item);
        item->setSelected(true);
    }
    emit SaveContact();
}

void ContactTreeWidget::addGroupSlots(bool checked)
{
    if(addGroup("NewGroup"))
    {
        emit GroupModifyFinishedSignal();
    }
}

void ContactTreeWidget::editGroupSlots(bool checked)
{
    editGroup(currentItem());
}

void ContactTreeWidget::delGroupSlots(bool checked)
{
    if(delGroup(currentItem()))
    {
        emit GroupModifyFinishedSignal();
    }
}

void ContactTreeWidget::editPersonSlots(bool checked)
{
    editPerson(currentItem());
}

void ContactTreeWidget::delPersonSlots(bool checked)
{
   delPerson(currentItem());
}

void ContactTreeWidget::moveToGroupSlots(bool checked)
{
    QTreeWidgetItem *currentItem = this->currentItem();
    QAction* action = qobject_cast<QAction*>(sender());
    if(!currentItem || !currentItem->parent() || !action) return;

    QTreeWidgetItem *toItem = action->data().value<QTreeWidgetItem*>();
    if(!toItem) return;

    moveToGroup(currentItem,toItem);
}

void ContactTreeWidget::editGroupFinishSlots()
{
    QLineEdit * lineEdit =qobject_cast<QLineEdit *>(sender());
    if(!lineEdit) return;

    disconnect(lineEdit,&QLineEdit::editingFinished,this,&ContactTreeWidget::editGroupFinishSlots);
    QString newName = lineEdit->text();

    std::shared_ptr<ContactGroup> group = currentItem()->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
    if(!group)
    {
        lineEdit->close();
        return;
    }

    if(_p->contactSheet->renameGroup(newName,group))
    {
        currentItem()->setText(0,newName);
        emit GroupModifyFinishedSignal();
        emit SaveContact();
    }
    else
    {
        QMessageBox::information(nullptr,tr("Warning"), tr("Cannot Rename Group!"));
        //组重命名的提示
    }
    lineEdit->close();
}

void ContactTreeWidget::editPersonFinishSlots()
{
    QLineEdit * lineEdit =qobject_cast<QLineEdit *>(sender());
    if(!lineEdit) return;

    disconnect(lineEdit,&QLineEdit::editingFinished,this,&ContactTreeWidget::editPersonFinishSlots);
    QString newName = lineEdit->text();

    std::shared_ptr<ContactPerson> person = currentItem()->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>();
    if(!person)
    {
        lineEdit->close();
        return;
    }

    person->name = newName;
    currentItem()->setText(0,newName);
    emit PersonModifyFinishedSignal();
    emit SaveContact();
    lineEdit->close();
}

void ContactTreeWidget::initContactSheet(std::shared_ptr<ContactSheet> data)
{
    _p->contactSheet = data;
    initTreeData();
    //默认选中第一个
    if( 0 < topLevelItemCount() && 0 < topLevelItem(0)->childCount())
    {
        QTreeWidgetItem *item = topLevelItem(0)->child(0);
        if(item)
        {
            setItemSelected(item,true);
            emit ShowContactPerson(item->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>()->address);
        }

    }
}

void ContactTreeWidget::initTreeStyle()
{
    setWindowFlags(Qt::FramelessWindowHint);//无边框
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setStyleSheet("QWidget{border:none;}"
                  "QTreeView{background-color:rgb(55,36,88);border:none;padding-left:0px;\
                      color:rgb(104,86,144);font:65 14px \"Microsoft YaHei UI\";outline:0px;}"
                  "QTreeView::item{height:32px;}"
                  "QTreeView::item::selected{border-bottom:1px solid rgb(255,255,255);color:white;}"
                  "QTreeView::branch:selected:has-children:adjoins-item {border-bottom:1px solid rgb(255,255,255);}"
                  "QTreeView::branch::selected::adjoins-item{border-bottom:1px solid rgb(255,255,255);}"
                  "QTreeView::branch::selected::!adjoins-item{border-bottom:1px solid rgb(255,255,255);}"

        "QTreeView::branch::hover::adjoins-item,QTreeView::branch::hover::!adjoins-item,QTreeView::item::hover{background:rgb(55,36,88);}"

        "QTreeView::branch:has-children:!has-siblings:closed,QTreeView::branch:closed:has-children:has-siblings { border-image: none;\
            image: url(:/close.png);}"

        "QTreeView::branch:open:has-children:!has-siblings,QTreeView::branch:open:has-children:has-siblings  {\
            border-image: none;image: url(:/open.png);}"

                 );

}

void ContactTreeWidget::initContextMenu()
{
    _p->contextMenu = new QMenu(this);
    _p->contextMenu->setStyleSheet(MENU_STYLE);
    _p->movePersonMenu = new QMenu(tr("MoveToGroup"),this);

    _p->addGroupAction = new QAction(tr("AddGroup"),this);

    _p->editGroupAction = new QAction(tr("EditGroup"),this);
    _p->delGroupAction = new QAction(tr("DeleteGroup"),this);

    _p->editPersonAction = new QAction(tr("EditContact"),this);
    _p->delPersonAction = new QAction(tr("DeleteContact"),this);

    _p->contextMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    _p->contextMenu->setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | _p->contextMenu->windowFlags());

    _p->movePersonMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    _p->movePersonMenu->setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | _p->movePersonMenu->windowFlags());

    connect(_p->addGroupAction,&QAction::triggered,this,&ContactTreeWidget::addGroupSlots);
    connect(_p->editGroupAction,&QAction::triggered,this,&ContactTreeWidget::editGroupSlots);
    connect(_p->delGroupAction,&QAction::triggered,this,&ContactTreeWidget::delGroupSlots);

    connect(_p->editPersonAction,&QAction::triggered,this,&ContactTreeWidget::editPersonSlots);
    connect(_p->delPersonAction,&QAction::triggered,this,&ContactTreeWidget::delPersonSlots);
}

void ContactTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
    setCurrentItem(itemAt(event->pos()));
    QTreeWidgetItem *currentItem = this->currentItem();

    _p->contextMenu->clear();

    if (nullptr == currentItem)
    {//空白处，只显示添加组
        _p->contextMenu->addAction(_p->addGroupAction);
    }
    else if (std::shared_ptr<ContactGroup> group = currentItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>())
    {//组右键，修改、删除
        _p->contextMenu->addAction(_p->editGroupAction);
        if(group->groupType != ContactGroup::GroupType_Default)
        {
            _p->contextMenu->addAction(_p->delGroupAction);
        }
    }
    else if (std::shared_ptr<ContactPerson> person = currentItem->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>())
    {
        _p->contextMenu->addAction(_p->editPersonAction);
        _p->contextMenu->addSeparator();
        _p->contextMenu->addAction(_p->delPersonAction);
        _p->contextMenu->addSeparator();
        _p->contextMenu->addMenu(_p->movePersonMenu);
        //刷新分组菜单
        _p->movePersonMenu->clear();

        for(int i = 0;i < topLevelItemCount();++i)
        {
            std::shared_ptr<ContactGroup> group = topLevelItem(i)->data(0,Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
            if(!group) continue;
            bool isInThisGroup = false;
            for(auto it = group->groupPeople.begin();it != group->groupPeople.end();++it)
            {
                if((*it)->address == person->address)
                {
                    isInThisGroup = true;
                    break;
                }
            }
            if (!isInThisGroup)
            {
                QAction *action = new QAction(group->groupName,this);
                action->setData(QVariant::fromValue<QTreeWidgetItem*>(topLevelItem(i)));
                connect(action,&QAction::triggered,this,&ContactTreeWidget::moveToGroupSlots);
                _p->movePersonMenu->addAction(action);
            }

        }
    }
    _p->contextMenu->exec(QCursor::pos());
}

void ContactTreeWidget::mousePressEvent(QMouseEvent *event)
{
    setSelectionMode(QAbstractItemView::SingleSelection);

    if(QTreeWidgetItem *item = itemAt(QPoint(event->localPos().x(),event->localPos().y())))
    {
        if(std::shared_ptr<ContactPerson> person = item->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>())
        {
            emit ShowContactPerson(person->address);
        }
    }
    else
    {
        clearSelection();
    }

    QTreeWidget::mousePressEvent(event);
}

QTreeWidgetItem *ContactTreeWidget::createItemWithPerson(const std::shared_ptr<ContactPerson> &person) const
{
    if(!person) return nullptr;
    QTreeWidgetItem *personItem = new QTreeWidgetItem(QStringList()<<person->name);
    personItem->setToolTip(0,person->name);
    personItem->setData(0,Qt::UserRole,QVariant::fromValue<std::shared_ptr<ContactPerson>>(person));
    return personItem;
}

QTreeWidgetItem *ContactTreeWidget::createItemWithGroup(const std::shared_ptr<ContactGroup> &group) const
{
    if(!group) return nullptr;
    QTreeWidgetItem *groupItem = new QTreeWidgetItem(QStringList()<<group->groupName);
    groupItem->setToolTip(0,group->groupName);
    groupItem->setData(0,Qt::UserRole,QVariant::fromValue<std::shared_ptr<ContactGroup>>(group));
    return groupItem;
}

void ContactTreeWidget::initTreeData()
{
    if(!_p->contactSheet) return;

    clear();
    foreach (std::shared_ptr<ContactGroup> group, _p->contactSheet->groups) {
        QTreeWidgetItem * topItem = createItemWithGroup(group);
        addTopLevelItem(topItem);
        foreach(std::shared_ptr<ContactPerson> person,group->groupPeople){
             topItem->addChild(createItemWithPerson(person));
        }
    }
    expandAll();
}

//void ContactTreeWidget::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);

//    QLinearGradient linear(QPointF(0, 408), QPointF(130, 0));
//    linear.setColorAt(0, QColor(56,19,56));
//    linear.setColorAt(1, QColor(27,17,44));
//    linear.setSpread(QGradient::PadSpread);
//    painter.setBrush(linear);
//    painter.drawRect(QRect(-1,-1,131,408));
//    QWidget::paintEvent(event);
//}
