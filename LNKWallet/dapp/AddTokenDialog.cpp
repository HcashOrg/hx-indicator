#include "AddTokenDialog.h"
#include "ui_AddTokenDialog.h"

#include "wallet.h"
#include "commondialog.h"

static QStringList tokenContractAbis = {"transfer", "transferFrom", "approve"};
static QStringList tokenContractOfflineAbis = {"precision", "totalSupply", "balanceOf", "approvedBalanceFrom", "allApprovedFromUser"};
static QStringList codeHashList = {"3e96f4a1fa3b647afb8cbb5aae08beaa50e4d9ba"};

AddTokenDialog::AddTokenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTokenDialog)
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

    QRegExp rx1(QString("[a-zA-Z0-9]{0,%1}").arg(QString(CONTRACT_ADDRESS_PREFIX).size() + 33));
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->contractAddressLineEdit->setValidator(pReg1);

    ui->contractAddressLineEdit->setFocus();
}

AddTokenDialog::~AddTokenDialog()
{
    delete ui;
}

void AddTokenDialog::pop()
{
    move(0,0);
    exec();
}

void AddTokenDialog::jsonDataUpdated(QString id)
{
    if(id == "AddTokenDialog-get_contract_info")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith(QString("\"result\":")))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object().value("result").toObject();
            QString contractId = object.value("id").toString();
            QJsonObject codeObject = object.value("code_printable").toObject();

            QJsonArray  abiArray   = codeObject.value("abi").toArray();
            QStringList abis;
            foreach (QJsonValue v, abiArray)
            {
                abis += v.toString();
            }

            QJsonArray  offlineAbiArray     = codeObject.value("offline_abi").toArray();
            QStringList offlineAbis;
            foreach (QJsonValue v, offlineAbiArray)
            {
                offlineAbis += v.toString();
            }

            QString     codeHash   = codeObject.value("code_hash").toString();

            bool isStandard = true;
            foreach (QString str, tokenContractAbis)
            {
                if(!abis.contains(str))
                {
                    isStandard = false;
                    break;
                }
            }
            foreach (QString str, tokenContractOfflineAbis)
            {
                if(!offlineAbis.contains(str))
                {
                    isStandard = false;
                    break;
                }
            }

            if(isStandard)
            {
                if(codeHashList.contains(codeHash))
                {
                    // 如果是模板合约  检查是否init了
                    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
                    if(accounts.size() == 0)    return;
                    HXChain::getInstance()->postRPC( "AddTokenDialog-invoke_contract_offline-state-" + contractId, toJsonFormat( "invoke_contract_offline",
                                                                                           QJsonArray() << accounts.at(0) << contractId
                                                                                           << "state"  << ""));
                }
                else
                {
                    HXChain::getInstance()->configFile->setValue("/contractTokens/" + contractId, 1);
                    newTokenAdded = true;

                    CommonDialog commonDialog(CommonDialog::OkOnly);
                    commonDialog.setText( tr("This contract has been added!") );
                    commonDialog.pop();
                }
            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("This contract does not meet the standard!") );
                commonDialog.pop();
            }

        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("This contract does not exist!") );
            commonDialog.pop();
        }

        return;
    }


    if(id.startsWith("AddTokenDialog-invoke_contract_offline-state-"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        QString contractId = id.mid(QString("AddTokenDialog-invoke_contract_offline-state-").size());

        if(result.startsWith(QString("\"result\":")))
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QString state = parse_doucment.object().value("result").toString();
            if(state == "NOT_INITED")
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("This contract has not been initialized!") );
                commonDialog.pop();
            }
            else
            {
                HXChain::getInstance()->configFile->setValue("/contractTokens/" + contractId, 1);
                newTokenAdded = true;

                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("This contract has been added!") );
                commonDialog.pop();
            }
        }
        return;
    }
}

void AddTokenDialog::on_closeBtn_clicked()
{
    close();
}

void AddTokenDialog::on_okBtn_clicked()
{
    if(ui->contractAddressLineEdit->text().isEmpty())   return;

    if(checkAddress(ui->contractAddressLineEdit->text(), ContractAddress))
    {
        HXChain::getInstance()->postRPC( "AddTokenDialog-get_contract_info", toJsonFormat( "get_contract_info",
                                                                               QJsonArray() << ui->contractAddressLineEdit->text()));
    }
    else
    {
        ui->tipLabel->setText(tr("This contract address is invalid"));
    }

}

void AddTokenDialog::on_cancelBtn_clicked()
{
    close();
}

void AddTokenDialog::on_contractAddressLineEdit_textChanged(const QString &arg1)
{
    ui->tipLabel->clear();
}
