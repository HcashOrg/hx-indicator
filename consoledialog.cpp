#include "consoledialog.h"
#include "ui_consoledialog.h"
#include "wallet.h"


#include <QKeyEvent>
#include <QDesktopWidget>

ConsoleDialog::ConsoleDialog(QWidget *parent) :
    QDialog(parent),
    cmdIndex(0),
    ui(new Ui::ConsoleDialog)
{
    
    ui->setupUi(this);

//    UBChain::getInstance()->appendCurrentDialogVector(this);
//    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    QPalette pal = palette();
     pal.setColor(QPalette::Background, Qt::transparent);
     setPalette(pal);
     this->setAutoFillBackground(true);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(251, 251, 254,100);border-radius:10px;}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color:rgb(0,210,255);border-radius:10px;}");

    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    setStyleSheet("QToolButton#clearBtn{background-color:#5474EB; border:none;border-radius:5px;color: rgb(255, 255, 255);}"
                  "QToolButton#clearBtn:hover{background-color:#00D2FF;}"
                  "QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
                  "QLineEdit:focus{border:none;}"
                  );

    ui->containerWidget->installEventFilter(this);

    ui->consoleLineEdit->installEventFilter(this);


    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


//    ui->consoleLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
//    ui->consoleLineEdit->setTextMargins(8,0,0,0);
//    ui->consoleBrowser->setStyleSheet("QTextBrowser{color:black;border:1px solid #CCCCCC;border-radius:3px;}");

    ui->consoleLineEdit->setFocus();


//    setStyleSheet("#ConsoleDialog{background-color: rgb(246, 246, 246);}");

//    mouse_press = false;

    ui->checkBox->hide();  // rpc选项隐藏
    
}

ConsoleDialog::~ConsoleDialog()
{
    
    qDebug() << "ConsoleDialog delete";
//    UBChain::getInstance()->currentDialog = NULL;
//    UBChain::getInstance()->removeCurrentDialogVector(this);
    delete ui;
    
}

void ConsoleDialog::pop()
{
//    QEventLoop loop;
//    show();
//    ui->consoleLineEdit->grabKeyboard();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

//    move(0,0);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);
    exec();
}



bool ConsoleDialog::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {
//            QPainter painter(ui->containerWidget);
//            painter.setPen(Qt::NoPen);

//            painter.setBrush(QBrush(QColor(251,251,254),Qt::SolidPattern));
//            painter.drawRoundedRect(0,20,600,400,10,10);

//            painter.setBrush(QBrush(QColor(84,116,235),Qt::SolidPattern));
//            painter.drawRoundedRect(0,0,600,37,10,10);
//            painter.drawRect(0,20,600,17);

            return true;
        }
    }

    if(watched == ui->consoleLineEdit)
    {
        if(e->type() == QEvent::KeyPress)
        {
            QKeyEvent* event = static_cast<QKeyEvent*>(e);
            if( event->key() == Qt::Key_Up)
            {
                cmdIndex--;
                if( cmdIndex >= 0 && cmdIndex <= cmdVector.size() - 1)
                {
                    ui->consoleLineEdit->setText(cmdVector.at(cmdIndex));
                }

                if( cmdIndex < 0)
                {
                    cmdIndex = 0;
                }

            }
            else if( event->key() == Qt::Key_Down)
            {
                cmdIndex++;
                if( cmdIndex >= 0 && cmdIndex <= cmdVector.size() - 1)
                {
                    ui->consoleLineEdit->setText(cmdVector.at(cmdIndex));
                }

                if( cmdIndex > cmdVector.size() - 1)
                {
                    cmdIndex = cmdVector.size() - 1;
                }

            }
        }

    }

    return QWidget::eventFilter(watched,e);
}

void ConsoleDialog::on_closeBtn_clicked()
{
    close();
//    emit accepted();
}


void ConsoleDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();;
     }
}

void ConsoleDialog::mouseMoveEvent(QMouseEvent *event)
{
    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }
}

void ConsoleDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void ConsoleDialog::on_consoleLineEdit_returnPressed()
{
    

    if( !ui->consoleLineEdit->text().simplified().isEmpty())
    {
        cmdVector.removeAll(ui->consoleLineEdit->text());
        cmdVector.append(ui->consoleLineEdit->text());
        cmdIndex = cmdVector.size();
    }

    QString str = ui->consoleLineEdit->text();
    str = str.simplified();
    QStringList paramaters = str.split(' ');
    QString command = paramaters.at(0);
    paramaters.removeFirst();

    QJsonArray array;
    foreach (QString param, paramaters)
    {
        if(param.startsWith("[") && param.endsWith("]"))
        {
            array << QJsonDocument::fromJson(param.toLatin1()).array();
        }
        else
        {
            array << param;
        }
    }

    UBChain::getInstance()->postRPC( "console-" + str, toJsonFormat( command, array ));

    ui->consoleLineEdit->clear();

    return;
}

//void ConsoleDialog::setVisible(bool visiable)
//{
//    QWidget::setVisible(visiable);
//}

void ConsoleDialog::jsonDataUpdated(QString id)
{

    if( id.startsWith("console-"))
    {
        ui->consoleBrowser->append(">>>" + id.mid(8));
        ui->consoleBrowser->append( UBChain::getInstance()->jsonDataValue(id));
        ui->consoleBrowser->append("\n");
        return;
    }
}

void ConsoleDialog::on_clearBtn_clicked()
{
    ui->consoleBrowser->clear();
}
