#include "MessageNotifyWidget.h"
#include "ui_MessageNotifyWidget.h"

#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QLinearGradient>
#include <QDebug>
class MessageNotifyWidget::DataPrivate
{
public:
    DataPrivate()
        :showTimer(nullptr)
        ,stayTimer(nullptr)
        ,closeTimer(nullptr)
    {

    }
public:
    QTimer *showTimer;
    QTimer *stayTimer;
    QTimer *closeTimer;
};

MessageNotifyWidget::MessageNotifyWidget(const QString &info,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageNotifyWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWdiget();
}

MessageNotifyWidget::~MessageNotifyWidget()
{
    delete _p;
    delete ui;
}

void MessageNotifyWidget::SetMessageContext(const QString &info)
{
    ui->textBrowser->setText(info);
}

void MessageNotifyWidget::OnShowSlots()
{
    if(this->geometry().bottomRight().y() < QApplication::desktop()->availableGeometry().height())
    {
        this->setWindowOpacity(1);
        _p->showTimer->stop();
        _p->stayTimer->start(2000);
    }
    else
    {
        move(this->geometry().x(), this->geometry().topLeft().y()-1);
        this->setWindowOpacity(this->windowOpacity()+1.0/this->geometry().height());
        _p->showTimer->start(5);
    }
}

void MessageNotifyWidget::OnStaySlots()
{
    _p->stayTimer->stop();
    _p->closeTimer->start(10);
}

void MessageNotifyWidget::OnCloseSlots()
{
    if(this->windowOpacity() <= 0)
    {
        _p->closeTimer->stop();
        close();
    }
    else
    {
        setWindowOpacity(this->windowOpacity()-0.0035);
    }
}

void MessageNotifyWidget::ShowWidget()
{
    QRect rect = QApplication::desktop()->availableGeometry();
    move(rect.width()-this->width(),rect.height());
    _p->showTimer->start(1);
    this->setWindowOpacity(0);
    show();
}

void MessageNotifyWidget::enterEvent(QEvent *event)
{
    _p->closeTimer->stop();
    this->setWindowOpacity(1);
}

void MessageNotifyWidget::leaveEvent(QEvent *event)
{
    _p->stayTimer->start(1000);
}

void MessageNotifyWidget::InitWdiget()
{
    setWindowFlags(Qt::SubWindow | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    InitStyle();

    _p->showTimer = new QTimer(this);
    _p->stayTimer = new QTimer(this);
    _p->closeTimer = new QTimer(this);
    connect(_p->showTimer,&QTimer::timeout,this,&MessageNotifyWidget::OnShowSlots);
    connect(_p->stayTimer,&QTimer::timeout,this,&MessageNotifyWidget::OnStaySlots);
    connect(_p->closeTimer,&QTimer::timeout,this,&MessageNotifyWidget::OnCloseSlots);
    connect(ui->toolButton,&QToolButton::clicked,this,&MessageNotifyWidget::close);
}

void MessageNotifyWidget::InitStyle()
{

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);

    ui->toolButton->setIconSize(QSize(12,12));
    ui->toolButton->setIcon(QIcon(":/notifyWidget/close.png"));
    ui->toolButton->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                            "QToolButton:hover{background-color:#5474EB;}");


    QLinearGradient linearGradient(0, 0, ui->label_titleBar->width(), ui->label_titleBar->height());
    linearGradient.setColorAt(0, QColor(0x54,0x74,0xEB));
    linearGradient.setColorAt(1.0, QColor(0x00,0xD2,0xFF));
    QBrush brush(linearGradient);
    QPalette pa;
    pa.setBrush(QPalette::Window, brush);
    ui->label_titleBar->setAutoFillBackground(true);
    ui->label_titleBar->setPalette(pa);

    ui->label_logo->setPixmap(QPixmap(":/notifyWidget/logo.png").scaled(ui->label_logo->size()));

    QFont font("Microsoft YaHei UI",10,63);
    ui->label_lname->setFont(font);
    QPalette lpa;
    lpa.setColor(QPalette::WindowText,Qt::white);
    ui->label_lname->setPalette(lpa);

    setStyleSheet("QTextBrowser{color:blue;background:transparent;border:none;padding:10px;font-size:11pt;font-family:Microsoft YaHei UI Light;}"
                  );
}
