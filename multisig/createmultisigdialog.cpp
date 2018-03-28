#include "createmultisigdialog.h"
#include "ui_createmultisigdialog.h"

#include <QDebug>

#include "lnk.h"
#include "../commondialog.h"
#include "showcontentdialog.h"
#include "addownerdialog.h"

CreateMultiSigDialog::CreateMultiSigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateMultiSigDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));


    ui->createBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);


    ui->requiresLineEdit->setText("1");
    QIntValidator *validator = new QIntValidator(1,9999,this);
    ui->requiresLineEdit->setValidator( validator );
    ui->requiresLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->ownersTableWidget->installEventFilter(this);
    ui->ownersTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->ownersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ownersTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->ownersTableWidget->setFrameShape(QFrame::NoFrame);
    ui->ownersTableWidget->setMouseTracking(true);
    ui->ownersTableWidget->setShowGrid(false);//隐藏表格线

    ui->ownersTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->ownersTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->ownersTableWidget->horizontalHeader()->setVisible(true);
    ui->ownersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    ui->ownersTableWidget->setColumnWidth(0,300);
    ui->ownersTableWidget->setColumnWidth(1,100);
    ui->ownersTableWidget->setColumnWidth(2,80);

    QStringList keys = UBChain::getInstance()->addressMap.keys();
    ui->accountComboBox->addItems( keys);
}

CreateMultiSigDialog::~CreateMultiSigDialog()
{
    delete ui;
}

void CreateMultiSigDialog::pop()
{
    move(0,0);
    exec();
}

void CreateMultiSigDialog::updateOwnersList()
{
    int size = owners.size();
    ui->ownersTableWidget->setRowCount(0);
    ui->ownersTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        QString address = owners.at(i);

        ui->ownersTableWidget->setRowHeight(i,35);

        ui->ownersTableWidget->setItem(i,0,new QTableWidgetItem(address));
        ui->ownersTableWidget->item(i,0)->setTextColor(QColor(192,196,212));

        if(UBChain::getInstance()->isMyAddress(address))
        {
            QString accountName = UBChain::getInstance()->addressToName(address);
            ui->ownersTableWidget->setItem(i,1,new QTableWidgetItem(accountName));
            ui->ownersTableWidget->item(i,1)->setTextColor(QColor(43,230,131));
        }
        else
        {
            ui->ownersTableWidget->setItem(i,1,new QTableWidgetItem(tr("non-local")));
            ui->ownersTableWidget->item(i,1)->setTextColor(QColor(255,34,76));
        }

        ui->ownersTableWidget->setItem(i,2,new QTableWidgetItem(tr("delete")));
        ui->ownersTableWidget->item(i,2)->setTextColor(QColor(255,34,76));
        QFont font = ui->ownersTableWidget->font();
        font.setUnderline(true);
        ui->ownersTableWidget->item(i,2)->setFont(font);


        for(int j = 0; j < 3; j++)
        {
            ui->ownersTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

            if(i % 2)
            {
                ui->ownersTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
            }
            else
            {
                ui->ownersTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
            }

        }
    }

}

void CreateMultiSigDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_wallet_create_multisig_account") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
qDebug()  << id << result;
        if( result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
            QByteArray ba = utfCodec->fromUnicode(result);


            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {

                    QJsonObject jsonObject = parse_doucment.object();
                    if( jsonObject.contains("result"))
                    {
                        QJsonValue resultValue = jsonObject.take("result");
                        if( resultValue.isArray())
                        {
                            QJsonArray resultArray = resultValue.toArray();
                            multiSigAddress = resultArray.at(0).toString();

                            close();
                        }
                    }
                }
            }
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Fail to create multisig address: ") + errorMessage);
            commonDialog.pop();
        }



        return;
    }
}

void CreateMultiSigDialog::on_createBtn_clicked()
{
    if(ui->accountComboBox->currentText().isEmpty())    return;

    int requires = ui->requiresLineEdit->text().toInt();
    if(requires < 1)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("The number of required confirmation should not be 0!"));
        commonDialog.pop();

        return;
    }

    if(owners.size() < 1)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("No owners to be added!"));
        commonDialog.pop();

        return;
    }

    if(owners.size() < requires)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("The number of required confirmation should not be less than the number of owners!"));
        commonDialog.pop();

        return;
    }

    QString str = "[";
    foreach (QString owner, owners)
    {
        str += "\"" + owner + "\",";
    }
    str += "]";
    str.replace(",]","]");

    UBChain::getInstance()->postRPC( "id_wallet_create_multisig_account",
                                     toJsonFormat( "wallet_create_multisig_account",
                                                   QStringList() << getBigNumberString(1,ASSET_PRECISION) << ASSET_NAME
                                                   << ui->accountComboBox->currentText() << QString::number(requires)
                                                   << str << ""
                                                   ));


}

void CreateMultiSigDialog::on_cancelBtn_clicked()
{
    close();
}

void CreateMultiSigDialog::on_addBtn_clicked()
{
    AddOwnerDialog addOwnerDialog;
    addOwnerDialog.pop();

    if(addOwnerDialog.ownerAddress.isEmpty())   return;

    if(owners.contains(addOwnerDialog.ownerAddress))
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("This owner already exists!"));
        commonDialog.pop();
    }
    else
    {
        owners.append(addOwnerDialog.ownerAddress);
        updateOwnersList();
    }
}

void CreateMultiSigDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->balanceLabel->setText( getBigNumberString(UBChain::getInstance()->accountBalanceMap.value(arg1).value(0),ASSET_PRECISION) + " UB");
}

void CreateMultiSigDialog::on_ownersTableWidget_cellPressed(int row, int column)
{
    if( column == 0 )
    {
        ShowContentDialog showContentDialog( ui->ownersTableWidget->item(row, column)->text(),this);

        int x = ui->ownersTableWidget->columnViewportPosition(column) + ui->ownersTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->ownersTableWidget->rowViewportPosition(row) - 10 + ui->ownersTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->ownersTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if( column == 2 )
    {
        if(ui->ownersTableWidget->item(row, 0) == NULL) return;
        QString address = ui->ownersTableWidget->item(row, 0)->text();
        owners.removeAll(address);
        updateOwnersList();

        return;
    }
}

void CreateMultiSigDialog::on_closeBtn_clicked()
{
    close();
}
