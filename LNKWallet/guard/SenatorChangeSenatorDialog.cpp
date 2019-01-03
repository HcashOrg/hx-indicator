#include "SenatorChangeSenatorDialog.h"
#include "ui_SenatorChangeSenatorDialog.h"

#include <limits>
#include <functional>
#include "wallet.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "capitalTransferPage/PasswordConfirmWidget.h"

SenatorChangeSenatorDialog::SenatorChangeSenatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SenatorChangeSenatorDialog)
{
    ui->setupUi(this);
    InitWidget();
}

SenatorChangeSenatorDialog::~SenatorChangeSenatorDialog()
{
    delete ui;
}

void SenatorChangeSenatorDialog::ChangeSenatorSlots()
{
    if(ui->account->currentText().isEmpty()) return;

    QMap<QString,QString> replaceMap = ui->replaceWidget->getReplacePair();
    if(replaceMap.isEmpty()) return;

    QString proposingAccount = ui->account->currentText();
    int expiration_time = static_cast<int>(ui->lineEdit_expTime->text().toDouble()*3600);
    //发起更换提案
    QJsonArray replaceArr;
    QMapIterator<QString, QString> i(replaceMap);
    while (i.hasNext()) {
        i.next();
        QJsonArray temp;
        temp<<i.key()<<i.value();
        replaceArr<<temp;
    }
    //输入密码框
    PasswordConfirmWidget *wi = new PasswordConfirmWidget(HXChain::getInstance()->mainFrame);
    connect(wi,&PasswordConfirmWidget::confirmSignal,[this,proposingAccount,expiration_time,replaceArr](){
        HXChain::getInstance()->postRPC("id_senator_change_senator",toJsonFormat("update_senator_formal",
                                        QJsonArray()<<proposingAccount<<replaceArr<<expiration_time<<true));

    });
    wi->show();
}

void SenatorChangeSenatorDialog::jsonDataUpdated(QString id)
{
    if("id_senator_change_senator" == id)
    {
        close();
        QString result = HXChain::getInstance()->jsonDataValue(id);
        if( result.startsWith("\"result\":{"))             // 成功
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of changing senators has been sent,please wait for confirmation"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Failed!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }
    }
}

void SenatorChangeSenatorDialog::InitWidget()
{
    ui->label_3->setVisible(false);
    setParent(HXChain::getInstance()->mainFrame);
    move(0,0);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    installDoubleValidator(ui->lineEdit_expTime,0,720.0,1);

    InitData();
    if(ui->account->currentText().isEmpty())
    {
        ui->okBtn->setEnabled(false);
    }

    connect(ui->closeBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&SenatorChangeSenatorDialog::ChangeSenatorSlots);
    connect(HXChain::getInstance(),&HXChain::jsonDataUpdated,this,&SenatorChangeSenatorDialog::jsonDataUpdated);

}

void SenatorChangeSenatorDialog::InitData()
{
    //初始化账户combobox
    QStringList accounts = HXChain::getInstance()->getMyGuards();
    foreach (QString acc, accounts) {

        GuardInfo info = HXChain::getInstance()->allGuardMap.value(acc);
        if("PERMANENT" == info.senatorType)
        {
            ui->account->addItem(acc);
        }
    }
    ui->replaceWidget->InitData(true);
}

void SenatorChangeSenatorDialog::installDoubleValidator(QLineEdit *line, double mi, double ma, int pre)
{
    QDoubleValidator *validator = new QDoubleValidator(mi,ma,pre);
    validator->setNotation(QDoubleValidator::StandardNotation);
    line->setValidator( validator );
    connect(line,&QLineEdit::textChanged,[line](){
        //修正数值
        QDoubleValidator* via = dynamic_cast<QDoubleValidator*>(const_cast<QValidator*>(line->validator()));
        if(!via)
        {
            return;
        }
        if(line->text().toDouble() > via->top())
        {
            line->setText(line->text().remove(line->text().length()-1,1));
            return;
        }
    });
}
