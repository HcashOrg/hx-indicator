#include "CitizenResolutionDialog.h"
#include "ui_CitizenResolutionDialog.h"

#include <QRadioButton>
#include "wallet.h"
#include "FeeChooseWidget.h"
#include "dialog/TransactionResultDialog.h"
#include "dialog/ErrorResultDialog.h"

CitizenResolutionDialog::CitizenResolutionDialog(const ResolutionInfo& _info, QString _account, QWidget *parent) :
    QDialog(parent),
    info(_info),
    account(_account),
    ui(new Ui::CitizenResolutionDialog)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setParent(HXChain::getInstance()->mainFrame);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    bgGroup = new QButtonGroup(this);

    // 根据选项数量生成qradiobutton
    foreach (int index, info.optionsMap.keys())
    {
        QRadioButton* rb = new QRadioButton(info.optionsMap.value(index), ui->scrollAreaWidgetContents);
        bgGroup->addButton( rb);
        bgGroup->setId(rb, index);

        rb->setGeometry(150,150 + 30 * index, 310, 20);
        rb->setChecked(!index); // 选中第0个
    }

    ui->stackedWidget_fee->move(ui->stackedWidget_fee->x(), 150 + 30 * info.optionsMap.keys().size());
    ui->scrollAreaWidgetContents->setMinimumHeight(ui->stackedWidget_fee->y() + ui->stackedWidget_fee->height() + 10);

//    connect( bgGroup, SIGNAL(buttonToggled(int, bool), this, SLOT(on_bgGroup_toggled(int, bool)));

    if(account.isEmpty())
    {
        ui->accountLabel->setText(tr("no citizen account"));
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->accountLabel->setText(account);
    }

    ui->titlePlainTextEdit->setReadOnly(true);
    ui->titlePlainTextEdit->setPlainText(info.title);


    FeeChooseWidget *feeWidget = new FeeChooseWidget(HXChain::getInstance()->feeChargeInfo.citizenResolution.toDouble(),
                                                     HXChain::getInstance()->feeType,account);
    connect(feeWidget,&FeeChooseWidget::feeSufficient,ui->okBtn,&QToolButton::setEnabled);
    QTimer::singleShot(100,[this,feeWidget](){this->ui->okBtn->setEnabled(feeWidget->isSufficient());});
    ui->stackedWidget_fee->addWidget(feeWidget);
    ui->stackedWidget_fee->setCurrentWidget(feeWidget);
    ui->stackedWidget_fee->currentWidget()->resize(ui->stackedWidget_fee->size());
}

CitizenResolutionDialog::~CitizenResolutionDialog()
{
    delete ui;
}

void CitizenResolutionDialog::pop()
{
    move(0,0);
    exec();
}

void CitizenResolutionDialog::jsonDataUpdated(QString id)
{
    if( id == "CitizenResolutionDialog-cast_vote")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":{"))             // 成功
        {
            close();
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of citizen-voting has been sent,please wait for confirmation"));
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

        return;
    }
}

void CitizenResolutionDialog::on_okBtn_clicked()
{
    if(account.isEmpty())   return;
    HXChain::getInstance()->postRPC("CitizenResolutionDialog-cast_vote",toJsonFormat("cast_vote",
                                    QJsonArray() << account << info.id << bgGroup->checkedId() << true));


}

void CitizenResolutionDialog::on_cancelBtn_clicked()
{
    close();
}

void CitizenResolutionDialog::on_closeBtn_clicked()
{
    close();
}

void CitizenResolutionDialog::on_bgGroup_toggled(int id, bool status)
{
qDebug() << id << status;
}
