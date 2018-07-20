#include "WithdrawPage.h"
#include "ui_WithdrawPage.h"

#include "WithdrawInputWidget.h"
#include "WithdrawConfirmWidget.h"
#include "withdrawrecordwidget.h"
#include "wallet.h"

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
    QString ammount;
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

void WithdrawPage::ShowConfirmWidget(const QString &address, const QString & ammount)
{//收到提现信号，弹出确认窗口
    WithdrawConfirmWidget *confirm = new WithdrawConfirmWidget(WithdrawConfirmWidget::WithdrawConfirmInput(
                                                               _p->name,ammount,_p->assetSymbol,address),
                                                               HXChain::getInstance()->mainFrame);
    connect(confirm,&WithdrawConfirmWidget::closeSelf,this,&WithdrawPage::close);
    connect(confirm,&WithdrawConfirmWidget::closeSelf,std::bind(&WithdrawPage::backBtnVisible,this,false));
    //confirm->setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint);
    //confirm->setWindowModality(Qt::WindowModal);
    confirm->setAttribute(Qt::WA_DeleteOnClose);
    //confirm->move(mapToGlobal(QPoint(-190,-50)));
    confirm->show();
}

void WithdrawPage::InitWidget()
{
    InitStyle();

    ui->stackedWidget->addWidget(_p->inputWidget);
    _p->inputWidget->InitData(_p->ammount,_p->assetSymbol);

    connect(ui->withdrawRecordBtn,&QToolButton::clicked,this,&WithdrawPage::ShowRecordSlots);
    connect(_p->inputWidget,&WithdrawInputWidget::withdrawSignal,this,&WithdrawPage::ShowConfirmWidget);
}

void WithdrawPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);


    ui->withdrawRecordBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
}



void WithdrawPage::on_withdrawRecordBtn_clicked()
{
    WithdrawRecordWidget* widget = new WithdrawRecordWidget(this);
    widget->move(0,0);
    widget->show();
    widget->showWithdrawRecord(_p->address);
}
