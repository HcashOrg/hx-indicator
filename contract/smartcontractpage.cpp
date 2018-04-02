#include "smartcontractpage.h"
#include "ui_smartcontractpage.h"

#include <QDebug>
#include <QPainter>

#include "wallet.h"
#include "dialog/addtokendialog.h"
#include "dialog/createtokendialog.h"

#define SMARTCONTRACTPAGE_SELECTED_STYLE    "QToolButton{background-color:rgb(70,82,113);color:white;border-radius:12px;}"
#define SMARTCONTRACTPAGE_UNSELECTED_STYLE  "QToolButton{background:transparent;color:rgb(192,196,212);}"  \
                                            "QToolButton:hover{color:white;}"

SmartContractPage::SmartContractPage(QWidget *parent) :
    QWidget(parent),
    detailWidget(NULL),
    ui(new Ui::SmartContractPage)
{
    ui->setupUi(this);

    detailWidget = new ContractDetailWidget(this);
    connect(detailWidget,SIGNAL(back()),this,SLOT(hideDetailWidget()));
    connect(detailWidget,SIGNAL(refresh()),this,SLOT(refresh()));
    detailWidget->hide();
    detailWidget->move(0,0);
    detailWidget->raise();

    updateContractVector();

    on_hotBtn_clicked();

}

SmartContractPage::~SmartContractPage()
{
    delete ui;
}

void SmartContractPage::showContracts(QVector<SingleContractWidget *> vector)
{
    foreach (SingleContractWidget* w, contractVector)
    {
        w->hide();
    }

    int size = vector.size();
    int row = 0;
    int column = 0;
    for(int i = 0; i < size; i++)
    {
        row = i / 3;
        column = i % 3;

        SingleContractWidget* widget = vector.at(i);
        widget->move( 30 + column * 215, 190 * row);
        widget->show();
    }
    ui->scrollAreaWidgetContents->setMinimumHeight(190 + 190 * row);
}

void SmartContractPage::refresh()
{
    updateContractVector();

    detailWidget->setContract(detailWidget->contractAddress);
}



void SmartContractPage::on_hotBtn_clicked()
{
    ui->hotBtn->setStyleSheet(SMARTCONTRACTPAGE_SELECTED_STYLE);
    ui->normalBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->mortgageBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->myBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);

    QVector<SingleContractWidget*> vector;
    foreach (SingleContractWidget* widget, contractVector)
    {
        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(widget->contractAddress);
        if(info.hotValue > 0)  vector.append(widget);
    }


    // 根据hotValue排序
    for(int i = 0; i < vector.size(); i++)
    {
        for(int j = 0; j < vector.size() - i - 1; j++)
        {
            SingleContractWidget* w1 = vector.at(j);
            int value1 = UBChain::getInstance()->ERC20TokenInfoMap.value(w1->contractAddress).hotValue;

            SingleContractWidget* w2 = vector.at(j + 1);
            int value2 = UBChain::getInstance()->ERC20TokenInfoMap.value(w2->contractAddress).hotValue;


            if( value1 < value2)
            {
                vector[j] = w2;
                vector[j + 1] = w1;
            }
        }
    }


    showContracts(vector);
}

void SmartContractPage::on_normalBtn_clicked()
{
    ui->hotBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->normalBtn->setStyleSheet(SMARTCONTRACTPAGE_SELECTED_STYLE);
    ui->mortgageBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->myBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);

    QVector<SingleContractWidget*> vector;
    foreach (SingleContractWidget* widget, contractVector)
    {
        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(widget->contractAddress);
        if(!info.isSmart)    vector.append(widget);
    }

    showContracts(vector);
}

void SmartContractPage::on_mortgageBtn_clicked()
{
    ui->hotBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->normalBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->mortgageBtn->setStyleSheet(SMARTCONTRACTPAGE_SELECTED_STYLE);
    ui->myBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);

    QVector<SingleContractWidget*> vector;
    foreach (SingleContractWidget* widget, contractVector)
    {
        ERC20TokenInfo info = UBChain::getInstance()->ERC20TokenInfoMap.value(widget->contractAddress);
        if(info.isSmart)    vector.append(widget);
    }

    showContracts(vector);
}

void SmartContractPage::on_myBtn_clicked()
{
    ui->hotBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->normalBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->mortgageBtn->setStyleSheet(SMARTCONTRACTPAGE_UNSELECTED_STYLE);
    ui->myBtn->setStyleSheet(SMARTCONTRACTPAGE_SELECTED_STYLE);

    QVector<SingleContractWidget*> vector;
    foreach (SingleContractWidget* widget, contractVector)
    {
        if(UBChain::getInstance()->myConcernedContracts.contains(widget->contractAddress))  vector.append(widget);
    }

    showContracts(vector);
}

void SmartContractPage::showDetailWidget(QString contractAddress)
{
    detailWidget->show();
    detailWidget->setContract(contractAddress);
}

void SmartContractPage::hideDetailWidget()
{
    detailWidget->hide();
}

void SmartContractPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(0,0,680,482);
}

void SmartContractPage::updateContractVector()
{
    QStringList keys = UBChain::getInstance()->ERC20TokenInfoMap.keys();

    foreach (QString key, keys)
    {
        SingleContractWidget* widget = getWidgetFromVector(key);
        if( widget != NULL)
        {
            widget->setContract(key);
        }
        else
        {
            SingleContractWidget* newWidget = new SingleContractWidget(ui->scrollAreaWidgetContents);
            connect(newWidget,SIGNAL(showDetail(QString)),this,SLOT(showDetailWidget(QString)));
            connect(newWidget,SIGNAL(refresh()),this,SLOT(refresh()));
//            newWidget->setStyleSheet("background-color:white;border:1px solid rgb(230,230,230);boder-radius:5px;");

            newWidget->setContract(key);
            contractVector.append(newWidget);
        }
    }

}

SingleContractWidget *SmartContractPage::getWidgetFromVector(QString _contractAddress)
{
    SingleContractWidget* result = NULL;
    foreach (SingleContractWidget* widget, contractVector)
    {
        if(widget->contractAddress == _contractAddress)
        {
            result = widget;
            break;
        }
    }

    return result;
}

void SmartContractPage::on_addContractBtn_clicked()
{
    AddTokenDialog addTokenDialog;
    addTokenDialog.pop();
}

void SmartContractPage::on_createContractBtn_clicked()
{
    CreateTokenDialog createTokenDialog;
    createTokenDialog.pop();
}
