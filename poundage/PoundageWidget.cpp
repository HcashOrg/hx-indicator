#include "PoundageWidget.h"
#include "ui_PoundageWidget.h"

#include <QDebug>
#include "PoundageDataUtil.h"
#include "PublishPoundageWidget.h"
#include "PoundageShowWidget.h"

#include "wallet.h"

class PoundageWidget::PoundageWidgetPrivate
{
public:
    PoundageWidgetPrivate()
        :allPoundageWidget(new PoundageShowWidget())
        ,myPoundageWidget(new PoundageShowWidget())
        ,allPoundageSheet(std::make_shared<PoundageSheet>())
        ,myPoundageSheet(std::make_shared<PoundageSheet>())
    {
        myPoundageWidget->EnableDeleteAction(true);
        allPoundageWidget->EnableDeleteAction(false);
    }
public:
    PoundageShowWidget *allPoundageWidget;
    PoundageShowWidget *myPoundageWidget;
    std::shared_ptr<PoundageSheet> allPoundageSheet;
    std::shared_ptr<PoundageSheet> myPoundageSheet;
};

PoundageWidget::PoundageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PoundageWidget),
    _p(new PoundageWidgetPrivate())
{
    ui->setupUi(this);
   // _p->allPoundageSheet = std::make_shared<PoundageSheet>();
   // _p->myPoundageSheet = std::make_shared<PoundageSheet>();
    InitWidget();
}

PoundageWidget::~PoundageWidget()
{
    delete _p;
    delete ui;
}

void PoundageWidget::autoRefreshSlots()
{
    if(ui->toolButton_allPoundage->isChecked())
    {
        _p->allPoundageSheet->clear();
        //刷新目前对应的承税单
        UBChain::getInstance()->postRPC("id_list_guarantee_order",
                                        toJsonFormat("list_guarantee_order",
                                                     QJsonArray()<<ui->comboBox_coinType->currentText()<<false
                                                     )
                                        );
    }
    else if(ui->toolButton_myPoundage->isChecked())
    {
        _p->myPoundageSheet->clear();

        //获取自己账户的所有信息
        foreach(QString key,UBChain::getInstance()->accountInfoMap.keys())
        {
            UBChain::getInstance()->postRPC("id_get_my_guarantee_order",
                                            toJsonFormat("get_my_guarantee_order",
                                                         QJsonArray()<<key<<false
                                                         )
                                            );
        }
    }

}

void PoundageWidget::PublishPoundageSlots()
{
    PublishPoundageWidget *publishWidget = new PublishPoundageWidget(this);
    publishWidget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::FramelessWindowHint);
    publishWidget->setWindowModality(Qt::WindowModal);
    publishWidget->setAttribute(Qt::WA_DeleteOnClose);
    publishWidget->move(mapToGlobal(QPoint(0,0)));
    publishWidget->show();
}

void PoundageWidget::ShowAllPoundageSlots()
{
    ui->toolButton_allPoundage->setChecked(true);
    ui->toolButton_myPoundage->setChecked(false);
    autoRefreshSlots();
    ui->stackedWidget->setCurrentWidget(_p->allPoundageWidget);
}

void PoundageWidget::ShowMyPoundageSlots()
{
    ui->toolButton_allPoundage->setChecked(false);
    ui->toolButton_myPoundage->setChecked(true);

    autoRefreshSlots();
    ui->stackedWidget->setCurrentWidget(_p->myPoundageWidget);
}

void PoundageWidget::SortByStuffSlots()
{
    switch (ui->comboBox_sortType->currentData(Qt::UserRole).value<int>()) {
    case 0:
        _p->allPoundageSheet->sortByTime(true);
        _p->myPoundageSheet->sortByTime(true);
        break;
    case 1:
        _p->allPoundageSheet->sortByTime(false);
        _p->myPoundageSheet->sortByTime(false);
        break;
    case 2:
        _p->allPoundageSheet->sortByRate(true);
        _p->myPoundageSheet->sortByRate(true);
        break;
    case 3:
        _p->allPoundageSheet->sortByRate(false);
        _p->myPoundageSheet->sortByRate(false);
        break;
    default:
        break;
    }

    _p->allPoundageWidget->InitData(_p->allPoundageSheet);    
    _p->myPoundageWidget->InitData(_p->myPoundageSheet);
}

void PoundageWidget::jsonDataUpdated(QString id)
{
    if("id_create_guarantee_order" == id)
    {
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;
        result.prepend("{");
        result.append("}");

        //qDebug()<<"chargecccccc"<<result;
        //刷新承税单
        autoRefreshSlots();
    }
    else if("id_list_guarantee_order" == id)
    {
        //转化为结构体
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;
        result.prepend("{");
        result.append("}");
        RefreshAllPoundageWidget(result);
    }
    else if("id_get_my_guarantee_order" == id)
    {
        //转化为结构体
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;
        result.prepend("{");
        result.append("}");

        RefreshMyPoundageWidget(result);

    }
    else if("id_cancel_guarantee_order" == id)
    {
        //转化为结构体
        QString result = UBChain::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;
        result.prepend("{");
        result.append("}");
        qDebug()<<result;
        autoRefreshSlots();
    }
}

void PoundageWidget::DeletePoundageSlots(const QString &orderID)
{
    UBChain::getInstance()->postRPC("id_cancel_guarantee_order",
                                    toJsonFormat("cancel_guarantee_order",
                                                 QJsonArray()<<orderID<<true)
                                    );
}

void PoundageWidget::RefreshAllPoundageWidget(const QString &jsonIncome)
{
    _p->allPoundageSheet->clear();
    PoundageDataUtil::convertJsonToPoundage(jsonIncome,_p->allPoundageSheet);
    SortByStuffSlots();
    _p->allPoundageWidget->InitData(_p->allPoundageSheet);
}

void PoundageWidget::RefreshMyPoundageWidget(const QString &jsonIncome)
{
    PoundageDataUtil::convertJsonToPoundage(jsonIncome,_p->myPoundageSheet);
    _p->myPoundageSheet->filterByChainType(ui->comboBox_coinType->currentText());
    SortByStuffSlots();
    _p->myPoundageWidget->InitData(_p->myPoundageSheet);
}

void PoundageWidget::InitWidget()
{
    InitStyle();

    //
    ui->toolButton_allPoundage->setCheckable(true);
    ui->toolButton_allPoundage->setChecked(true);
    ui->toolButton_myPoundage->setCheckable(true);
    ui->toolButton_myPoundage->setChecked(false);
    //初始化排序comboBox
    ui->comboBox_sortType->clear();
    ui->comboBox_sortType->addItem(tr("时间由早到晚"),0);
    ui->comboBox_sortType->addItem(tr("时间由晚到早"),1);
    ui->comboBox_sortType->addItem(tr("汇率由低到高"),2);
    ui->comboBox_sortType->addItem(tr("汇率由高到低"),3);
    ui->comboBox_sortType->setCurrentIndex(0);

    //初始化币种
    InitCoinType();

    ui->stackedWidget->addWidget(_p->allPoundageWidget);
    ui->stackedWidget->addWidget(_p->myPoundageWidget);
    ui->stackedWidget->setCurrentWidget(_p->allPoundageWidget);


    connect( UBChain::getInstance(), &UBChain::jsonDataUpdated, this, &PoundageWidget::jsonDataUpdated);

    connect(ui->pushButton_publishPoundage,&QPushButton::clicked,this,&PoundageWidget::PublishPoundageSlots); 
    connect(ui->toolButton_allPoundage,&QPushButton::clicked,this,&PoundageWidget::ShowAllPoundageSlots);
    connect(ui->toolButton_myPoundage,&QPushButton::clicked,this,&PoundageWidget::ShowMyPoundageSlots);

    connect(ui->comboBox_sortType,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&PoundageWidget::SortByStuffSlots);
    connect(ui->comboBox_coinType,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&PoundageWidget::autoRefreshSlots);

    connect(_p->myPoundageWidget,&PoundageShowWidget::DeletePoundageSignal,this,&PoundageWidget::DeletePoundageSlots);
    autoRefreshSlots();
}

void PoundageWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    QFont font("Microsoft YaHei UI Light",20,63);
    ui->label->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0,0,0));
    ui->label->setPalette(pa);

    setStyleSheet("QToolButton#toolButton_allPoundage{background:transparent;color:rgb(144,144,144);\
                                                      font-family: MicrosoftYaHei;font:14px;font-weight:600;}\
                   QToolButton#toolButton_allPoundage:checked{color:rgb(0,0,0);}\
                   QToolButton#toolButton_myPoundage{background:transparent;color:rgb(144,144,144);\
                                                     font-family: MicrosoftYaHei;font:14px;font-weight:600;}\
                   QToolButton#toolButton_myPoundage:checked{color:rgb(0,0,0);}\
                   \
                   QComboBox{    \
                   border: none;\
                   background:transparent;\
                   font-size: 10pt;\
                   font-family: MicrosoftYaHei;\
                   background-position: center left;\
                   color: black;\
                   selection-background-color: darkgray;}\
                   \
                   QPushButton#pushButton_publishPoundage{\
                   color:white;\
                   border: none;\
                   border-radius: 10px;\
                   background-color:rgb(0,210,255);} \
                   QPushButton#pushButton_publishPoundage:hover{\
                   background-color:#5474EB;}\
    ");
//                  QComboBox::down-arrow {\
//                   image: url(:/misc/down_arrow_2);\
//                 }\

}

void PoundageWidget::InitCoinType()
{
    //初始化币种(币symbol，币id)
    ui->comboBox_coinType->clear();
    foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
        if(asset.id == "1.3.0") continue;
        ui->comboBox_coinType->addItem(asset.symbol,asset.id);
    }
    if(ui->comboBox_coinType->count() > 0)
    {
        ui->comboBox_coinType->setCurrentIndex(0);
    }
}
