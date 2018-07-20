#include "signmultisigdialog.h"
#include "ui_signmultisigdialog.h"

#include <QClipboard>

#include "wallet.h"
#include "../commondialog.h"
#include "multisiginfodialog.h"
#include "showcontentdialog.h"
#include "dialog/checkpwddialog.h"

SignMultiSigDialog::SignMultiSigDialog(QWidget *parent) :
    QDialog(parent),
    useFile(true),
    localUnsignedCount(0),
    ui(new Ui::SignMultiSigDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->nextBtn->setStyleSheet(OKBTN_STYLE);
    ui->signBtn->setStyleSheet(OKBTN_STYLE);
    ui->copyBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn2->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn3->setStyleSheet(CANCELBTN_STYLE);
    ui->backBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

    ui->signInfoTableWidget->installEventFilter(this);
    ui->signInfoTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->signInfoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->signInfoTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->signInfoTableWidget->setFrameShape(QFrame::NoFrame);
    ui->signInfoTableWidget->setMouseTracking(true);
    ui->signInfoTableWidget->setShowGrid(false);//隐藏表格线

    ui->signInfoTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->signInfoTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->signInfoTableWidget->horizontalHeader()->setVisible(true);
    ui->signInfoTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->signInfoTableWidget->setColumnWidth(0,300);
    ui->signInfoTableWidget->setColumnWidth(1,100);
    ui->signInfoTableWidget->setColumnWidth(2,100);

    init();
}

SignMultiSigDialog::~SignMultiSigDialog()
{
    delete ui;
}

void SignMultiSigDialog::pop()
{
    move(0,0);
    exec();
}

void SignMultiSigDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith("id_wallet_builder_get_multisig_detail") || id.startsWith("id_wallet_builder_file_get_multisig_detail")  )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
qDebug() << id << result;
        if( result.startsWith("\"result\":") )             // 成功
        {
            localUnsignedCount = 0;

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
                            QJsonValue fromAddressValue = resultObject.take("from_address");
                            if(fromAddressValue.isNull())
                            {
                                CommonDialog commonDialog(CommonDialog::OkOnly);
                                commonDialog.setText(tr("Wrong transaction builder!"));
                                commonDialog.pop();

                                return;
                            }
                            else
                            {
                                ui->stackedWidget->setCurrentIndex(1);
                            }

                            QString fromAddress = fromAddressValue.toString();
                            QString toAddress = resultObject.take("to_address").toString();
                            int required = resultObject.take("required").toInt();

                            QStringList addresses;
                            QJsonArray array = resultObject.take("addresses").toArray();
                            for(int i = 0; i < array.size(); i++)
                            {
                                addresses += array.at(i).toString();
                            }

                            QStringList signedAddresses;
                            QJsonArray array2 = resultObject.take("signed_addresses").toArray();
                            for(int i = 0; i < array2.size(); i++)
                            {
                                signedAddresses += array2.at(i).toString();
                            }

                            ui->fromLabel->setText(fromAddress);
                            ui->toLabel->setText(toAddress);
                            ui->requireLabel->setText(QString("%1 / %2").arg(signedAddresses.size()).arg(required));

                            int size = addresses.size();
                            ui->signInfoTableWidget->setRowCount(0);
                            ui->signInfoTableWidget->setRowCount(size);
                            for(int i = 0; i < size; i++)
                            {
                                QString address = addresses.at(i);
                                ui->signInfoTableWidget->setItem(i,0,new QTableWidgetItem(address));
                                ui->signInfoTableWidget->item(i,0)->setTextColor(QColor(192,196,212));

                                if(HXChain::getInstance()->isMyAddress(address))
                                {
                                    ui->signInfoTableWidget->setItem(i,1,new QTableWidgetItem(HXChain::getInstance()->addressToName(address)));
                                    ui->signInfoTableWidget->item(i,1)->setTextColor(QColor(43,230,131));
                                }
                                else
                                {
                                    ui->signInfoTableWidget->setItem(i,1,new QTableWidgetItem(tr("non-local")));
                                    ui->signInfoTableWidget->item(i,1)->setTextColor(QColor(255,34,76));
                                }

                                if(signedAddresses.contains(address))
                                {
                                    ui->signInfoTableWidget->setItem(i,2,new QTableWidgetItem(tr("signed")));
                                    ui->signInfoTableWidget->item(i,2)->setTextColor(QColor(43,230,131));
                                }
                                else
                                {
                                    ui->signInfoTableWidget->setItem(i,2,new QTableWidgetItem(tr("unsigned")));
                                    ui->signInfoTableWidget->item(i,2)->setTextColor(QColor(255,34,76));
                                }

                                if(HXChain::getInstance()->isMyAddress(address) && !signedAddresses.contains(address))
                                {
                                    localUnsignedCount++;
                                }        

                                for( int j = 0; j < 3; j++)
                                {
                                    ui->signInfoTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

                                    if(i % 2)
                                    {
                                        ui->signInfoTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
                                    }
                                    else
                                    {
                                        ui->signInfoTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        return;
    }


    if( id.startsWith("id_wallet_builder_add_signature") || id.startsWith("id_wallet_builder_file_add_signature")  )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
qDebug() << id << result;
        if( result.startsWith("\"result\":") )             // 成功
        {
            ui->stackedWidget->setCurrentIndex(2);

            QString builderStr = result.mid(9);
            ui->transactionBrowser2->setText(builderStr);

            if(id.startsWith("id_wallet_builder_file_add_signature"))
            {

                QString path = ui->pathLineEdit->text();
                path = path.left( path.lastIndexOf("/") );

#ifdef WIN32
                path.replace( "/", "\\");
                QProcess::startDetached("explorer \"" + path + "\"");
#else
                QProcess::startDetached("open \"" + path + "\"");
#endif
            }
        }

        return;
    }
}

void SignMultiSigDialog::on_nextBtn_clicked()
{
    if(useFile)
    {
        HXChain::getInstance()->postRPC( "id_wallet_builder_file_get_multisig_detail",
                                         toJsonFormat( "wallet_builder_file_get_multisig_detail",
                                                       QJsonArray() << ui->pathLineEdit->text()));

    }
    else
    {
        HXChain::getInstance()->postRPC( "id_wallet_builder_get_multisig_detail",
                                         toJsonFormat( "wallet_builder_get_multisig_detail",
                                                       QJsonArray() << ui->transactionBrowser->toPlainText()));

    }
}

void SignMultiSigDialog::on_cancelBtn_clicked()
{
    close();
}

void SignMultiSigDialog::on_fileCheckBox_clicked()
{
    useFile = true;
    ui->pathLineEdit->setEnabled(true);
    ui->selectPathBtn->setEnabled(true);

    ui->transactionBrowser->setEnabled(false);

    ui->fileCheckBox->setChecked(true);
    ui->bullderCheckBox->setChecked(false);
}

void SignMultiSigDialog::on_bullderCheckBox_clicked()
{
    useFile = false;
    ui->pathLineEdit->setEnabled(false);
    ui->selectPathBtn->setEnabled(false);

    ui->transactionBrowser->setEnabled(true);

    ui->fileCheckBox->setChecked(false);
    ui->bullderCheckBox->setChecked(true);
}

void SignMultiSigDialog::init()
{
    on_fileCheckBox_clicked();

    ui->stackedWidget->setCurrentIndex(0);

    ui->copyBtn->setToolTip(tr("copy to clipboard"));
}

void SignMultiSigDialog::on_selectPathBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose your multisig transaction file."),HXChain::getInstance()->walletConfigPath,"(*.json)");
#ifdef WIN32
    file.replace("\\","/");
#endif
    ui->pathLineEdit->setText(file);
}

void SignMultiSigDialog::on_backBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void SignMultiSigDialog::on_closeBtn_clicked()
{
    close();
}

void SignMultiSigDialog::on_signBtn_clicked()
{
    if(localUnsignedCount < 1)
    {
        CommonDialog tipDialog(CommonDialog::YesOrNo);
        tipDialog.setText( tr("No signature will be added. Do you still want to sign it?"));
        if( !tipDialog.pop() )      return;
    }

    CheckPwdDialog checkPwdDialog;
    if(!checkPwdDialog.pop())   return;

    if(useFile)
    {
        HXChain::getInstance()->postRPC( "id_wallet_builder_file_add_signature",
                                         toJsonFormat( "wallet_builder_file_add_signature",
                                                       QJsonArray() << ui->pathLineEdit->text() << "true"));

    }
    else
    {        
        HXChain::getInstance()->postRPC( "id_wallet_builder_add_signature",
                                         toJsonFormat( "wallet_builder_add_signature",
                                                       QJsonArray() << ui->transactionBrowser->toPlainText() << "true"));
    }
}

void SignMultiSigDialog::on_closeBtn2_clicked()
{
    close();
}

void SignMultiSigDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText( ui->transactionBrowser2->toPlainText() );
}

void SignMultiSigDialog::on_signInfoTableWidget_cellPressed(int row, int column)
{
    if( column == 0 )
    {
        ShowContentDialog showContentDialog( ui->signInfoTableWidget->item(row, column)->text(),this);

        int x = ui->signInfoTableWidget->columnViewportPosition(column) + ui->signInfoTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->signInfoTableWidget->rowViewportPosition(row) - 10 + ui->signInfoTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->signInfoTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void SignMultiSigDialog::on_closeBtn_2_clicked()
{
    close();
}

void SignMultiSigDialog::on_closeBtn3_clicked()
{
    close();
}
