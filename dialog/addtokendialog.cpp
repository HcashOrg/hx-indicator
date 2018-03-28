#include "addtokendialog.h"
#include "ui_addtokendialog.h"

#include <QDebug>

#include "lnk.h"
#include "../commondialog.h"


AddTokenDialog::AddTokenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTokenDialog)
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

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);

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

void AddTokenDialog::on_okBtn_clicked()
{
    if( UBChain::getInstance()->addressMap.keys().size() < 1) return;
    QString contractAddress = ui->contractAddressLineEdit->text().simplified();
    if(contractAddress.isEmpty())   return;

    UBChain::getInstance()->postRPC( "id_contract_get_info_addtokendialog+" + contractAddress, toJsonFormat( "contract_get_info", QStringList() << contractAddress
                                               ));


}

void AddTokenDialog::on_cancelBtn_clicked()
{
    close();
}

void AddTokenDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_contract_get_info_addtokendialog+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            QString contractAddress = id.mid(QString("id_contract_get_info_addtokendialog+").size());

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
                            QString id = resultObject.take("id").toString();
                            QString level = resultObject.take("level").toString();
                            if(level == "forever")
                            {
                                QJsonObject codeObject = resultObject.take("code_printable").toObject();
                                QString codeHash = codeObject.take("code_hash").toString();
        qDebug() << "added contract's  code hash  " << codeHash <<  id;
                                if( UBChain::getInstance()->allowedContractHashList.contains(codeHash))
                                {
                                    UBChain::getInstance()->postRPC( "id_contract_call_offline_state_addtokendialog+" + id,
                                                                     toJsonFormat( "contract_call_offline", QStringList() << id
                                                                               << UBChain::getInstance()->addressMap.keys().at(0) << "state" << ""
                                                                               ));
                                }
                                else
                                {
                                    CommonDialog commonDialog(CommonDialog::OkOnly);
                                    commonDialog.setText(tr("Nonstandard contract can not be added!"));
                                    commonDialog.pop();
                                }
                            }
                            else
                            {
                                CommonDialog commonDialog(CommonDialog::OkOnly);
                                commonDialog.setText(tr("Contract has not been upgraded!"));
                                commonDialog.pop();
                            }

                        }
                    }
                }
            }
        }
        else
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Wrong contract address!"));
            commonDialog.pop();
        }
        return;
    }


    if( id.startsWith( "id_contract_call_offline_state_addtokendialog+") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if( result.startsWith( "\"error\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText(QString::fromLocal8Bit("错误的合约地址!"));
            commonDialog.setText(tr("Wrong contract address!"));
            commonDialog.pop();
        }
        else if( result == "\"result\":\"NOT_INITED\"")
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText(QString::fromLocal8Bit("合约未初始化!"));
            commonDialog.setText(tr("Contract uninitialized!"));
            commonDialog.pop();
        }
        else
        {
            QString contractAddress = id.mid(46);
            UBChain::getInstance()->updateERC20TokenInfo(contractAddress);
            UBChain::getInstance()->addConcernedContract(contractAddress);
//            if( !UBChain::getInstance()->ERC20TokenInfoMap.keys().contains(contractAddress))
//            {
//                UBChain::getInstance()->ERC20TokenInfoMap.insert(contractAddress,ERC20TokenInfo());
//            }
//            UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].contractAddress = contractAddress;
//            UBChain::getInstance()->configFile->setValue("/AddedContractToken/" + contractAddress,1);
//            UBChain::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+tokenName+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
//                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "tokenName" << ""
//                                                       ));

//            UBChain::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+precision+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
//                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "precision" << ""
//                                                       ));

//            UBChain::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+admin+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
//                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "admin" << ""
//                                                       ));

//            UBChain::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+tokenSymbol+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
//                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "tokenSymbol" << ""
//                                                       ));

//            UBChain::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+totalSupply+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
//                                                       << UBChain::getInstance()->addressMap.keys().at(0) << "totalSupply" << ""
//                                                       ));

            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText(QString::fromLocal8Bit("合约资产添加成功!"));
            commonDialog.setText(tr("Contract added!"));
            commonDialog.pop();

            close();
        }

        return;
    }

//    if( id.startsWith( "id_contract_call_offline_addtokendialog+") )
//    {
//        QString result = UBChain::getInstance()->jsonDataValue(id);
//        qDebug() << id << result;

//        if( result.startsWith("\"result\":"))
//        {
//            int pos = 40;
//            QString func = id.mid( pos, id.indexOf("+" , pos) - pos);
//            QString contractAddress = id.mid(id.indexOf("+" , pos) + 1);

//            if( func == "tokenName")
//            {
//                QString tokenName = result.mid(9);
//                tokenName.remove('\"');
//                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].name = tokenName;
//            }
//            else if( func == "precision")
//            {
//                QString precision = result.mid(9);
//                precision.remove('\"');
//                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].precision = precision.toULongLong();
//            }
//            else if( func == "admin")
//            {
//                QString admin = result.mid(9);
//                admin.remove('\"');
//                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].admin = admin;
//            }
//            else if( func == "tokenSymbol")
//            {
//                QString tokenSymbol = result.mid(9);
//                tokenSymbol.remove('\"');
//                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].symbol = tokenSymbol;
//            }
//            else if( func == "totalSupply")
//            {
//                QString totalSupply = result.mid(9);
//                totalSupply.remove('\"');
//                UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].totalSupply = totalSupply.toULongLong();
//            }

//        }

//        return;
//    }

}

void AddTokenDialog::on_contractAddressLineEdit_returnPressed()
{
    on_okBtn_clicked();
}

void AddTokenDialog::on_closeBtn_clicked()
{
    on_cancelBtn_clicked();
}
