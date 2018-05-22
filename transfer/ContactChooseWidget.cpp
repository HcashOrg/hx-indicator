#include "ContactChooseWidget.h"
#include "ui_ContactChooseWidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include "contact/ContactDataUtil.h"
#include "wallet.h"
#include "commondialog.h"

Q_DECLARE_METATYPE(std::shared_ptr<ContactGroup>);
Q_DECLARE_METATYPE(std::shared_ptr<ContactPerson>);

class ContactChooseWidget::ContactChooseWidgetPrivate
{
public:
    ContactChooseWidgetPrivate()
        :contactSheet(nullptr)
        ,contactFilePath("")
    {
        if( UBChain::getInstance()->configFile->contains("/settings/chainPath"))
        {
            contactFilePath = UBChain::getInstance()->configFile->value("/settings/chainPath").toString() + "/contacts.dat";
        }
        else
        {
            contactFilePath = UBChain::getInstance()->appDataPath + "/contacts.dat";
        }
    }
public:
    std::shared_ptr<ContactSheet> contactSheet;//数据源
    QString contactFilePath;

    QString contactName;
    QString contactAddress;
};

ContactChooseWidget::ContactChooseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactChooseWidget),
    _p(new ContactChooseWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ContactChooseWidget::~ContactChooseWidget()
{
    delete _p;
    delete ui;
}

void ContactChooseWidget::itemClickedSlots(QTreeWidgetItem *item)
{
    if(!item) return;
    std::shared_ptr<ContactPerson> person = item->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>();
    if(!person) return;

    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _p->contactName = person->name;
    _p->contactAddress = person->address;
    updateUI();
}

void ContactChooseWidget::itemDoubleClickedSlots(QTreeWidgetItem *item)
{
    if(!item) return;
    std::shared_ptr<ContactPerson> person = item->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>();
    if(!person) return;

    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _p->contactName = person->name;
    _p->contactAddress = person->address;
    updateUI();
    confirmSlots();
}

void ContactChooseWidget::confirmSlots()
{
    if(!_p->contactName.isEmpty() && !_p->contactAddress.isEmpty())
    {
        emit selectContactSignal(_p->contactName,_p->contactAddress);
    }

    emit closeSignal();
    close();
}

void ContactChooseWidget::QueryPersonSlots()
{
    QString queryString = ui->lineEdit->text();
    if(queryString.isEmpty()) return;
    ui->treeWidget->clearSelection();
    ui->treeWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    QList<QTreeWidgetItem*> findItems;
    QTreeWidgetItemIterator it(ui->treeWidget);
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
        itemClickedSlots(findItems.front());
    }
    else
    {
        _p->contactName = "";
        _p->contactAddress = "";
        updateUI();
    }
}

void ContactChooseWidget::updateUI()
{
    ui->name->setText(_p->contactName);
    //ui->addressLabel->setText(_p->contactAddress);
    ui->address->setText(_p->contactAddress);
}

void ContactChooseWidget::InitWidget()
{
    InitStyle();
    InitData();

    QPushButton *pSearchButton = new QPushButton(this);

    pSearchButton->setCursor(Qt::PointingHandCursor);
    pSearchButton->setFixedSize(20, 20);
    pSearchButton->setToolTip(tr("Search"));
    pSearchButton->setIconSize(QSize(18,18));
    pSearchButton->setIcon(QIcon(":/transfer/search.png"));

    //防止文本框输入内容位于按钮之下
    QMargins margins = ui->lineEdit->textMargins();
    ui->lineEdit->setTextMargins(10+pSearchButton->width(), margins.top(), margins.right(), margins.bottom());

    QHBoxLayout *pSearchLayout = new QHBoxLayout();
    pSearchLayout->addStretch();
    pSearchLayout->addWidget(pSearchButton);
    pSearchLayout->setSpacing(0);
    pSearchLayout->setDirection(QBoxLayout::RightToLeft);
    pSearchLayout->setContentsMargins(10, 0, 0, 0);
    ui->lineEdit->setLayout(pSearchLayout);


    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,&ContactChooseWidget::itemClickedSlots);
    connect(ui->treeWidget,&QTreeWidget::itemDoubleClicked,this,&ContactChooseWidget::itemDoubleClickedSlots);
    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&ContactChooseWidget::closeSignal);
    connect(ui->toolButton_close,&QToolButton::clicked,this,&ContactChooseWidget::closeSignal);
    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&ContactChooseWidget::close);
    connect(ui->toolButton_close,&QToolButton::clicked,this,&ContactChooseWidget::close);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&ContactChooseWidget::confirmSlots);

    connect(pSearchButton, &QPushButton::clicked, this, &ContactChooseWidget::QueryPersonSlots);

    connect(ui->lineEdit,&QLineEdit::textChanged,this, &ContactChooseWidget::QueryPersonSlots);

}

void ContactChooseWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    ui->treeWidget->setHeaderHidden(true);
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidget->setStyleSheet(
                  "QTreeView{background-color:transparent;border:none;\
                      color:black;font-size:16px;outline:0px;margin:0 0 0 20px;}"
                  "QTreeView::branch{background:transparent;}"
                  "QTreeView::item{height:20px;}"
                  "QTreeView::item::selected{background:#829DFF;border:none;color:white;}"
                  "QTreeView::branch::selected::adjoins-item,QTreeView::branch::selected::!adjoins-item{background:#829DFF;}"
                  "QTreeView::branch::hover::adjoins-item,QTreeView::branch::hover::!adjoins-item,QTreeView::item::hover{background:#829DFF;}"
                 );

    ui->toolButton_close->setStyleSheet(CLOSEBTN_STYLE);
    ui->toolButton_cancel->setStyleSheet(CANCELBTN_STYLE);
    ui->toolButton_confirm->setStyleSheet(OKBTN_STYLE);

    setStyleSheet("QWidget#ContactChooseWidget{border-radius:15px;}"
                  "QPushButton{ background:transparent;}"
                  "QLineEdit{background-color:transparent;border: 1px solid #999999 ;border-radius:15px;color:#5474EB;}");
    //ui->addressLabel->setWordWrap(true);

}

void ContactChooseWidget::InitData()
{
    if(!ContactDataUtil::readContactSheetFromPath(_p->contactFilePath,_p->contactSheet))
    {//联系人文件解析失败，提示用户没有联系人
        close();
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("Contact file doesn't exist or damaged!"));
        dia.pop();
        return;
    }
    if(!_p->contactSheet) return;
//创建联系人树
    ui->treeWidget->clear();
    foreach (std::shared_ptr<ContactGroup> group, _p->contactSheet->groups) {
        QTreeWidgetItem *groupItem = new QTreeWidgetItem(QStringList()<<group->groupName);
        groupItem->setToolTip(0,group->groupName);
        groupItem->setData(0,Qt::UserRole,QVariant::fromValue<std::shared_ptr<ContactGroup>>(group));
        ui->treeWidget->addTopLevelItem(groupItem);
        foreach(std::shared_ptr<ContactPerson> person,group->groupPeople){
            QTreeWidgetItem *personItem = new QTreeWidgetItem(QStringList()<<person->name);
            personItem->setToolTip(0,person->name);
            personItem->setData(0,Qt::UserRole,QVariant::fromValue<std::shared_ptr<ContactPerson>>(person));
            groupItem->addChild(personItem);
        }
    }
    ui->treeWidget->expandAll();
}

void ContactChooseWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setBrush(QBrush(QColor(248,249,253)));
    painter.setPen(Qt::transparent);

    painter.drawRoundedRect(rect(), 25, 25);
    //QWidget::paintEvent(event);
}
