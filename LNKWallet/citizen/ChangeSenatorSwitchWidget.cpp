#include "ChangeSenatorSwitchWidget.h"
#include "ui_ChangeSenatorSwitchWidget.h"

#include <QJsonObject>
#include <QJsonDocument>

#include "wallet.h"
#include "extra/HttpManager.h"

Q_DECLARE_METATYPE(GuardInfo)

class ChangeSenatorSwitchWidget::DataPrivate
{
public:
  DataPrivate()
  {

  }
public:
  HttpManager httpManager;

};

ChangeSenatorSwitchWidget::ChangeSenatorSwitchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangeSenatorSwitchWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ChangeSenatorSwitchWidget::~ChangeSenatorSwitchWidget()
{
    delete ui;
}

QMap<QString, QString> ChangeSenatorSwitchWidget::getReplacePair() const
{
    QMap<QString,QString> changeMap;
    if(ui->senator1->isVisible() && !ui->senator1->currentText().isEmpty() && !ui->candidate1->currentText().isEmpty())
    {
        changeMap[ui->candidate1->currentData().value<GuardInfo>().accountId]=ui->senator1->currentData().value<GuardInfo>().accountId;
    }
    if(ui->senator2->isVisible() && !ui->senator2->currentText().isEmpty() && !ui->candidate2->currentText().isEmpty())
    {
        changeMap[ui->candidate2->currentData().value<GuardInfo>().accountId]=ui->senator2->currentData().value<GuardInfo>().accountId;
    }
    if(ui->senator3->isVisible() && !ui->senator3->currentText().isEmpty() && !ui->candidate3->currentText().isEmpty())
    {
        changeMap[ui->candidate3->currentData().value<GuardInfo>().accountId]=ui->senator3->currentData().value<GuardInfo>().accountId;
    }
    return changeMap;
}

void ChangeSenatorSwitchWidget::AddProposalSlots()
{
    if(!ui->senator2->isVisible())
    {
        SetItemVisible(2,true);
    }
    else if(!ui->senator3->isVisible())
    {
        SetItemVisible(3,true);
    }
}

void ChangeSenatorSwitchWidget::RemoveProposalSlots()
{
    if(ui->senator3->isVisible())
    {
        SetItemVisible(3,false);
    }
    else if(ui->senator2->isVisible())
    {
        SetItemVisible(2,false);
    }
}

void ChangeSenatorSwitchWidget::httpReplied(QByteArray _data, int _status)
{
//    qDebug()<<"opop"<<_data;
    ui->senator1->clear();
    ui->senator2->clear();
    ui->senator3->clear();
    QJsonArray arr  = QJsonDocument::fromJson(_data).object().value("result").toArray();
    foreach (QJsonValue val, arr) {
        if(!val.isString()) continue;
        QMap<QString,GuardInfo> allSenator(HXChain::getInstance()->allGuardMap);
        QMapIterator<QString, GuardInfo> i(allSenator);
        while (i.hasNext()) {
            i.next();
//            qDebug()<<"qqq"<<i.value().accountId<<i.value().senatorType<<i.value().isFormal;
            if(i.value().isFormal)
            {
                if(val.toString() == i.value().accountId && "EXTERNAL" == i.value().senatorType)
                {
                    ui->senator1->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
                    ui->senator2->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
                    ui->senator3->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
                    break;
                }
            }
        }
    }
    if(0 == ui->senator1->count()  || 0 == ui->candidate1->count())
    {
        ui->addBtn->setVisible(false);
    }
    else
    {
        ui->addBtn->setVisible(true);
    }
    //强制不显示增加按钮
    ui->addBtn->setVisible(false);

}

void ChangeSenatorSwitchWidget::InitWidget()
{
    SetItemVisible(2,false);
    SetItemVisible(3,false);
    connect(ui->addBtn,&QToolButton::clicked,this,&ChangeSenatorSwitchWidget::AddProposalSlots);
    connect(ui->delBtn,&QToolButton::clicked,this,&ChangeSenatorSwitchWidget::RemoveProposalSlots);
    connect(&_p->httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));
}

void ChangeSenatorSwitchWidget::InitData(bool showPermanent)
{
    ui->senator1->clear();
    ui->senator2->clear();
    ui->senator3->clear();
    ui->candidate1->clear();
    ui->candidate2->clear();
    ui->candidate3->clear();

    //初始化提案combobox
    QMap<QString,GuardInfo> allSenator(HXChain::getInstance()->allGuardMap);
    QMapIterator<QString, GuardInfo> i(allSenator);
    while (i.hasNext()) {
        i.next();
        if(i.value().isFormal)
        {
            if((showPermanent && ("PERMANENT" == i.value().senatorType)) || (!showPermanent && ("EXTERNAL" == i.value().senatorType)))
            {
                ui->senator1->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
                ui->senator2->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
                ui->senator3->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
            }
        }
        else
        {
            ui->candidate1->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
            ui->candidate2->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
            ui->candidate3->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
        }
    }
    if(0 == ui->senator1->count()  || 0 == ui->candidate1->count())
    {
        ui->addBtn->setVisible(false);
    }
    //强制不显示增加按钮
    ui->addBtn->setVisible(false);
//    //citizen查询白名单
//    if(!showPermanent)
//    {
//        queryWhiteList();
//    }
}

void ChangeSenatorSwitchWidget::SetItemVisible(int n, bool vi)
{
    if(2==n)
    {
        ui->senator2->setVisible(vi);
        ui->label2->setVisible(vi);
        ui->candidate2->setVisible(vi);
    }
    else if(3==n)
    {
        ui->senator3->setVisible(vi);
        ui->label3->setVisible(vi);
        ui->candidate3->setVisible(vi);
    }

    if(ui->senator3->isVisible())
    {
        ui->addBtn->move(ui->addBtn->x(), ui->label3->geometry().bottomRight().y()+5);
        ui->delBtn->move(ui->delBtn->x(), ui->label3->geometry().bottomRight().y()+5);
    }
    else if(ui->senator2->isVisible())
    {
        ui->addBtn->move(ui->addBtn->x(),ui->label2->geometry().bottomRight().y()+5);
        ui->delBtn->move(ui->delBtn->x(),ui->label2->geometry().bottomRight().y()+5);
    }
    else
    {
        ui->addBtn->move(ui->addBtn->x(),ui->label1->geometry().bottomRight().y()+5);
        ui->delBtn->move(ui->delBtn->x(),ui->label1->geometry().bottomRight().y()+5);
    }

    ui->addBtn->setVisible(!ui->senator3->isVisible());
    ui->delBtn->setVisible(ui->senator2->isVisible());
}

void ChangeSenatorSwitchWidget::queryWhiteList()
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Plugin.QueryWhiteListSenatorId");
    _p->httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}
