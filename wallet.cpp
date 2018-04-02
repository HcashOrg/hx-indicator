#include "wallet.h"
#include "debug_log.h"
#include "websocketmanager.h"
#include "commondialog.h"


#include <QTextCodec>
#include <QDebug>
#include <QObject>
#ifdef WIN32
#include <Windows.h>
#endif
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QFrame>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopWidget>

UBChain* UBChain::goo = 0;
//QMutex mutexForJsonData;
//QMutex mutexForPending;
//QMutex mutexForConfigFile;
//QMutex mutexForMainpage;
//QMutex mutexForPendingFile;
//QMutex mutexForDelegateList;
//QMutex mutexForRegisterMap;
//QMutex mutexForBalanceMap;
//QMutex mutexForAddressMap;
//QMutex mutexForRpcReceiveOrNot;

UBChain::UBChain()
{
    nodeProc = new QProcess;
    clientProc = new QProcess;
    isExiting = false;

    wsManager = NULL;

    getSystemEnvironmentPath();

    currentDialog = NULL;
    hasDelegateSalary = false;
    needToScan = false;
    currentPort = CLIENT_RPC_PORT;
    currentAccount = "";
    currentTokenAddress = "";
    transactionFee = 0;

    configFile = new QSettings( walletConfigPath + "/config.ini", QSettings::IniFormat);
    if( configFile->value("/settings/lockMinutes").toInt() == 0)   // 如果第一次，没有config.ini
    {
        configFile->setValue("/settings/lockMinutes",5);
        lockMinutes     = 5;
        configFile->setValue("/settings/notAutoLock",false);
        notProduce      =  true;
        configFile->setValue("/settings/language","English");
        language = "English";
        minimizeToTray  = false;
        configFile->setValue("/settings/minimizeToTray",false);
        closeToMinimize = false;
        configFile->setValue("/settings/closeToMinimize",false);
        resyncNextTime = false;
        configFile->setValue("settings/resyncNextTime",false);

    }
    else
    {
        lockMinutes     = configFile->value("/settings/lockMinutes").toInt();
        notProduce      = !configFile->value("/settings/notAutoLock").toBool();
        minimizeToTray  = configFile->value("/settings/minimizeToTray").toBool();
        closeToMinimize = configFile->value("/settings/closeToMinimize").toBool();
        language        = configFile->value("/settings/language").toString();
        resyncNextTime  = configFile->value("/settings/resyncNextTime",false).toBool();

    }

    getConcernedContracts();

    QFile file( walletConfigPath + "/log.txt");       // 每次启动清空 log.txt文件
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

//    contactsFile = new QFile( "contacts.dat");
    contactsFile = NULL;

    pendingFile  = new QFile( walletConfigPath + "/pending.dat");

    delegateAccounts << "ubinit0" << "ubinit1" << "ubinit2" << "ubinit3" << "ubinit4" << "ubinit5" << "ubinit6" << "ubinit7" << "ubinit8";

    allowedContractHashList << "45337709c727325eb67bd90c6b2eb068664ebcf2"  << "fb9e7e27462a6be3af552c0c200dfb95bf42b738";

}

UBChain::~UBChain()
{
    

	if (configFile)
	{
		delete configFile;
		configFile = NULL;
	}

    if( contactsFile)
    {
        delete contactsFile;
        contactsFile = NULL;
    }

    if( wsManager)
    {
        delete wsManager;
        wsManager = NULL;
    }
}

UBChain*   UBChain::getInstance()
{
    if( goo == NULL)
    {
        goo = new UBChain;
    }
    return goo;
}

void UBChain:: startExe()
{
    connect(nodeProc,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(onNodeExeStateChanged()));

    QStringList strList;
    strList << "--data-dir=" + UBChain::getInstance()->configFile->value("/settings/chainPath").toString()
            << QString("--rpc-endpoint=127.0.0.1:%1").arg(NODE_RPC_PORT);
qDebug() << "ddddddddddddd " << strList
            ;
    if( UBChain::getInstance()->configFile->value("/settings/resyncNextTime",false).toBool())
    {
        strList << "--resync-blockchain";
    }
    UBChain::getInstance()->configFile->setValue("settings/resyncNextTime",false);

    nodeProc->start("lnk_node.exe",strList);
}

void UBChain::onNodeExeStateChanged()
{
    if(isExiting)   return;

    if(nodeProc->state() == QProcess::Starting)
    {
        qDebug() << QString("%1 is starting").arg("lnk_node.exe");
    }
    else if(nodeProc->state() == QProcess::Running)
    {
        qDebug() << QString("%1 is running").arg("lnk_node.exe");

        connect(clientProc,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(onClientExeStateChanged()));

        QStringList strList;
        strList << "--wallet-file=" + UBChain::getInstance()->configFile->value("/settings/chainPath").toString().replace("\\","/") + "/wallet.json"
                << QString("--server-rpc-endpoint=ws://127.0.0.1:%1").arg(NODE_RPC_PORT)
                << QString("--rpc-endpoint=127.0.0.1:%1").arg(CLIENT_RPC_PORT);

        clientProc->start("lnk_client.exe",strList);
    }
    else if(nodeProc->state() == QProcess::NotRunning)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch %1 !").arg("lnk_node.exe"));
        commonDialog.pop();
    }
}

void UBChain::onClientExeStateChanged()
{
    if(isExiting)   return;

    if(clientProc->state() == QProcess::Starting)
    {
        qDebug() << QString("%1 is starting").arg("lnk_client.exe");
    }
    else if(clientProc->state() == QProcess::Running)
    {
        qDebug() << QString("%1 is running").arg("lnk_client.exe");

        emit exeStarted();
    }
    else if(clientProc->state() == QProcess::NotRunning)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch %1 !").arg("lnk_client.exe"));
        commonDialog.pop();
    }
}


qint64 UBChain::write(QString cmd)
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QByteArray cmdBa = gbkCodec->fromUnicode(cmd);  // 转为gbk的bytearray
    clientProc->readAll();
    return clientProc->write(cmdBa.data());
}

QString UBChain::read()
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result;
    QString str;
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    while ( !result.contains(">>>"))        // 确保读取全部输出
    {
        clientProc->waitForReadyRead(50);
        str = gbkCodec->toUnicode(clientProc->readAll());
        result += str;
//        if( str.right(2) == ": " )  break;

        //  手动调用处理未处理的事件，防止界面阻塞
//        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    QApplication::restoreOverrideCursor();
    return result;

}



void UBChain::deleteAccountInConfigFile(QString accountName)
{
    
    mutexForConfigFile.lock();
    configFile->beginGroup("/accountInfo");
    QStringList keys = configFile->childKeys();

    int i = 0;
    for( ; i < keys.size(); i++)
    {
        if( configFile->value( keys.at(i)) == accountName)
        {
            break;
        }
    }

    for( ; i < keys.size() - 1; i++)
    {
        configFile->setValue( keys.at(i) , configFile->value( keys.at(i + 1)));
    }

    configFile->remove( keys.at(i));

    configFile->endGroup();
    mutexForConfigFile.unlock();

    
}


TwoAddresses UBChain::getAddress(QString name)
{
    TwoAddresses twoAddresses;

    if( name.isEmpty())
    {
        return twoAddresses;
    }

    QString result = jsonDataValue("id_wallet_list_my_addresses");

    int pos = result.indexOf( "\"name\":\"" + name + "\",") ;
    if( pos != -1)  // 如果 wallet_list_my_addresses 中存在
    {

        int pos2 = result.indexOf( "\"owner_address\":", pos) + 17;
        twoAddresses.ownerAddress = result.mid( pos2, result.indexOf( "\"", pos2) - pos2);

        pos2 = result.indexOf( "\"active_address\":", pos) + 18;
        twoAddresses.activeAddress = result.mid( pos2, result.indexOf( "\"", pos2) - pos2);
    }

    return twoAddresses;
}

bool UBChain::isMyAddress(QString address)
{
    if( address.isEmpty())
    {
        return false;
    }

    bool result = false;
    foreach (QString key, addressMap.keys())
    {
         if( addressMap.value(key).ownerAddress == address || addressMap.value(key).activeAddress == address)
         {
             result = true;
             break;
         }
    }

    return result;
}

QString UBChain::addressToName(QString address)
{
    foreach (QString key, addressMap.keys())
    {
        if( addressMap.value(key).ownerAddress == address)
        {
            return key;
        }
    }

    return address;
}

QString UBChain::getBalance(QString name)
{
    if( name.isEmpty())
    {
        return NULL;
    }


    QString result = jsonDataValue("id_balance");

    int p = result.indexOf( "[\"" + name + "\",");
    if( p != -1)  // 如果balance中存在
    {
        int pos = p + 8 + name.size();
        QString str = result.mid( pos, result.indexOf( "]", pos) - pos);
        str.remove("\"");

        double amount = str.toDouble() / ASSET_PRECISION;

        return doubleTo5Decimals( amount) + " " + QString(ASSET_NAME);
    }

    // balance中不存在
    return "0.00000 " + QString(ASSET_NAME);
}

QString UBChain::getRegisterTime(QString name)
{
    if( name.isEmpty())
    {
        return NULL;
    }

    QString result = jsonDataValue("id_wallet_list_my_addresses");
    int pos = result.indexOf( "\"name\":\"" + name + "\",") ;
    if( pos != -1)  // 如果 wallet_list_my_addresses 中存在
    {
        int pos2 = result.indexOf( "\"registration_date\":", pos) + 21;
        QString registerTime = result.mid( pos2, result.indexOf( "\"", pos2) - pos2);

        if( registerTime != "1970-01-01T00:00:00")
        {
            return registerTime;
        }
        else
        {
            return "NO";
        }
    }

    return "WRONGACCOUNTNAME";
}

void UBChain::getSystemEnvironmentPath()
{
    QStringList environment = QProcess::systemEnvironment();
    QString str;

#ifdef WIN32
    foreach(str,environment)
    {
        if (str.startsWith("APPDATA="))
        {
#ifdef TEST_WALLET
            walletConfigPath = str.mid(8) + "\\LNKWallet_test";
#else
            walletConfigPath = str.mid(8) + "\\LNKWallet";
#endif
            appDataPath = walletConfigPath + "\\chaindata";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#elif defined(TARGET_OS_MAC)
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
#ifdef TEST_WALLET
            walletConfigPath = str.mid(5) + "/Library/Application Support/LNKWallet_test";
#else
            walletConfigPath = str.mid(5) + "/Library/Application Support/LNKWallet";
#endif
            appDataPath = walletConfigPath + "/chaindata";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#else
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
#ifdef TEST_WALLET
            walletConfigPath = str.mid(5) + "/LNKWallet_test";
#else
            walletConfigPath = str.mid(5) + "/LNKWallet";
#endif
            appDataPath = walletConfigPath + "/chaindata";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#endif
}


void UBChain::getContactsFile()
{
    QString path;
    if( configFile->contains("/settings/chainPath"))
    {
        path = configFile->value("/settings/chainPath").toString();
    }
    else
    {
        path = appDataPath;
    }

    contactsFile = new QFile(path + "\\contacts.dat");
    if( contactsFile->exists())
    {
        // 如果数据路径下 有contacts.dat 则使用数据路径下的
        return;
    }
//    else
//    {
//        QFile file2("contacts.dat");
//        if( file2.exists())
//        {
//            // 如果数据路径下没有 钱包路径下有 将钱包路径下的剪切到数据路径下
//       qDebug() << "contacts.dat copy" << file2.copy(path + "\\contacts.dat");
//       qDebug() << "contacts.dat copy" << file2.remove();
//            return;
//        }
//        else
//        {
//            // 如果都没有 则使用钱包路径下的
//        }
    //    }
}

void UBChain::parseAccountInfo()
{
    QString jsonResult = jsonDataValue("id-list_my_accounts");
    jsonResult.prepend("{");
    jsonResult.append("}");


    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonResult.toLatin1());
    QJsonObject jsonObject = parse_doucment.object();
    if( jsonObject.contains("result"))
    {
        QJsonValue resultValue = jsonObject.take("result");
        if( resultValue.isArray())
        {
            accountInfoMap.clear();

            QJsonArray resultArray = resultValue.toArray();
            for( int i = 0; i < resultArray.size(); i++)
            {
                AccountInfo accountInfo;

                QJsonObject object = resultArray.at(i).toObject();
                accountInfo.id = object.take("id").toString();
                accountInfo.name = object.take("name").toString();
                accountInfo.address = object.take("addr").toString();

                accountInfoMap.insert(accountInfo.name,accountInfo);

                getAccountBalances(accountInfo.name);
            }
        }
    }
}

void UBChain::getAccountBalances(QString _accountName)
{
    postRPC( "id-get_account_balances-" + _accountName, toJsonFormat( "get_account_balances", QStringList() << _accountName));
}

void UBChain::parseBalance()
{
    accountBalanceMap.clear();

    QString jsonResult = jsonDataValue("id_balance");
    jsonResult.prepend("{");
    jsonResult.append("}");

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonResult.toLatin1(), &json_error);
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
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        QJsonValue resultArrayValue = resultArray.at(i);
                        if( resultArrayValue.isArray())
                        {
                            QJsonArray array = resultArrayValue.toArray();
                            QJsonValue value = array.at(0);
                            QString account = value.toString();
                            AssetBalanceMap assetBalanceMap;

                            QJsonArray array2 = array.at(1).toArray();

                            for( int i = 0; i < array2.size(); i++)
                            {
                                QJsonArray array3 = array2.at(i).toArray();
                                int asset = array3.at(0).toInt();

                                unsigned long long balance;
                                if( array3.at(1).isString())
                                {
                                    balance = array3.at(1).toString().toULongLong();
                                }
                                else
                                {
                                    balance = QString::number(array3.at(1).toDouble(),'g',10).toULongLong();
                                }

                                assetBalanceMap.insert(asset, balance);

                            }
                            accountBalanceMap.insert(account,assetBalanceMap);
                        }
                    }
                }

            }
        }
    }


}


void UBChain::parseAssetInfo()
{
    assetInfoMap.clear();

    QString jsonResult = jsonDataValue("id-list_assets");
    jsonResult.prepend("{");
    jsonResult.append("}");

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonResult.toLatin1(), &json_error);
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
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        AssetInfo assetInfo;

                        QJsonObject object = resultArray.at(i).toObject();
                        assetInfo.id = object.take("id").toString();
                        assetInfo.symbol = object.take("symbol").toString();
                        assetInfo.issuer = object.take("issuer").toString();
                        assetInfo.precision = object.take("precision").toInt();

                        QJsonObject object2 = object.take("options").toObject();

                        QJsonValue value2 = object2.take("max_supply");
                        if( value2.isString())
                        {
                            assetInfo.maxSupply = value2.toString().toULongLong();
                        }
                        else
                        {
                            assetInfo.maxSupply = QString::number(value2.toDouble(),'g',10).toULongLong();
                        }

                        qDebug() << assetInfo.id << assetInfo.symbol << assetInfo.issuer << assetInfo.precision << assetInfo.maxSupply;

                        assetInfoMap.insert(assetInfo.id,assetInfo);
                    }
                }
            }
        }
    }

}

QString UBChain::getAssetId(QString symbol)
{
    QString id;
    foreach (QString key, UBChain::getInstance()->assetInfoMap.keys())
    {
        AssetInfo info = UBChain::getInstance()->assetInfoMap.value(key);
        if( info.symbol == symbol)
        {
            id = key;
            continue;
        }
    }

    return id;
}

void UBChain::parseTransactions(QString result, QString accountName)
{
    transactionsMap.remove(accountName);

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
                    TransactionsInfoVector transactionsInfoVector;
                    QJsonArray resultArray = resultValue.toArray();
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        TransactionInfo transactionInfo;

                        QJsonObject object          = resultArray.at(i).toObject();
                        transactionInfo.isConfirmed = object.take("is_confirmed").toBool();
                        if ( !transactionInfo.isConfirmed)
                        {
                            // 包含error  则为失效交易
                            if( object.contains("error"))   continue;
                        }

                        transactionInfo.trxId       = object.take("trx_id").toString();
                        transactionInfo.isMarket    = object.take("is_market").toBool();
                        transactionInfo.isMarketCancel = object.take("is_market_cancel").toBool();
                        transactionInfo.blockNum    = object.take("block_num").toInt();
                        transactionInfo.timeStamp   = object.take("timestamp").toString();

                        QJsonArray entriesArray       = object.take("ledger_entries").toArray();
                        for( int j = 0; j < entriesArray.size(); j++)
                        {
                            QJsonObject entryObject = entriesArray.at(j).toObject();
                            Entry   entry;
                            entry.fromAccount       = entryObject.take("from_account").toString();
                            entry.toAccount         = entryObject.take("to_account").toString();
                            QJsonValue v = entryObject.take("memo");
                            entry.memo              = v.toString();

                            QJsonObject amountObject = entryObject.take("amount").toObject();
                            entry.amount.assetId     = amountObject.take("asset_id").toInt();
                            QJsonValue amountValue   = amountObject.take("amount");
                            if( amountValue.isString())
                            {
                                entry.amount.amount  = amountValue.toString().toULongLong();
                            }
                            else
                            {
                                entry.amount.amount  = QString::number(amountValue.toDouble(),'g',10).toULongLong();
                            }

                            QJsonArray runningBalanceArray  = entryObject.take("running_balances").toArray().at(0).toArray().at(1).toArray();
                            for( int k = 0; k < runningBalanceArray.size(); k++)
                            {
                                QJsonObject amountObject2    = runningBalanceArray.at(k).toArray().at(1).toObject();
                                AssetAmount assetAmount;
                                assetAmount.assetId = amountObject2.take("asset_id").toInt();
                                QJsonValue amountValue2   = amountObject2.take("amount");
                                if( amountValue2.isString())
                                {
                                    assetAmount.amount  = amountValue2.toString().toULongLong();
                                }
                                else
                                {
                                    assetAmount.amount  = QString::number(amountValue2.toDouble(),'g',10).toULongLong();
                                }
                                entry.runningBalances.append(assetAmount);
                            }

                            transactionInfo.entries.append(entry);
                        }

                        QJsonObject object5         = object.take("fee").toObject();
                        QJsonValue amountValue3     = object5.take("amount");
                        if( amountValue3.isString())
                        {
                            transactionInfo.fee     = amountValue3.toString().toULongLong();
                        }
                        else
                        {
                            transactionInfo.fee     = QString::number(amountValue3.toDouble(),'g',10).toULongLong();
                        }
                        transactionInfo.feeId       = object5.take("asset_id").toInt();

                        transactionsInfoVector.append(transactionInfo);
                    }

                    transactionsMap.insert(accountName,transactionsInfoVector);
                }
            }
        }
    }
}

void UBChain::scanLater()
{
    QTimer::singleShot(10000,this,SLOT(scan()));
}

void UBChain::updateAccountContractBalance(QString accountAddress, QString contractAddress)
{
    if( addressMap.keys().size() < 1) return;
    postRPC( "id_contract_call_offline_balanceOf+" + contractAddress + "+" + accountAddress,
             toJsonFormat( "contract_call_offline",
                           QStringList() << contractAddress << addressMap.keys().at(0) << "balanceOf" << accountAddress
                           ));
}

void UBChain::updateAllContractBalance()
{
    QStringList keys = UBChain::getInstance()->addressMap.keys();
    QStringList contracts = ERC20TokenInfoMap.keys();
    foreach (QString key, keys)
    {
        foreach (QString contract, contracts)
        {
            updateAccountContractBalance(UBChain::getInstance()->addressMap.value(key).ownerAddress,contract);
        }
    }
}

void UBChain::updateERC20TokenInfo(QString contract)
{
    if( addressMap.keys().size() < 1) return;
    postRPC( "id_contract_get_info+" + contract, toJsonFormat( "contract_get_info", QStringList() << contract
                           ));
}

void UBChain::updateAllToken()
{
//    configFile->beginGroup("/AddedContractToken");
//    QStringList keys = configFile->childKeys();
//    foreach (QString key, keys)
//    {
//        updateERC20TokenInfo(key);
//    }
//    configFile->endGroup();

    foreach (QString address, myConcernedContracts)
    {
        updateERC20TokenInfo(address);
    }

    foreach (QString address, contractServerInfoManager.contractServerInfoSlice.map.keys())
    {
        updateERC20TokenInfo(address);
    }
}

void UBChain::readAllAccountContractTransactions()
{
    accountContractTransactionMap.clear();

    QStringList keys = UBChain::getInstance()->transactionDB.keys();

    QStringList accounts = UBChain::getInstance()->addressMap.keys();
    foreach (QString account, accounts)
    {
        QString accountAddress = UBChain::getInstance()->addressMap.value(account).ownerAddress;

        QStringList contracts = UBChain::getInstance()->ERC20TokenInfoMap.keys();

        foreach (QString contract, contracts)
        {
            QString key = accountAddress + "-" + contract;
            if(keys.contains(key))
            {
                accountContractTransactionMap.insert(key, transactionDB.getContractTransactions(key));
            }
            else
            {
                AccountContractTransactions transactions;
                transactions.collectedBlockNum = 0;
                accountContractTransactionMap.insert(key, transactions);
            }
        }
    }

    getAllContractCollectedBlockHeight();
}

void UBChain::saveAllAccountContractTransactions()
{
    QStringList keys = accountContractTransactionMap.keys();

    foreach (QString key, keys)
    {
        transactionDB.insertContractTransactions(key,accountContractTransactionMap.value(key));
    }
}

void UBChain::collectContractTransactions(QString contractAddress)
{
//    if( ERC20TokenInfoMap.keys().contains(contractAddress))
//    {
//        if( ERC20TokenInfoMap.value(contractAddress).collectedBlockHeight >= currentBlockHeight)    return;
//        if( ERC20TokenInfoMap.value(contractAddress).noResponseTimes == 0)
//        {
//            int fromBlockHeight = ERC20TokenInfoMap.value(contractAddress).collectedBlockHeight + 1;
//            int collectCount = ERC20TokenInfoMap.value(contractAddress).interval;
//            if( fromBlockHeight + collectCount > currentBlockHeight)     collectCount = currentBlockHeight - fromBlockHeight;

//            if( collectCount < 1)    return;
//            postRPC( toJsonFormat( "id_blockchain_list_contract_transaction_history+" + contractAddress + "+" + QString::number(collectCount),
//                                   "blockchain_list_contract_transaction_history",
//                                   QStringList() << QString::number(fromBlockHeight) << QString::number(collectCount)
//                                   << contractAddress  << "14"  ));
//            ERC20TokenInfoMap[contractAddress].noResponseTimes = 1;

//        }
//        else if( ERC20TokenInfoMap.value(contractAddress).noResponseTimes < 30)
//        {
//            ERC20TokenInfoMap[contractAddress].noResponseTimes++;
//            return;
//        }
//        else
//        {
//            ERC20TokenInfoMap[contractAddress].noResponseTimes = 0;
//            ERC20TokenInfoMap[contractAddress].interval = ERC20TokenInfoMap.value(contractAddress).interval / 10;
//            if( ERC20TokenInfoMap.value(contractAddress).interval < 1)   ERC20TokenInfoMap[contractAddress].interval = 1;
//            return;
//        }


//    }
}

void UBChain::collectContracts()
{
    foreach (QString key, ERC20TokenInfoMap.keys())
    {
//        collectContractTransactions(key);
    }
}

void UBChain::getAllContractCollectedBlockHeight()
{
    QStringList keys = accountContractTransactionMap.keys();
    if(keys.size() < 1)     return;

    QStringList contracts = ERC20TokenInfoMap.keys();
    foreach (QString contract, contracts)
    {
        int collectedBlockHeight = -1;
        foreach (QString key, keys)
        {
            if(key.endsWith(contract))
            {
                if(collectedBlockHeight == -1)
                {
                    collectedBlockHeight = accountContractTransactionMap.value(key).collectedBlockNum;
                    continue;
                }

                if( accountContractTransactionMap.value(key).collectedBlockNum < collectedBlockHeight)
                {
                    collectedBlockHeight = accountContractTransactionMap.value(key).collectedBlockNum;
                }
            }
        }

        if(collectedBlockHeight == -1)  collectedBlockHeight = 0;
        ERC20TokenInfoMap[contract].collectedBlockHeight = collectedBlockHeight;
    }
}

void UBChain::setContractCollectedBlockHeight(QString _contractAddress, int _blockHeight)
{
    QStringList keys = accountContractTransactionMap.keys();
    foreach (QString key, keys)
    {
        if(key.endsWith(_contractAddress))
        {
            if(_blockHeight > accountContractTransactionMap.value(key).collectedBlockNum)
            {
                accountContractTransactionMap[key].collectedBlockNum = _blockHeight;
            }
        }
    }
}

void UBChain::setCurrentCollectingContract(QString _contractAddress)
{
    currentCollectingContractAddress = _contractAddress;
}

void UBChain::startCollectContractTransactions()
{
    if(isCollecting)    return;

    QStringList contracts = ERC20TokenInfoMap.keys();
    if(contracts.size() < 1)    return;

    bool needToCollect = false;
    foreach (QString contract, contracts)
    {
//        qDebug() << "contract collected block height:   " << contract << ERC20TokenInfoMap.value(contract).collectedBlockHeight;
        if(ERC20TokenInfoMap.value(contract).collectedBlockHeight < currentBlockHeight)
        {
            setCurrentCollectingContract(contract);
            needToCollect = true;
            break;
        }
    }

    if(needToCollect)
    {
        int fromBlockHeight = ERC20TokenInfoMap.value(currentCollectingContractAddress).collectedBlockHeight + 1;
        postRPC( "id_blockchain_list_contract_transaction_history+" + currentCollectingContractAddress + "+" + QString::number(ONCE_COLLECT_COUNT),
                 toJsonFormat(  "blockchain_list_contract_transaction_history",
                                QStringList() << QString::number(fromBlockHeight) << QString::number(ONCE_COLLECT_COUNT)
                                << currentCollectingContractAddress  << "14"  ));

        isCollecting = true;
    }
    else
    {
        isCollecting = false;
    }

}

bool UBChain::checkContractTransactionExist(QString _key, QString _trxId)
{
    bool result = false;
    AccountContractTransactions transactions = accountContractTransactionMap.value(_key);
    foreach (ContractTransaction transaction, transactions.transactionVector)
    {
        if(transaction.trxId == _trxId)
        {
            result = true;
            break;
        }
    }

    return result;
}

void UBChain::setRecollect()
{
    QStringList contracts = ERC20TokenInfoMap.keys();
    foreach (QString contract, contracts)
    {
        ERC20TokenInfoMap[contract].collectedBlockHeight = 0;
    }
}

QStringList UBChain::getMyFeederAccount()
{
    QStringList myFeederAccounts;
    foreach (QString feederAddress, UBChain::getInstance()->feeders)
    {
        if( UBChain::getInstance()->isMyAddress(feederAddress))
        {
            myFeederAccounts += UBChain::getInstance()->addressToName(feederAddress);
        }
    }

    return myFeederAccounts;
}

void UBChain::getTokensType()
{
    if(UBChain::getInstance()->addressMap.size() == 0)  return;
    UBChain::getInstance()->postRPC( QString("id_contract_call_offline_") + FEEDER_CONTRACT_ADDRESS + "_to_feed_tokens",
                                     toJsonFormat( "contract_call_offline",
                                                   QStringList() << FEEDER_CONTRACT_ADDRESS << UBChain::getInstance()->addressMap.keys().at(0)
                                                   << "to_feed_tokens" << ""));
}

void UBChain::getFeeders()
{
    if(UBChain::getInstance()->addressMap.size() == 0)  return;
    UBChain::getInstance()->postRPC( QString("id_contract_call_offline_") + FEEDER_CONTRACT_ADDRESS + "_feeders",
                                     toJsonFormat( "contract_call_offline",
                                                   QStringList() << FEEDER_CONTRACT_ADDRESS << UBChain::getInstance()->addressMap.keys().at(0)
                                                   << "feeders" << ""));
}

QStringList UBChain::getConcernedContracts()
{
    myConcernedContracts.clear();

    configFile->beginGroup("/AddedContractToken");
    QStringList keys = configFile->childKeys();
    foreach (QString key, keys)
    {
        myConcernedContracts.append(key);
    }
    configFile->endGroup();

    return myConcernedContracts;
}

void UBChain::addConcernedContract(QString contractAddress)
{
    UBChain::getInstance()->configFile->setValue("/AddedContractToken/" + contractAddress,1);
    if(!myConcernedContracts.contains(contractAddress))     myConcernedContracts.append(contractAddress);

}

void UBChain::removeConcernedContract(QString contractAddress)
{

    UBChain::getInstance()->configFile->remove("/AddedContractToken/" + contractAddress);
    if(myConcernedContracts.contains(contractAddress))     myConcernedContracts.removeAll(contractAddress);


}

void UBChain::parseMultiSigTransactions(QString result, QString multiSigAddress)
{
    multiSigTransactionsMap.remove(multiSigAddress);

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
                    TransactionsInfoVector transactionsInfoVector;
                    QJsonArray resultArray = resultValue.toArray();

                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        TransactionInfo transactionInfo;

                        QJsonObject object          = resultArray.at(i).toObject();
                        transactionInfo.isConfirmed = object.take("is_confirmed").toBool();
                        if ( !transactionInfo.isConfirmed)
                        {
                            // 包含error  则为失效交易
                            if( object.contains("error"))   continue;
                        }

                        transactionInfo.trxId       = object.take("trx_id").toString();
                        transactionInfo.isMarket    = object.take("is_market").toBool();
                        transactionInfo.isMarketCancel = object.take("is_market_cancel").toBool();
                        transactionInfo.blockNum    = object.take("block_num").toInt();
                        transactionInfo.timeStamp   = object.take("timestamp").toString();

                        QJsonArray entriesArray       = object.take("ledger_entries").toArray();
                        for( int j = 0; j < entriesArray.size(); j++)
                        {
                            QJsonObject entryObject = entriesArray.at(j).toObject();
                            Entry   entry;
                            entry.fromAccount       = entryObject.take("from_account").toString();
                            entry.toAccount         = entryObject.take("to_account").toString();
                            QJsonValue v = entryObject.take("memo");
                            entry.memo              = v.toString();

                            QJsonObject amountObject = entryObject.take("amount").toObject();
                            entry.amount.assetId     = amountObject.take("asset_id").toInt();
                            QJsonValue amountValue   = amountObject.take("amount");
                            if( amountValue.isString())
                            {
                                entry.amount.amount  = amountValue.toString().toULongLong();
                            }
                            else
                            {
                                entry.amount.amount  = QString::number(amountValue.toDouble(),'g',10).toULongLong();
                            }

                            QJsonArray runningBalanceArray  = entryObject.take("running_balances").toArray().at(0).toArray().at(1).toArray();
                            for( int k = 0; k < runningBalanceArray.size(); k++)
                            {
                                QJsonObject amountObject2    = runningBalanceArray.at(k).toArray().at(1).toObject();
                                AssetAmount assetAmount;
                                assetAmount.assetId = amountObject2.take("asset_id").toInt();
                                QJsonValue amountValue2   = amountObject2.take("amount");
                                if( amountValue2.isString())
                                {
                                    assetAmount.amount  = amountValue2.toString().toULongLong();
                                }
                                else
                                {
                                    assetAmount.amount  = QString::number(amountValue2.toDouble(),'g',10).toULongLong();
                                }
                                entry.runningBalances.append(assetAmount);
                            }

                            transactionInfo.entries.append(entry);
                        }

                        QJsonObject object5         = object.take("fee").toObject();
                        QJsonValue amountValue3     = object5.take("amount");
                        if( amountValue3.isString())
                        {
                            transactionInfo.fee     = amountValue3.toString().toULongLong();
                        }
                        else
                        {
                            transactionInfo.fee     = QString::number(amountValue3.toDouble(),'g',10).toULongLong();
                        }
                        transactionInfo.feeId       = object5.take("asset_id").toInt();

                        transactionsInfoVector.append(transactionInfo);

                    }

                    multiSigTransactionsMap.insert(multiSigAddress,transactionsInfoVector);
                }
            }
        }
    }
}

void UBChain::scan()
{
    postRPC( "id_scan", toJsonFormat( "scan", QStringList() << "0"));
}



void UBChain::quit()
{
    isExiting = true;

    if (nodeProc)
    {
        nodeProc->close();
        qDebug() << "nodeProc: close";
        delete nodeProc;
        nodeProc = NULL;
    }

    if (clientProc)
    {
        clientProc->close();
        qDebug() << "nodeProc: close";
        delete clientProc;
        clientProc = NULL;
    }
}

void UBChain::updateJsonDataMap(QString id, QString data)
{
    mutexForJsonData.lock();

    jsonDataMap.insert( id, data);
    emit jsonDataUpdated(id);

    mutexForJsonData.unlock();

}

QString UBChain::jsonDataValue(QString id)
{

    mutexForJsonData.lock();

    QString value = jsonDataMap.value(id);

    mutexForJsonData.unlock();

    return value;
}

double UBChain::getPendingAmount(QString name)
{
    mutexForConfigFile.lock();
    if( !pendingFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "pending.dat not exist";
        return 0;
    }
    QString str = QByteArray::fromBase64( pendingFile->readAll());
    pendingFile->close();
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForConfigFile.unlock();

    double amount = 0;
    foreach (QString ss, strList)
    {
        QStringList sList = ss.split(",");
        if( sList.at(1) == name)
        {
            amount += sList.at(2).toDouble() + sList.at(3).toDouble();
        }
    }

    return amount;
}

QString UBChain::getPendingInfo(QString id)
{
    mutexForConfigFile.lock();
    if( !pendingFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "pending.dat not exist";
        return 0;
    }
    QString str = QByteArray::fromBase64( pendingFile->readAll());
    pendingFile->close();
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForConfigFile.unlock();

    QString info;
    foreach (QString ss, strList)
    {
        QStringList sList = ss.split(",");
        if( sList.at(0) == id)
        {
            info = ss;
            break;
        }
    }

    return info;
}

QString doubleTo5Decimals(double number)
{
        QString num = QString::number(number,'f', 5);
        int pos = num.indexOf('.') + 6;
        return num.mid(0,pos);
}

QString UBChain::registerMapValue(QString key)
{
    mutexForRegisterMap.lock();
    QString value = registerMap.value(key);
    mutexForRegisterMap.unlock();

    return value;
}

void UBChain::registerMapInsert(QString key, QString value)
{
    mutexForRegisterMap.lock();
    registerMap.insert(key,value);
    mutexForRegisterMap.unlock();
}

int UBChain::registerMapRemove(QString key)
{
    mutexForRegisterMap.lock();
    int number = registerMap.remove(key);
    mutexForRegisterMap.unlock();
    return number;
}

QString UBChain::balanceMapValue(QString key)
{
    mutexForBalanceMap.lock();
    QString value = balanceMap.value(key);
    mutexForBalanceMap.unlock();

    return value;
}

void UBChain::balanceMapInsert(QString key, QString value)
{
    mutexForBalanceMap.lock();
    balanceMap.insert(key,value);
    mutexForBalanceMap.unlock();
}

int UBChain::balanceMapRemove(QString key)
{
    mutexForBalanceMap.lock();
    int number = balanceMap.remove(key);
    mutexForBalanceMap.unlock();
    return number;
}

TwoAddresses UBChain::addressMapValue(QString key)
{
    mutexForAddressMap.lock();
    TwoAddresses value = addressMap.value(key);
    mutexForAddressMap.unlock();

    return value;
}

void UBChain::addressMapInsert(QString key, TwoAddresses value)
{
    mutexForAddressMap.lock();
    addressMap.insert(key,value);
    mutexForAddressMap.unlock();
}

int UBChain::addressMapRemove(QString key)
{
    mutexForAddressMap.lock();
    int number = addressMap.remove(key);
    mutexForAddressMap.unlock();
    return number;
}

//bool UBChain::rpcReceivedOrNotMapValue(QString key)
//{
//    mutexForRpcReceiveOrNot.lock();
//    bool received = rpcReceivedOrNotMap.value(key);
//    mutexForRpcReceiveOrNot.unlock();
//    return received;
//}

//void UBChain::rpcReceivedOrNotMapSetValue(QString key, bool received)
//{
//    mutexForRpcReceiveOrNot.lock();
//    rpcReceivedOrNotMap.insert(key, received);
//    mutexForRpcReceiveOrNot.unlock();
//}


void UBChain::appendCurrentDialogVector( QWidget * w)
{
    currentDialogVector.append(w);
}

void UBChain::removeCurrentDialogVector( QWidget * w)
{
    currentDialogVector.removeAll(w);
}

void UBChain::hideCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        w->hide();
    }
}

void UBChain::showCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        qDebug() << "showCurrentDialog :" << w;
        w->show();
        w->move( mainFrame->pos());  // lock界面可能会移动，重新显示的时候要随之移动
    }
}

void UBChain::resetPosOfCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        w->move( mainFrame->pos());
    }
}

void UBChain::initWebSocketManager()
{
    wsManager = new WebSocketManager;
    wsManager->start();
    wsManager->moveToThread(wsManager);

    connect(this, SIGNAL(rpcPosted(QString,QString)), wsManager, SLOT(processRPCs(QString,QString)));

}

void UBChain::postRPC(QString _rpcId, QString _rpcCmd)
{
//    if( rpcReceivedOrNotMap.contains( _rpcId))
//    {
//        if( rpcReceivedOrNotMap.value( _rpcId) == true)
//        {
//            rpcReceivedOrNotMapSetValue( _rpcId, false);
//            emit rpcPosted(_rpcId, _rpcCmd);
//        }
//        else
//        {
//            // 如果标识为未返回 则不重复排入事件队列
//            return;
//        }
//    }
//    else
//    {
//        rpcReceivedOrNotMapSetValue( _rpcId, false);

//        emit rpcPosted(_rpcId, _rpcCmd);
//    }

    emit rpcPosted(_rpcId, _rpcCmd);
}

double roundDown(double decimal, int precision)
{
    int precisonFigureNum   = QString::number( precision, 'g', 15).count() - 1;

    double result = QString::number(decimal,'f',precisonFigureNum).toDouble();
    if( result > decimal)
    {
        if( precision == 0)     precision = 1;
        result = result - 1.0 / precision;
    }

    return result;
}

bool isExistInWallet(QString strName)
{
    mutexForAddressMap.lock();
    for (QMap<QString,TwoAddresses>::const_iterator i = UBChain::getInstance()->addressMap.constBegin(); i != UBChain::getInstance()->addressMap.constEnd(); ++i)
    {
        if(i.key() == strName)
        {
            mutexForAddressMap.unlock();
            return true;
        }
    }
    mutexForAddressMap.unlock();
    return false;
}

QString removeLastZeros(QString number)     // 去掉小数最后面的0
{
    if( !number.contains('.'))  return number;

    while (number.endsWith('0'))
    {
        number.chop(1);
    }

    if( number.endsWith('.'))   number.chop(1);

    return number;
}

QString getBigNumberString(unsigned long long number, int precision)
{
    QString str = QString::number(number);
    int size = precision;
    if( size < 0)  return "";
    if( size == 0) return str;

    if( str.size() > size)
    {
        str.insert(str.size() - size, '.');
    }
    else
    {
        // 前面需要补0
        QString zeros;
        zeros.fill('0',size - str.size() + 1);
        str.insert(0,zeros);
        str.insert(1,'.');
    }

    return removeLastZeros(str);
}

AddressType checkAddress(QString address, AddressFlags type)
{
    if(type & AccountAddress)
    {
        if(address.startsWith(ACCOUNT_ADDRESS_PREFIX)  && address.size() == QString(ACCOUNT_ADDRESS_PREFIX).size() + 33)
        {
            return AccountAddress;
        }
    }

    if(type & ContractAddress)
    {
        if(address.startsWith("C")  && address.size() == QString("C").size() + 33)
        {
            return ContractAddress;
        }
    }

    if(type & MultiSigAddress)
    {
        if(address.startsWith("M")  && address.size() == QString("M").size() + 33)
        {
            return MultiSigAddress;
        }
    }

    if(type & ScriptAddress)
    {
        if(address.startsWith("S")  && address.size() == QString("S").size() + 33)
        {
            return ScriptAddress;
        }
    }

    return InvalidAddress;
}


QDataStream &operator >>(QDataStream &in, ContractTransaction &data)
{
    in >> data.contractAddress >> data.method >> data.trxId >> data.blockNum >> data.fromAddress >> data.toAddress
            >> data.amount >> data.timeStamp >> data.fee;
    return in;
}

QDataStream &operator <<(QDataStream &out, const ContractTransaction &data)
{
    out << data.contractAddress << data.method << data.trxId << data.blockNum << data.fromAddress << data.toAddress
            << data.amount << data.timeStamp << data.fee;
    return out;
}

QDataStream &operator >>(QDataStream &in, AccountContractTransactions &data)
{
    in >> data.collectedBlockNum >> data.transactionVector;
    return in;
}

QDataStream &operator <<(QDataStream &out, const AccountContractTransactions &data)
{
    out << data.collectedBlockNum << data.transactionVector;
    return out;
}

void moveWidgetToScreenCenter(QWidget *w)
{
    QRect rect = QApplication::desktop()->availableGeometry();
    w->setGeometry( (rect.width() - w->width()) / 2, (rect.height() - w->height()) / 2,
                 w->width(), w->height());
}

QString toJsonFormat(QString instruction,
                      QStringList parameters)
{
    QJsonObject object;
    object.insert("id", 32800);
    object.insert("method", instruction);
    QJsonArray array;
    foreach (QString param, parameters)
    {
        array.append(param);
    }
    object.insert("params",array);

    return QJsonDocument(object).toJson();
}
