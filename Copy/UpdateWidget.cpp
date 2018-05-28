#include "UpdateWidget.h"
#include "ui_UpdateWidget.h"

#include <QDir>
#include <QTimer>
#include <QString>
#include <QProcess>

#include "DataUtil.h"

static const QString OKBTN_STYLE =  "QToolButton{font: 14px \"Microsoft YaHei UI Light\";background-color: rgb(84,116,235);border:0px solid white;border-radius:15px;color: white;}"  \
                        "QToolButton:hover{background-color:rgb(0,210,255);}" ;

UpdateWidget::UpdateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateWidget)
{
    ui->setupUi(this);
    InitWidget();
}

UpdateWidget::~UpdateWidget()
{
    delete ui;
}

void UpdateWidget::startMove()
{
    //复制临时文件到主目录
    DataUtil::copyDir(QCoreApplication::applicationDirPath()+QDir::separator()+"temp",QCoreApplication::applicationDirPath());
    //删除临时文件
    DataUtil::deleteDir(QCoreApplication::applicationDirPath()+QDir::separator()+"temp");

    //更新结束
    copyFinish();
}

void UpdateWidget::copyFinish()
{
    ui->update->setText(tr("update finish!"));

    restartWallet();
}

void UpdateWidget::restartWallet()
{
    //启动外部复制程序
    QProcess *copproc = new QProcess();
    copproc->start("LNKWallet.exe");
    close();
}

void UpdateWidget::InitWidget()
{
    InitStyle();
    ui->toolButton_close->setVisible(false);
    ui->toolButton_restart->setVisible(false);

    ui->update->setText(tr("updating,please wait!"));
    QTimer::singleShot(500,this,&UpdateWidget::startMove);

    connect(ui->toolButton_close,&QToolButton::clicked,this,&UpdateWidget::close);
    connect(ui->toolButton_restart,&QToolButton::clicked,this,&UpdateWidget::restartWallet);
}

void UpdateWidget::InitStyle()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush( QPalette::Window,
                     QBrush(QPixmap(":/resource/background.png").scaled(// 缩放背景图.
                         this->size(),
                         Qt::IgnoreAspectRatio,
                         Qt::SmoothTransformation)));
    setPalette(palette);

    //label背景
    ui->label->setPixmap(QPixmap(":/resource/login.png").scaled(ui->label->size(),
                                                                     Qt::IgnoreAspectRatio,
                                                                     Qt::SmoothTransformation));
    //label图标
    ui->label_logo->setPixmap(QPixmap(":/resource/welcomeLabel.png").scaled(ui->label_logo->size(),
                                                                     Qt::IgnoreAspectRatio,
                                                                     Qt::SmoothTransformation));

    ui->toolButton_close->setStyleSheet(OKBTN_STYLE);
    ui->toolButton_restart->setStyleSheet(OKBTN_STYLE);
}
