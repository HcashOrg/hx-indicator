#include "ContactWidget.h"
#include "ui_ContactWidget.h"


#include "ContactDataUtil.h"
#include "ContactTreeWidget.h"
#include "ContactSearchWidget.h"
#include "ContactBriefWidget.h"
#include "ContactAddWidget.h"
#include "ContactInfoWidget.h"

#include "wallet.h"

class ContactWidget::ContactWidgetPrivate
{
public:
    ContactWidgetPrivate()
        :contactTreeWidget(new ContactTreeWidget())
        ,contactSearchWidget(new ContactSearchWidget())
        ,contactBriefWidget(new ContactBriefWidget())
        ,contactAddWidget(nullptr)
        ,contactInfoWidget(nullptr)
        ,contactSheet(nullptr)
        ,contactFilePath("")
    {
        if( HXChain::getInstance()->configFile->contains("/settings/chainPath"))
        {
            contactFilePath = HXChain::getInstance()->configFile->value("/settings/chainPath").toString() + "/contacts.dat";
        }
        else
        {
            contactFilePath = HXChain::getInstance()->appDataPath + "/contacts.dat";
        }
    }
public:
    ContactTreeWidget *contactTreeWidget;//联系人编组树
    ContactSearchWidget *contactSearchWidget;//搜索界面
    ContactBriefWidget *contactBriefWidget;//右上角简介界面
    ContactAddWidget *contactAddWidget;//新增界面
    ContactInfoWidget *contactInfoWidget;//联系人显示界面

    std::shared_ptr<ContactSheet> contactSheet;//数据源

    QString contactFilePath;
};

ContactWidget::ContactWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactWidget),
    _p(new ContactWidgetPrivate())
{
    ui->setupUi(this);

    InitWidget();
    InitData();
}

ContactWidget::~ContactWidget()
{
    delete _p;
    delete ui;
}

void ContactWidget::AddNewContactSlots()
{
    if(!_p->contactAddWidget)
    {
        disconnect(_p->contactAddWidget,&ContactAddWidget::addContactFinishedSignal,_p->contactTreeWidget,&ContactTreeWidget::AddPersonSlots);
        disconnect(_p->contactTreeWidget,&ContactTreeWidget::GroupModifyFinishedSignal,_p->contactAddWidget,&ContactAddWidget::groupModifySlots);
        delete _p->contactAddWidget;
        _p->contactAddWidget = nullptr;
    }
    _p->contactAddWidget = new ContactAddWidget(_p->contactSheet);
    ui->scrollArea_rightBottom->setWidget(_p->contactAddWidget);
    connect(_p->contactAddWidget,&ContactAddWidget::addContactFinishedSignal,std::bind(&ContactWidget::backBtnVisible,this,false));
    connect(_p->contactAddWidget,&ContactAddWidget::addContactFinishedSignal,_p->contactTreeWidget,&ContactTreeWidget::AddPersonSlots);
    connect(_p->contactTreeWidget,&ContactTreeWidget::GroupModifyFinishedSignal,_p->contactAddWidget,&ContactAddWidget::groupModifySlots);
    emit backBtnVisible(true);
}

void ContactWidget::ShowContactInfoSlots(const QString &address)
{
    if(!_p->contactInfoWidget)
    {
        disconnect(_p->contactInfoWidget,&ContactInfoWidget::gotoTransferPage,this,&ContactWidget::gotoTransferPage);
    }
    if(address.isEmpty())
    {
        ui->scrollArea_rightBottom->setWidget(new QWidget);
        return;
    }
    _p->contactInfoWidget = new ContactInfoWidget();
    _p->contactInfoWidget->setData(_p->contactSheet->findPerson(address));
    ui->scrollArea_rightBottom->setWidget(_p->contactInfoWidget);
    connect(_p->contactInfoWidget,&ContactInfoWidget::gotoTransferPage,this,&ContactWidget::gotoTransferPage);
    connect(_p->contactTreeWidget,&ContactTreeWidget::PersonModifyFinishedSignal,_p->contactInfoWidget,&ContactInfoWidget::PersonModifyFinishedSlots);
}

void ContactWidget::InitStyle()
{
    //setWindowFlags(Qt::FramelessWindowHint);
    //
    //setAutoFillBackground(true);
    //QPalette palette;
    //palette.setColor(QPalette::Background, QColor(255,255,255));
    //setPalette(palette);
//    QFile fileExit("Config/styleSheet/NJHRGroupTreeView.qss");
//	fileExit.open(QIODevice::ReadOnly);
//	QString qss = fileExit.readAll();
//	setStyleSheet(qss);

    QWidget *widget = new QWidget(ui->scrollArea_rightBottom);
    widget->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(229,226,240));
    widget->setPalette(palette);
    ui->scrollArea_rightBottom->setWidget(widget);

    setStyleSheet("QScrollArea{border:none;}");
    //ui->scrollArea_leftBottom->setStyleSheet("QScrollArea{border:none;}");
    //ui->scrollArea_leftTop->setStyleSheet("QWidget{border:none;}");
    //ui->scrollArea_rightTop->setStyleSheet("QWidget{border:none;}");
    //ui->scrollArea_rightBottom->setStyleSheet("QScrollArea{border:none;}");
    //setStyleSheet("QWidget{background-color:rgb(40,46,66);color:white;}");
    //setStyleSheet("QWidget{background-color:rgb(24,28,45);border:none;}");
}

void ContactWidget::InitWidget()
{
    InitStyle();

    ui->scrollArea_leftBottom->setWidget(_p->contactTreeWidget);
    ui->scrollArea_leftTop->setWidget(_p->contactSearchWidget);
    ui->scrollArea_rightTop->setWidget(_p->contactBriefWidget);

    connect(_p->contactTreeWidget,&ContactTreeWidget::ShowContactPerson,this,&ContactWidget::ShowContactInfoSlots);
    connect(_p->contactTreeWidget,&ContactTreeWidget::SaveContact,this,&ContactWidget::SaveData);
    connect(_p->contactSearchWidget,&ContactSearchWidget::searchSignal,_p->contactTreeWidget,&ContactTreeWidget::QueryPersonSlots);
    connect(_p->contactBriefWidget,&ContactBriefWidget::addNewContactSignal,this,&ContactWidget::AddNewContactSlots);
}

void ContactWidget::InitData()
{
    if(!ContactDataUtil::readContactSheetFromPath(_p->contactFilePath,_p->contactSheet))
    {//联系人文件解析失败，自己新建一个联系人名单
       _p->contactSheet = std::make_shared<ContactSheet>();
       _p->contactSheet->sheetName = tr("New Sheet");
       std::shared_ptr<ContactGroup> group = std::make_shared<ContactGroup>();
       group->groupName = tr("Default Group");
       group->groupType = ContactGroup::GroupType_Default;
       _p->contactSheet->addGroup(group);
    }
    _p->contactTreeWidget->initContactSheet(_p->contactSheet);
}

void ContactWidget::SaveData()
{
    ContactDataUtil::writeContactSheetToPath(_p->contactFilePath,_p->contactSheet);
}

void ContactWidget::closeEvent(QCloseEvent *event)
{
    SaveData();
    QWidget::closeEvent(event);
}

//void ContactWidget::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);

//    QLinearGradient linear(QPointF(0, 510), QPointF(130, 0));
//    linear.setColorAt(0, QColor(56,19,56));
//    linear.setColorAt(1, QColor(27,17,44));
//    linear.setSpread(QGradient::PadSpread);
//    painter.setBrush(linear);
//    painter.drawRect(QRect(-1,-1,131,511));
//    QWidget::paintEvent(event);
//}
