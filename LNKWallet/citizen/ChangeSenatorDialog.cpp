#include "ChangeSenatorDialog.h"
#include "ui_ChangeSenatorDialog.h"

#include <limits>
#include <functional>
#include "wallet.h"
#include "FeeChooseWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"
#include "commondialog.h"
#include "capitalTransferPage/PasswordConfirmWidget.h"

//Q_DECLARE_METATYPE(AccountInfo)

ChangeSenatorDialog::ChangeSenatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeSenatorDialog)
{
    ui->setupUi(this);
    InitWidget();
}

ChangeSenatorDialog::~ChangeSenatorDialog()
{
    delete ui;
}

void ChangeSenatorDialog::ChangeSenatorSlots()
{
    if(ui->account->currentText().isEmpty() || ui->lineEdit_feeNumber->text().isEmpty()) return;

    QMap<QString,QString> replaceMap = ui->replaceWidget->getReplacePair();
    if(replaceMap.isEmpty()) return;

    if(ui->lineEdit_feeNumber->text().toDouble() < 1000)
    {
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("pledge amount shouldn't less than 1000!"));
        dia.pop();
        return;
    }

    QString proposingAccount = ui->account->currentText();
    QString feeNumber = ui->lineEdit_feeNumber->text();
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
    connect(wi,&PasswordConfirmWidget::confirmSignal,[this,proposingAccount,feeNumber,replaceArr](){
        if(FeeChooseWidget* feeWidget = dynamic_cast<FeeChooseWidget*>(this->ui->stackedWidget_fee->currentWidget()))
        {
            feeWidget->updatePoundageID();
        }
        HXChain::getInstance()->postRPC("id_change_senator",toJsonFormat("citizen_referendum_for_senator",
                                        QJsonArray()<<proposingAccount<<feeNumber<<replaceArr<<true));

    });
    wi->show();
}

void ChangeSenatorDialog::jsonDataUpdated(QString id)
{
    if("id_change_senator" == id)
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

void ChangeSenatorDialog::InitWidget()
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


    installDoubleValidator(ui->lineEdit_feeNumber,0,(std::numeric_limits<double>::max)(),ASSET_PRECISION);
    InitData();


    connect(ui->closeBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&ChangeSenatorDialog::ChangeSenatorSlots);
    connect(HXChain::getInstance(),&HXChain::jsonDataUpdated,this,&ChangeSenatorDialog::jsonDataUpdated);


    FeeChooseWidget *feeWidget = new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.ChangeSenatorFee.toDouble(),
                                                     HXChain::getInstance()->feeType,ui->account->currentText());
    connect(feeWidget,&FeeChooseWidget::feeSufficient,ui->okBtn,&QToolButton::setEnabled);
    QTimer::singleShot(100,[this,feeWidget](){this->ui->okBtn->setEnabled(feeWidget->isSufficient() && !ui->account->currentText().isEmpty());});
    connect(ui->lineEdit_feeNumber,&QLineEdit::textChanged,[feeWidget](const QString &number){
        feeWidget->updateFeeNumberSlots(number.toDouble());
    });
    connect(ui->account,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
            std::bind(&FeeChooseWidget::updateAccountNameSlots,feeWidget,std::placeholders::_1,true));
    ui->stackedWidget_fee->addWidget(feeWidget);
    ui->stackedWidget_fee->setCurrentWidget(feeWidget);
    ui->stackedWidget_fee->currentWidget()->resize(ui->stackedWidget_fee->size());

}

void ChangeSenatorDialog::InitData()
{
    //初始化账户combobox
    QStringList accounts = HXChain::getInstance()->getMyCitizens();
    ui->account->addItems(accounts);

    ui->replaceWidget->InitData(false);
//    QMap<QString,AccountInfo> account(HXChain::getInstance()->accountInfoMap);
//    foreach (AccountInfo info, account) {
//        ui->account->addItem(info.name,QVariant::fromValue<AccountInfo>(info));
    //    }
}

void ChangeSenatorDialog::installDoubleValidator(QLineEdit *line, double mi, double ma, int pre)
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
