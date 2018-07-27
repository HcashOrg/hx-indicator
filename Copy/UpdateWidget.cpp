#include "UpdateWidget.h"
#include "ui_UpdateWidget.h"

#include <QDir>
#include <QTimer>
#include <QString>
#include <QProcess>
#include <QDebug>
#include <QPainter>

#include "DataUtil.h"

#define OKBTN_STYLE    "QToolButton{font: 11px \"微软雅黑\";background-color: rgb(84,61,137);border:0px solid white;border-radius:3px;color: white;}"  \
                        "QToolButton:pressed{background-color:rgb(84,61,137);}"\
                        "QToolButton:disabled{background-color:rgb(196,191,214);}"

#define CANCELBTN_STYLE "QToolButton{font: 11px \"微软雅黑\";background:rgb(235,0,94);color: rgb(255,255,255);border:0px solid rgb(84,116,235);border-radius:3px;}"  \
                        "QToolButton:pressed{background-color:rgb(235,0,94);color: white}"\
                        "QToolButton:disabled{background-color:rgb(196,191,214);}"

#define CLOSEBTN_STYLE  "QToolButton{background-image:url(:/ui/wallet_ui/close_button.png);background-repeat: no-repeat;background-position: center;background-color:transparent;border:none;}"   \
                        "QToolButton:hover{background-color:rgb(208,228,255);"

static const QString TEMP_FOLDER_NAME = "temp";
static const QString PACKAGE_UN = "blocklink";
static const QString PACKAGE_NAME = "blocklink.zip";
static const QString MAINEXE_NAME = "HXIndicator.exe";

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
    //解压文件
    QString tempdir = QCoreApplication::applicationDirPath()+QDir::separator()+TEMP_FOLDER_NAME;
    QString appdir = QCoreApplication::applicationDirPath();
    QString compressPath = tempdir +QDir::separator()+ PACKAGE_NAME;
    DataUtil::unCompress( compressPath,tempdir);
    //复制文件到temp目录
    DataUtil::copyDir(tempdir+QDir::separator()+PACKAGE_UN,QCoreApplication::applicationDirPath());
    //删除压缩文件、解压目录
    qDebug()<<compressPath;
    QFile::remove(compressPath);
    DataUtil::deleteDir(tempdir + QDir::separator() + PACKAGE_UN);

    //复制临时文件到主目录
    DataUtil::copyDir(QCoreApplication::applicationDirPath()+QDir::separator()+TEMP_FOLDER_NAME,QCoreApplication::applicationDirPath());
    //删除临时文件
    DataUtil::deleteDir(QCoreApplication::applicationDirPath()+QDir::separator()+TEMP_FOLDER_NAME);
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
    copproc->start(MAINEXE_NAME);
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
    palette.setBrush(QPalette::Window,  QBrush(QPixmap(":/resource/login_back.png").scaled(this->size(),
                                                                                               Qt::IgnoreAspectRatio,
                                                                                               Qt::SmoothTransformation)));
    setPalette(palette);
    
    ui->welcome->setPixmap(QPixmap(":/resource/Welcome.png").scaled(ui->welcome->width(), ui->welcome->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));


    ui->toolButton_close->setStyleSheet(CANCELBTN_STYLE);
    ui->toolButton_restart->setStyleSheet(OKBTN_STYLE);
}

void UpdateWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(243,241,250)));
    painter.drawRect(0,0,228,24);
    painter.drawPixmap(7,5,32,12,QPixmap(":/resource/hx_label_logo.png").scaled(32,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    painter.drawPixmap(94,38,36,36,QPixmap(":/resource/logo_center.png").scaled(36,36,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget::paintEvent(e);
}
