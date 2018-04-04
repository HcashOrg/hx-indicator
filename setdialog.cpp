
#include <QMouseEvent>
#include <QUrl>
#include <QDesktopServices>
#include <QListView>


#include "setdialog.h"
#include "ui_setdialog.h"
#include "wallet.h"

#include "commondialog.h"


#define SETDIALOG_GENERALBTN_SELECTED_STYLE     "QToolButton{background-color:rgb(70,82,113);color:white;border:1px solid rgb(70,82,113);border-top-left-radius: 12px;border-bottom-left-radius: 12px;}"
#define SETDIALOG_GENERALBTN_UNSELECTED_STYLE   "QToolButton{background:transparent;color:rgb(192,196,212);border:1px solid rgb(70,82,113);border-top-left-radius: 12px;border-bottom-left-radius: 12px;}"
#define SETDIALOG_SAVEBTN_SELECTED_STYLE        "QToolButton{background-color:rgb(70,82,113);color:white;border:1px solid rgb(70,82,113);border-top-right-radius: 12px;border-bottom-right-radius: 12px;}"
#define SETDIALOG_SAVEBTN_UNSELECTED_STYLE      "QToolButton{background-color:transparent;color:rgb(192,196,212);border:1px solid rgb(70,82,113);border-top-right-radius: 12px;border-bottom-right-radius: 12px;}"


SetDialog::SetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetDialog)
{
	

    ui->setupUi(this);

//    UBChain::getInstance()->appendCurrentDialogVector(this);
    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->saveBtn->setStyleSheet(OKBTN_STYLE);
    ui->confirmBtn->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->languageComboBox->setView(new QListView());


    ui->containerWidget->installEventFilter(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    on_generalBtn_clicked();

    ui->lockTimeSpinBox->setValue(QString::number(UBChain::getInstance()->lockMinutes).toInt());

    if(!UBChain::getInstance()->notProduce)
    {
        ui->nolockCheckBox->setChecked(true);
        ui->lockTimeSpinBox->setEnabled(false);
    }
    else
    {
        ui->nolockCheckBox->setChecked(false);
        ui->lockTimeSpinBox->setEnabled(true);
    }


    QString language = UBChain::getInstance()->language;
    if( language == "Simplified Chinese")
    {
        ui->languageComboBox->setCurrentIndex(0);
    }
    else if( language == "English")
    {
        ui->languageComboBox->setCurrentIndex(1);
    }
    else
    {
        ui->languageComboBox->setCurrentIndex(0);
    }

    ui->minimizeCheckBox->setChecked( UBChain::getInstance()->minimizeToTray);
    ui->closeCheckBox->setChecked( UBChain::getInstance()->closeToMinimize);
    ui->resyncCheckBox->setChecked( UBChain::getInstance()->resyncNextTime);

    ui->feeLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->feeLineEdit->setText( getBigNumberString(UBChain::getInstance()->transactionFee,ASSET_PRECISION));

    QRegExp rx(QString("^([0]|[1-9][0-9]{0,2})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(QString::number(ASSET_PRECISION).size() - 1));
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->feeLineEdit->setValidator(pReg);

    ui->confirmBtn->setEnabled(false);

    ui->oldPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->newPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->confirmPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);

    ui->oldPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->newPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->confirmPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->lockTimeSpinBox->setAttribute(Qt::WA_InputMethodEnabled, false);

}

SetDialog::~SetDialog()
{
    qDebug() << "setdialog delete";
//    UBChain::getInstance()->currentDialog = NULL;
    delete ui;
//    UBChain::getInstance()->removeCurrentDialogVector(this);
}

void SetDialog::pop()
{
//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}


bool SetDialog::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {
            QPainter painter(ui->containerWidget);
            painter.setPen(QPen(QColor(40,45,66),Qt::SolidLine));
            painter.setBrush(QBrush(QColor(40,45,66),Qt::SolidPattern));
            painter.drawRoundedRect(0,20,600,400,10,10);

            painter.setPen(QPen(QColor(32,36,54),Qt::SolidLine));
            painter.setBrush(QBrush(QColor(32,36,54),Qt::SolidPattern));
            painter.drawRoundedRect(0,0,600,37,10,10);
            painter.drawRect(0,20,600,17);

            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}

void SetDialog::on_closeBtn_clicked()
{
    close();
//    emit accepted();
}

void SetDialog::on_saveBtn_clicked()
{
	
    mutexForConfigFile.lock();
    UBChain::getInstance()->lockMinutes = ui->lockTimeSpinBox->value();
    UBChain::getInstance()->configFile->setValue("/settings/lockMinutes", UBChain::getInstance()->lockMinutes);
    if( ui->nolockCheckBox->isChecked())
    {
        UBChain::getInstance()->notProduce = false;
    }
    else
    {
        UBChain::getInstance()->notProduce = true;
    }
    UBChain::getInstance()->configFile->setValue("/settings/notAutoLock", !UBChain::getInstance()->notProduce);

    if( ui->languageComboBox->currentIndex() == 0)      // config 中保存语言设置
    {
        UBChain::getInstance()->configFile->setValue("/settings/language", "Simplified Chinese");
        UBChain::getInstance()->language = "Simplified Chinese";
    }
    else if( ui->languageComboBox->currentIndex() == 1)
    {
        UBChain::getInstance()->configFile->setValue("/settings/language", "English");
        UBChain::getInstance()->language = "English";
    }


    UBChain::getInstance()->minimizeToTray = ui->minimizeCheckBox->isChecked();
    UBChain::getInstance()->configFile->setValue("/settings/minimizeToTray", UBChain::getInstance()->minimizeToTray);

    UBChain::getInstance()->closeToMinimize = ui->closeCheckBox->isChecked();
    UBChain::getInstance()->configFile->setValue("/settings/closeToMinimize", UBChain::getInstance()->closeToMinimize);

    UBChain::getInstance()->resyncNextTime = ui->resyncCheckBox->isChecked();
    UBChain::getInstance()->configFile->setValue("/settings/resyncNextTime", UBChain::getInstance()->resyncNextTime);

    mutexForConfigFile.unlock();

    UBChain::getInstance()->postRPC( "id_wallet_set_transaction_fee", toJsonFormat( "wallet_set_transaction_fee", QStringList() << ui->feeLineEdit->text() ));

    emit settingSaved();

//    close();
//    emit accepted();

	
}


//void SetDialog::mousePressEvent(QMouseEvent *event)
//{
//    if(event->button() == Qt::LeftButton)
//     {
//          mouse_press = true;
//          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
//          move_point = event->pos();;
//     }
//}

//void SetDialog::mouseMoveEvent(QMouseEvent *event)
//{
//    //若鼠标左键被按下
//    if(mouse_press)
//    {
//        //鼠标相对于屏幕的位置
//        QPoint move_pos = event->globalPos();

//        //移动主窗体位置
//        this->move(move_pos - move_point);
//    }
//}

//void SetDialog::mouseReleaseEvent(QMouseEvent *)
//{
//    mouse_press = false;
//}

void SetDialog::on_nolockCheckBox_clicked()
{
    if(ui->nolockCheckBox->isChecked())
    {
        ui->lockTimeSpinBox->setEnabled(false);
    }
    else
    {
        ui->lockTimeSpinBox->setEnabled(true);
    }
}

void SetDialog::on_lockTimeSpinBox_valueChanged(const QString &arg1)
{
    if( arg1.startsWith('0') || arg1.isEmpty())
    {
        ui->lockTimeSpinBox->setValue(1);
    }
}

void SetDialog::on_generalBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->generalBtn->setStyleSheet(SETDIALOG_GENERALBTN_SELECTED_STYLE);
    ui->safeBtn->setStyleSheet(SETDIALOG_SAVEBTN_UNSELECTED_STYLE);
}

void SetDialog::on_safeBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->generalBtn->setStyleSheet(SETDIALOG_GENERALBTN_UNSELECTED_STYLE);
    ui->safeBtn->setStyleSheet(SETDIALOG_SAVEBTN_SELECTED_STYLE);
}

void SetDialog::on_confirmBtn_clicked()
{
    ui->tipLabel3->clear();

    UBChain::getInstance()->postRPC( "id_wallet_check_passphrase", toJsonFormat( "wallet_check_passphrase", QStringList() << ui->oldPwdLineEdit->text() ));

}

void SetDialog::on_newPwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->newPwdLineEdit->setText( ui->newPwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->tipLabel->setText("");
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->newPwdLineEdit->text().length() < 8)
    {
//        ui->tipLabel->setText(QString::fromLocal8Bit("密码最少为8位"));
        ui->tipLabel->setText(tr("at least 8 letters"));
        ui->confirmBtn->setEnabled(false);

        return;
    }
    else
    {
        ui->tipLabel->clear();
    }

    if( ui->confirmPwdLineEdit->text().isEmpty())
    {
        ui->tipLabel2->clear();
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->newPwdLineEdit->text() == ui->confirmPwdLineEdit->text())
    {
        ui->tipLabel2->clear();
        ui->confirmBtn->setEnabled(true);

    }
    else
    {
//        ui->tipLabel2->setText(QString::fromLocal8Bit("两次密码不一致"));
        ui->tipLabel2->setText(tr("not consistent"));
        ui->confirmBtn->setEnabled(false);

    }
}

void SetDialog::on_confirmPwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->confirmPwdLineEdit->setText( ui->confirmPwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->tipLabel2->setText("");
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->confirmPwdLineEdit->text().length() < 8)
    {
//        ui->tipLabel2->setText(QString::fromLocal8Bit("两次密码不一致"));
        ui->tipLabel2->setText(tr("not consistent"));
        ui->tipLabel2->setText(tr("at least 8 letters"));
        ui->confirmBtn->setEnabled(false);

        return;
    }
    else
    {
        ui->tipLabel2->clear();
    }


    if( ui->newPwdLineEdit->text() == ui->confirmPwdLineEdit->text())
    {
        ui->confirmBtn->setEnabled(true);

    }
    else
    {
//        ui->tipLabel2->setText(QString::fromLocal8Bit("两次密码不一致"));
        ui->tipLabel2->setText(tr("not consistent"));
        ui->confirmBtn->setEnabled(false);

    }
}


void SetDialog::on_oldPwdLineEdit_textChanged(const QString &arg1)
{
    ui->tipLabel3->clear();

    if( arg1.indexOf(' ') > -1)
    {
        ui->oldPwdLineEdit->setText( ui->oldPwdLineEdit->text().remove( ' '));
    }
}

void SetDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_check_passphrase")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            UBChain::getInstance()->postRPC( "id_wallet_change_passphrase", toJsonFormat( "wallet_change_passphrase", QStringList() << ui->oldPwdLineEdit->text() <<  ui->newPwdLineEdit->text() ));
        }
        else
        {
            ui->tipLabel3->setText( tr("wrong password"));
        }

        return;
    }

    if( id == "id_wallet_change_passphrase")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            close();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Failed: " + errorMessage);
            commonDialog.pop();
        }

        return;
    }

    if( id == "id_wallet_set_transaction_fee")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            UBChain::getInstance()->postRPC( "id_wallet_get_transaction_fee", toJsonFormat( "wallet_get_transaction_fee", QStringList() << "" ));

            close();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            if( errorMessage == "invalid fee")
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
//                commonDialog.setText( QString::fromLocal8Bit("手续费不能为0!"));
                commonDialog.setText( tr("The fee should not be 0!"));
                commonDialog.pop();
            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( "Failed: " + errorMessage);
                commonDialog.pop();
            }
        }

        return;
    }
}


//void SetDialog::setVisible(bool visiable)
//{
//    // 这样调用的是qwidget的setvisible 而不是qdialog的setvisible
//    // 即使调用hide 也不会结束exec(delete)
////    qDebug() << "setvisible " << visiable;
//    QWidget::setVisible(visiable);
//}
