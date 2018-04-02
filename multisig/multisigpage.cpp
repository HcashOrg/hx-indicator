#include "multisigpage.h"
#include "ui_multisigpage.h"

#include <QPainter>

#include "wallet.h"
#include "commondialog.h"
#include "addmultisigdialog.h"
#include "showcontentdialog.h"
#include "choosemultisigoperationdialog.h"
#include "multisiginfodialog.h"
#include "createmultisigdialog.h"
#include "withdrawmultisigdialog.h"
#include "signmultisigdialog.h"

MultiSigPage::MultiSigPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiSigPage)
{
    ui->setupUi(this);

    ui->multiSigTableWidget->installEventFilter(this);
    ui->multiSigTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->multiSigTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->multiSigTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->multiSigTableWidget->setFrameShape(QFrame::NoFrame);
    ui->multiSigTableWidget->setMouseTracking(true);
    ui->multiSigTableWidget->setShowGrid(false);//隐藏表格线

    ui->multiSigTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->multiSigTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->multiSigTableWidget->horizontalHeader()->setVisible(true);
    ui->multiSigTableWidget->verticalHeader()->setVisible(false);
    ui->multiSigTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    ui->multiSigTableWidget->setColumnWidth(0,310);
    ui->multiSigTableWidget->setColumnWidth(1,160);
    ui->multiSigTableWidget->setColumnWidth(2,160);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    refresh();
}

MultiSigPage::~MultiSigPage()
{
    delete ui;
}

void MultiSigPage::refresh()
{
    UBChain::getInstance()->postRPC( "id_wallet_multisig_account_balance", toJsonFormat( "wallet_multisig_account_balance", QStringList() << ""
                                                   ));

}

void MultiSigPage::updateMultiSigList()
{
    QStringList keys = UBChain::getInstance()->multiSigInfoMap.keys();
    int size = keys.size();

    ui->multiSigTableWidget->setRowCount(0);
    ui->multiSigTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        QString key = keys.at(i);
        MultiSigInfo info = UBChain::getInstance()->multiSigInfoMap.value(key);

        ui->multiSigTableWidget->setRowHeight(i,37);

        ui->multiSigTableWidget->setItem(i,0,new QTableWidgetItem(key));
        ui->multiSigTableWidget->item(i,0)->setTextColor(QColor(192,196,212));

        ui->multiSigTableWidget->setItem(i,1,new QTableWidgetItem(getBigNumberString(info.balanceMap.value(0),ASSET_PRECISION) ));
        ui->multiSigTableWidget->item(i,1)->setTextColor(QColor(24,250,239));

        ui->multiSigTableWidget->setItem(i,2,new QTableWidgetItem(tr("manage")));
        ui->multiSigTableWidget->item(i,2)->setTextColor(QColor(192,196,212));
        QFont font = ui->multiSigTableWidget->font();
        font.setUnderline(true);
        ui->multiSigTableWidget->item(i,2)->setFont(font);


        for( int j = 0; j < 3; j++)
        {
            ui->multiSigTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

            if(i % 2)
            {
                ui->multiSigTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
            }
            else
            {
                ui->multiSigTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
            }
        }

    }
}

void MultiSigPage::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_wallet_multisig_account_balance") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

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
                            int size = resultArray.size();
                            for( int i = 0; i < size; i++)
                            {
                                if(!resultArray[i].isArray())   return;
                                MultiSigInfo info;

                                QJsonArray array1 = resultArray.at(i).toArray();
                                info.multiSigAddress = array1.at(0).toString();

                                if(!array1.at(1).isArray())     return;
                                QJsonArray array2 = array1.at(1).toArray();

                                int size2 = array2.size();
                                for(int j = 0; j < size2; j++)
                                {
                                    QJsonArray array3 = array2.at(j).toArray();
                                    int assetId = array3.at(0).toInt();
                                    unsigned long long balance = 0;
                                    if(array3.at(1).isString())
                                    {
                                        balance = array3.at(1).toString().toULongLong();
                                    }
                                    else
                                    {
                                        balance = QString::number(array3.at(1).toDouble(),'g',10).toULongLong();
                                    }

                                    info.balanceMap.insert(assetId,balance);
                                }

                                UBChain::getInstance()->postRPC( "id_wallet_import_multisig_account+" + info.multiSigAddress,
                                                                 toJsonFormat( "wallet_import_multisig_account",
                                                                               QStringList() << info.multiSigAddress
                                                                           ));


                                UBChain::getInstance()->multiSigInfoMap.insert(info.multiSigAddress,info);
                            }

                        }
                    }
                }
            }


            updateMultiSigList();
        }
        return;
    }

    if( id.startsWith( "id_wallet_import_multisig_account+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

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
                        if( resultValue.isObject())
                        {
                            QJsonObject resultObject = resultValue.toObject();
                            bool res = resultObject.take("result").toBool();

                            if( res)
                            {
                                QString address = id.mid(QString("id_wallet_import_multisig_account+").size());

                                QJsonArray array = resultObject.take("owners").toArray();
                                QStringList owners;
                                for(int i = 0; i < array.size(); i++)
                                {
                                    owners << array.at(i).toString();
                                }

                                UBChain::getInstance()->multiSigInfoMap[address].multiSigAddress = address;
                                UBChain::getInstance()->multiSigInfoMap[address].requires = resultObject.take("requires").toInt();
                                UBChain::getInstance()->multiSigInfoMap[address].owners = owners;

                            }
                        }
                    }
                }
            }

        }

        return;
    }

    if( id.startsWith("id_wallet_multisig_account_history+"))
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        QString multiSigAddress = id.mid(QString("id_wallet_multisig_account_history+").size());
        UBChain::getInstance()->parseMultiSigTransactions(result, multiSigAddress);


        return;
    }
}

void MultiSigPage::on_importMultiSigBtn_clicked()
{
    AddMultiSigDialog addMultiSigDialog;
    addMultiSigDialog.pop();
}

void MultiSigPage::on_multiSigTableWidget_cellPressed(int row, int column)
{
    if( column == 0 )
    {
        ShowContentDialog showContentDialog( ui->multiSigTableWidget->item(row, column)->text(),this);

        int x = ui->multiSigTableWidget->columnViewportPosition(column) + ui->multiSigTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->multiSigTableWidget->rowViewportPosition(row) - 10 + ui->multiSigTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->multiSigTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if( column == 2 )
    {
        ChooseMultiSigOperationDialog chooseMultiSigOperationDialog(this);

        int x = ui->multiSigTableWidget->columnViewportPosition(column) + ui->multiSigTableWidget->columnWidth(column) / 2;
        int y = ui->multiSigTableWidget->rowViewportPosition(row) + 30 + ui->multiSigTableWidget->horizontalHeader()->height();

        chooseMultiSigOperationDialog.move(ui->multiSigTableWidget->mapToGlobal(QPoint(x,y)));

        int choice = chooseMultiSigOperationDialog.pop();

        if(ui->multiSigTableWidget->item(row,0) == NULL)   return;
        QString address = ui->multiSigTableWidget->item(row,0)->text();

        switch (choice)
        {
        case 0:
            break;
        case 1:
        {
            MultiSigInfoDialog multiSigInfoDialog(address);
            multiSigInfoDialog.pop();
        }
            break;
        case 2:
            emit goToTransferPage(address,"");
            break;
        case 3:
        {
            WithdrawMultiSigDialog withdrawMultiSigDialog(address);
            withdrawMultiSigDialog.pop();
        }
            break;
        case 4:
            emit showMultiSigTransactionPage(address);
            break;
        default:
            break;
        }

        return;
    }
}

void MultiSigPage::on_createMultiSigBtn_clicked()
{
    CreateMultiSigDialog createMultiSigDialog;
    createMultiSigDialog.pop();

    if(checkAddress(createMultiSigDialog.multiSigAddress,MultiSigAddress))
    {
        if(UBChain::getInstance()->multiSigInfoMap.contains(createMultiSigDialog.multiSigAddress))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("This multisig address already exists!"));
            commonDialog.pop();
        }
        else
        {
            refresh();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("%1 has been created!").arg(createMultiSigDialog.multiSigAddress));
            commonDialog.pop();
        }
    }
}

void MultiSigPage::on_signMultiSigBtn_clicked()
{
    SignMultiSigDialog signMultiSigDialog;
    signMultiSigDialog.pop();
}

void MultiSigPage::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(0,0,680,482);
}
