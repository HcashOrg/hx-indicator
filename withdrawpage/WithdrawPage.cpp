#include "WithdrawPage.h"
#include "ui_WithdrawPage.h"

#include "WithdrawInputWidget.h"
#include "WithdrawConfirmWidget.h"

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

void WithdrawPage::ShowConfirmWidget(const QString &address, double ammount)
{//收到提现信号，弹出确认窗口
    WithdrawConfirmWidget *confirm = new WithdrawConfirmWidget(WithdrawConfirmWidget::WithdrawConfirmInput(
                                                               _p->name,QString::number(ammount),_p->assetSymbol,address),this);
    confirm->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::FramelessWindowHint);
    confirm->setWindowModality(Qt::WindowModal);
    confirm->setAttribute(Qt::WA_DeleteOnClose);
    confirm->move(mapToGlobal(QPoint(0,0)));
    confirm->show();
}

void WithdrawPage::InitWidget()
{
    InitStyle();

    ui->stackedWidget->addWidget(_p->inputWidget);
    _p->inputWidget->setMaxAmmount(_p->ammount);
    _p->inputWidget->setSymbol(_p->assetSymbol);

    connect(ui->toolButton,&QToolButton::clicked,this,&WithdrawPage::ShowRecordSlots);
    connect(_p->inputWidget,&WithdrawInputWidget::withdrawSignal,this,&WithdrawPage::ShowConfirmWidget);
}

void WithdrawPage::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    QFont font("Microsoft YaHei UI Light",20,63);
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


