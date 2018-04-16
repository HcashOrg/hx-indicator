#include "WithdrawPage.h"
#include "ui_WithdrawPage.h"

#include "WithdrawInputWidget.h"

class WithdrawPage::WithdrawPagePrivate
{
public:
    WithdrawPagePrivate(const WithdrawDataInput &data)
        :address(data.accountAddress),name(data.accountName),assetSymbol(data.assetSymbol)
        ,ammount(data.assetAmmount),assetID(data.assetID)
        ,inputWidget(new WithdrawInputWidget())
    {

    }
public:
    QString address;//地址
    QString name;
    QString assetSymbol;
    double ammount;
    QString assetID;

    WithdrawInputWidget *inputWidget;
};

WithdrawPage::WithdrawPage(const WithdrawDataInput &data,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawPage),
    _p(new WithdrawPagePrivate(data))
{
    ui->setupUi(this);
    InitWidget();
}

WithdrawPage::~WithdrawPage()
{
    delete _p;
    delete ui;
}

void WithdrawPage::ShowRecordSlots()
{

}

void WithdrawPage::InitWidget()
{
    InitStyle();

    ui->stackedWidget->addWidget(_p->inputWidget);
    _p->inputWidget->setMaxAmmount(_p->ammount);
    _p->inputWidget->setSymbol(_p->assetSymbol);

    connect(ui->toolButton,&QToolButton::clicked,this,&WithdrawPage::ShowRecordSlots);
}

void WithdrawPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,249,253));
    setPalette(palette);

    QFont font("MicrosoftYaHeiLight",20,63);
    ui->label->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::black);
    ui->label->setPalette(pa);

    ui->toolButton->setStyleSheet("QToolButton{color:white;\
                                  border-top-left-radius:10px;  \
                                  border-top-right-radius:10px; \
                                  border-bottom-left-radius:10px;  \
                                  border-bottom-right-radius:10px; \
                                  border:none;\
                                  background-color:#00D2FF;\}"
                                  "QToolButton::hover{background-color:#4861DC;}");
}
