#include "CreateMultisigWidget.h"
#include "ui_CreateMultisigWidget.h"

#include "wallet.h"
#include "AddPubKeyDialog.h"
#include "ConfirmCreateMultiSigDialog.h"
#include "commondialog.h"
#include "AddLocalPubKeyDialog.h"
#include "showcontentdialog.h"

CreateMultisigWidget::CreateMultisigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateMultisigWidget)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->pubKeyTableWidget->installEventFilter(this);
    ui->pubKeyTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->pubKeyTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->pubKeyTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->pubKeyTableWidget->setFrameShape(QFrame::NoFrame);
    ui->pubKeyTableWidget->setMouseTracking(true);
    ui->pubKeyTableWidget->setShowGrid(false);//隐藏表格线

    ui->pubKeyTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->pubKeyTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->pubKeyTableWidget->horizontalHeader()->setVisible(true);
    ui->pubKeyTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->pubKeyTableWidget->setColumnWidth(0,530);
    ui->pubKeyTableWidget->setColumnWidth(1,100);
    ui->pubKeyTableWidget->horizontalHeader()->setStretchLastSection(true);

    QRegExp regx("[0-9]{2}");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->requiredLineEdit->setValidator( validator );

    showPubKeys();
}

CreateMultisigWidget::~CreateMultisigWidget()
{
    delete ui;
}

void CreateMultisigWidget::showPubKeys()
{
    int size = accountPubKeyVector.size();
    ui->pubKeyTableWidget->setRowCount(0);
    ui->pubKeyTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->pubKeyTableWidget->setItem( i, 0, new QTableWidgetItem( accountPubKeyVector.at(i).pubKey));
        ui->pubKeyTableWidget->setItem( i, 1, new QTableWidgetItem( tr("delete")));
    }

    tableWidgetSetItemZebraColor(ui->pubKeyTableWidget);

    ui->totalNumLabel->setText(tr("/ %1 (Max: 15)").arg(size));
    on_requiredLineEdit_textEdited(ui->requiredLineEdit->text());
}

void CreateMultisigWidget::jsonDataUpdated(QString id)
{
    if( id.startsWith("CreateMultisigWidget-create_multisignature_address-") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id  << result;

        QString paramsStr = id.mid(QString("CreateMultisigWidget-create_multisignature_address-").size());
        QJsonArray params = QJsonDocument::fromJson( paramsStr.toUtf8()).array();

        if( result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");
            QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
            QJsonArray array = object.value("result").toArray().at(0).toArray();
            QString multiAddress = array.at(1).toString();
            HXChain::getInstance()->configFile->setValue("/multisigAddresses/" + multiAddress, 1);

            if(paramsStr.size() > 0)
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText(tr("Multi-sig address created: %1").arg(multiAddress));
                commonDialog.pop();
            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText(tr("Multi-sig address has been added: %1").arg(multiAddress));
                commonDialog.pop();
            }
        }
        else
        {
            if(result.contains("unspecified: Assert Exception: !itr->multisignatures.valid():") && result.contains("\"multisignature\":\""))
            {
                int pos = result.indexOf("\"multisignature\":") + QString("\"multisignature\":").size();
                QString address = result.mid( pos, result.indexOf(',',pos) - pos);

                CommonDialog commonDialog(CommonDialog::YesOrNo);
                commonDialog.setText(tr("Multi-sig address %1 has already been created in the past.Do you want to add it to your wallet?").arg(address));
                if(commonDialog.pop())
                {
                    params << false;
                    HXChain::getInstance()->postRPC( "CreateMultisigWidget-create_multisignature_address-",
                                                     toJsonFormat( "create_multisignature_address",
                                                                   params ));

                }

            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText(tr("Fail to create the multi-sig address!"));
                commonDialog.pop();
            }
        }

        return;
    }
}

void CreateMultisigWidget::on_addPubKeyBtn_clicked()
{
    AddPubKeyDialog addPubKeyDialog;
    addPubKeyDialog.pop();
    if(!addPubKeyDialog.pubKey.isEmpty())
    {
        if(accountPubKeyVector.size() + 1 > 15)
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Multi-sig supports at most 15 public keys!"));
            commonDialog.pop();
            return;
        }

        AccountPubKey apk;
        apk.pubKey = addPubKeyDialog.pubKey;
        if(!accountPubKeyVector.contains(apk))
        {
            accountPubKeyVector.append(apk);
            showPubKeys();
        }
    }
}

void CreateMultisigWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void CreateMultisigWidget::on_requiredLineEdit_textEdited(const QString &arg1)
{
    if(ui->requiredLineEdit->text().toInt() < 1)
    {
        ui->requiredLineEdit->setText("1");
        return;
    }

    if( accountPubKeyVector.size() > 0 && ui->requiredLineEdit->text().toInt() > accountPubKeyVector.size())
    {
        ui->requiredLineEdit->setText(QString::number(accountPubKeyVector.size()));
    }
}

void CreateMultisigWidget::on_createBtn_clicked()
{
    if(accountPubKeyVector.size() < 2)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("A multi-sig address needs at least 2 public keys."));
        commonDialog.pop();
        return;
    }

    ConfirmCreateMultiSigDialog confirmCreateMultiSigDialog;
    if(confirmCreateMultiSigDialog.pop())
    {
        QJsonArray pubKeysArray;
        foreach (const AccountPubKey& pk, accountPubKeyVector)
        {
            pubKeysArray << pk.pubKey;
        }


        QJsonArray params = QJsonArray() << confirmCreateMultiSigDialog.account << pubKeysArray
                                         << ui->requiredLineEdit->text().toInt();
        QString str = QJsonDocument(params).toJson();
        params << true;
        HXChain::getInstance()->postRPC( "CreateMultisigWidget-create_multisignature_address-" + str,
                                         toJsonFormat( "create_multisignature_address",
                                                       params));

    }
}

void CreateMultisigWidget::on_pubKeyTableWidget_cellClicked(int row, int column)
{
    if(column == 0)
    {
        if( !ui->pubKeyTableWidget->item(row, column))    return;

        ShowContentDialog showContentDialog( ui->pubKeyTableWidget->item(row, column)->text(),this);

        int x = ui->pubKeyTableWidget->columnViewportPosition(column) + ui->pubKeyTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->pubKeyTableWidget->rowViewportPosition(row) - 10 + ui->pubKeyTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->pubKeyTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if(column == 1)
    {
        QTableWidgetItem* item = ui->pubKeyTableWidget->item(row,0);
        if(item)
        {
            accountPubKeyVector.remove(row);
            showPubKeys();
        }
        return;
    }
}

void CreateMultisigWidget::on_addLocalAccountBtn_clicked()
{
    AddLocalPubKeyDialog addLocalPubKeyDialog;
    addLocalPubKeyDialog.setAddedPubKeys(accountPubKeyVector);
    if(addLocalPubKeyDialog.pop())
    {
        qDebug() << "aaaaaaaaaaa " << addLocalPubKeyDialog.addedAccounts;
        QVector<AccountPubKey> tempVector = accountPubKeyVector;
        foreach (QString account, HXChain::getInstance()->accountInfoMap.keys())
        {
            QString pubKey = HXChain::getInstance()->accountInfoMap.value(account).pubKey;
            if(addLocalPubKeyDialog.addedAccounts.contains(account))
            {
                if(!accountPubKeyVector.contains(pubKey))
                {
                    accountPubKeyVector.append(pubKey);
                }
            }
            else
            {
                if(accountPubKeyVector.contains(pubKey))
                {
                    accountPubKeyVector.removeAll(pubKey);
                }
            }
        }

        if(accountPubKeyVector.size() > 15)
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Multi-sig supports at most 15 public keys!"));
            commonDialog.pop();
            accountPubKeyVector = tempVector;
            return;
        }

        showPubKeys();
    }
}
