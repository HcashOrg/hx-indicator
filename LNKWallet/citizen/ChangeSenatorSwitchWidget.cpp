#include "ChangeSenatorSwitchWidget.h"
#include "ui_ChangeSenatorSwitchWidget.h"

#include "wallet.h"

Q_DECLARE_METATYPE(GuardInfo)
ChangeSenatorSwitchWidget::ChangeSenatorSwitchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangeSenatorSwitchWidget)
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

void ChangeSenatorSwitchWidget::InitWidget()
{
//    InitData();
    SetItemVisible(2,false);
    SetItemVisible(3,false);
    connect(ui->addBtn,&QToolButton::clicked,this,&ChangeSenatorSwitchWidget::AddProposalSlots);
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
        if(!showPermanent && ("EXTERNAL" != i.value().senatorType)) continue;
        if(i.value().isFormal)
        {
            ui->senator1->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
            ui->senator2->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
            ui->senator3->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
        }
        else
        {
            ui->candidate1->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
            ui->candidate2->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
            ui->candidate3->addItem(i.key(),QVariant::fromValue<GuardInfo>(i.value()));
        }
    }
}

void ChangeSenatorSwitchWidget::SetItemVisible(int n, bool vi)
{
    if(1==n)
    {
        ui->senator1->setVisible(vi);
        ui->label1->setVisible(vi);
        ui->candidate1->setVisible(vi);
    }
    else if(2==n)
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
    }
    else if(ui->senator2->isVisible())
    {
        ui->addBtn->move(ui->addBtn->x(),ui->label2->geometry().bottomRight().y()+5);
    }
    else
    {
        ui->addBtn->move(ui->addBtn->x(),ui->label1->geometry().bottomRight().y()+5);
    }
    ui->addBtn->setVisible(!ui->senator3->isVisible());
}
