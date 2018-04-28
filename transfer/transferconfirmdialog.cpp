#include "transferconfirmdialog.h"
#include "ui_transferconfirmdialog.h"

#include "wallet.h"
#include "commondialog.h"

#include <QDebug>


TransferConfirmDialog::TransferConfirmDialog(QString address, QString amount, QString fee, QString remark, QString assetSymbol, QWidget *parent) :
    QDialog(parent),
    address(address),
    amount(amount),
    fee(fee),
    remark(remark),
    yesOrNo(false),
    ui(new Ui::TransferConfirmDialog)
{
    
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
//    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
//    ui->containerWidget->setObjectName("containerwidget");
//    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet("QToolButton{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                             "QToolButton:hover{background-color:#00D2FF;}");
    ui->cancelBtn->setStyleSheet("QToolButton{background-color:#E5E5E5; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                                 "QToolButton:hover{background-color:#00D2FF;}");

    setStyleSheet("QLineEdit{color:blue;background:transparent;border-width:0;border-style:outset;lineedit-password-mask-delay: 1000;}");

    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);
    ui->closeBtn->setVisible(false);

    ui->containerWidget->installEventFilter(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


    ui->addressLabel->setText( address);
    ui->amountLabel->setText( "<body><B>" + amount + "</B> " + assetSymbol + "</body>");
    ui->feeLabel->setText( "<body>" + fee + " " + ASSET_NAME +"</body>");
    ui->remarkLabel->setText( remark.isEmpty()?tr("none"):remark);


    ui->okBtn->setText(tr("Ok"));
    ui->cancelBtn->setText(tr("Cancel"));

    ui->pwdLineEdit->setPlaceholderText( tr("Enter login password"));
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setFocus();


}

TransferConfirmDialog::~TransferConfirmDialog()
{
    delete ui;
}

bool TransferConfirmDialog::pop()
{
    ui->pwdLineEdit->grabKeyboard();

//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();

    return yesOrNo;
}

void TransferConfirmDialog::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-TransferConfirmDialog")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            yesOrNo = true;
            close();
        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }
}

void TransferConfirmDialog::on_okBtn_clicked()
{
    if( ui->pwdLineEdit->text().isEmpty())
    {
        ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Please enter the password!") + "</font></body>" );
        return;
    }

    UBChain::getInstance()->postRPC( "id-unlock-TransferConfirmDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));

}

void TransferConfirmDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
//    emit accepted();
}

void TransferConfirmDialog::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void TransferConfirmDialog::on_pwdLineEdit_returnPressed()
{
    on_okBtn_clicked();
}

void TransferConfirmDialog::on_pwdLineEdit_textEdited(const QString &arg1)
{
    ui->tipLabel2->setText("");
}


void TransferConfirmDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}

void TransferConfirmDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);


    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,255,255,100));//最后一位是设置透明属性（在0-255取值）
    painter.drawRect(rect());

    painter.drawPixmap(410,80,325,450,QPixmap(":/ui/wallet_ui/trade.png").scaled(325,450));

    QWidget::paintEvent(event);

}
