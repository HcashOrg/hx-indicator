 #include "wallet.h"

#include "websocketmanager.h"
#include "commondialog.h"
#include "ToolButtonWidget.h"
#include "control/AssetIconItem.h"
#include "control/FeeGuaranteeWidget.h"
#include "extra/guiutil.h"

#ifdef WIN32
#include <Windows.h>
#endif
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QFrame>
#include <QDesktopWidget>
#include <QtMath>

#ifdef  WIN32
#define NODE_PROC_NAME      "hx_node"WALLET_EXE_SUFFIX".exe"
#define CLIENT_PROC_NAME    "hx_client"WALLET_EXE_SUFFIX".exe"
#define COPY_PROC_NAME      "Copy.exe"
#else
#define NODE_PROC_NAME      "./hx_node" WALLET_EXE_SUFFIX
#define CLIENT_PROC_NAME    "./hx_client" WALLET_EXE_SUFFIX
#define COPY_PROC_NAME      "./Copy"
#endif

#define MD5CHECK
//定义node client的md5值，用于版本判定（很多用户升级后，实际node、client并未替换成功，原因是进程未结束，文件无法替换，
//但是该问题很难发现，因此采用md5标记一下，每次启动时进行对比，将异常情况打入log日志中，方便排查,记得更换后台的时候同时更新该值）
//如果想取消该校验，注释 #define MD5CHECK即可
#ifdef MD5CHECK
#ifdef WIN32
#ifdef TEST_WALLET//windows测试链
static const QString NODE_PROC_MD5 = "79f462f3e29256ab60399b32464bd5f1";
static const QString CLIENT_PROC_MD5 = "05b40a9ec576cbdcba4ea5ffcf239534";
#else//windows正式链
static const QString NODE_PROC_MD5 = "f52a38c331790adf9e27bd7bdb990935";
static const QString CLIENT_PROC_MD5 = "f73e35c983364a14831291fab7cfac4d";
#endif
#else
#ifdef TEST_WALLET//mac测试链
static const QString NODE_PROC_MD5 = "tttttttttttttt";
static const QString CLIENT_PROC_MD5 = "tttttttttttttt";
#else//mac正式链
static const QString NODE_PROC_MD5 = "tttttttttttttt";
static const QString CLIENT_PROC_MD5 = "tttttttttttttt";
#endif
#endif
#endif

HXChain* HXChain::goo = 0;
static const QMap<QString,double>    defaultAutoWithdrawAmountMap = { {"BTC",10},{"LTC",1000},{"HC",10000},{"ETH",100} };
extern const QStringList ERCAssets = {"PAX","ELF"};

HXChain::HXChain()
{
    officialMiddleWareUrl<<MIDDLE_DEFAULT_URL<<"http://47.74.23.176:5005/api";
    currentAccount = "";
    nodeProc = new QProcess;
    clientProc = new QProcess;
    isExiting = false;
    isBlockSyncFinish = false;
    wsManager = NULL;

    getSystemEnvironmentPath();

    currentDialog = NULL;
    currentPort = CLIENT_RPC_PORT;
    currentAccount = "";

    witnessConfig = new WitnessConfig;
    configFile = new QSettings( walletConfigPath + "/config.ini", QSettings::IniFormat);
    if( configFile->value("/settings/lockMinutes").toInt() == 0)   // 如果第一次，没有config.ini
    {
        configFile->setValue("/settings/lockMinutes",5);
        lockMinutes     = 5;
        configFile->setValue("/settings/notAutoLock",false);
        notProduce      =  true;
        configFile->setValue("/settings/language","English");
        language = "English";
        configFile->setValue("/settings/feeType","HX");
        feeType = "HX";
        configFile->setValue("/settings/feeOrderID","");
        feeOrderID = "";
        configFile->setValue("/settings/backupNeeded",false);
        IsBackupNeeded = false;
        configFile->setValue("/settings/autoDeposit",false);
        autoDeposit = false;
        minimizeToTray  = false;
        configFile->setValue("/settings/minimizeToTray",false);
        closeToMinimize = false;
        configFile->setValue("/settings/closeToMinimize",false);
        resyncNextTime = false;
        configFile->setValue("settings/resyncNextTime",false);
        contractFee = 1;
        configFile->setValue("settings/contractFee",1);
        middlewarePath = MIDDLE_DEFAULT_URL;
        configFile->setValue("settings/middlewarePath",middlewarePath);
        configFile->setValue("/settings/autoWithdrawConfirm",false);
        autoWithdrawConfirm = false;

        configFile->setValue("/settings/autoBackupWallet",true);
        autoBackupWallet = true;
    }
    else
    {
        lockMinutes     = configFile->value("/settings/lockMinutes").toInt();
        notProduce      = !configFile->value("/settings/notAutoLock",false).toBool();
        minimizeToTray  = configFile->value("/settings/minimizeToTray",false).toBool();
        closeToMinimize = configFile->value("/settings/closeToMinimize",false).toBool();
        language        = configFile->value("/settings/language").toString();
        feeType         = configFile->value("/settings/feeType","HX").toString();
        feeOrderID      = configFile->value("/settings/feeOrderID").toString();
        IsBackupNeeded  = configFile->value("/settings/backupNeeded",false).toBool();
        autoDeposit     = configFile->value("/settings/autoDeposit",false).toBool();
        resyncNextTime  = configFile->value("/settings/resyncNextTime",false).toBool();
        contractFee     = configFile->value("/settings/contractFee",1).toULongLong();
        middlewarePath  = configFile->value("/settings/middlewarePath",MIDDLE_DEFAULT_URL).toString();
//        middlewarePath  = configFile->value("/settings/middlewarePath","http://192.168.1.164:5000/api").toString();
        importedWalletNeedToAddTrackAddresses = configFile->value("/settings/importedWalletNeedToAddTrackAddresses",false).toBool();
        autoWithdrawConfirm = configFile->value("/settings/autoWithdrawConfirm",false).toBool();

        autoBackupWallet = configFile->value("/settings/autoBackupWallet",true).toBool();
    }
    loadAutoWithdrawAmount();

    QFile file( walletConfigPath + "/log.txt");       // 每次启动清空 log.txt文件
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

//    contactsFile = new QFile( "contacts.dat");
    contactsFile = NULL;

    pendingFile  = new QFile( walletConfigPath + "/pending.dat");

    //初始化手续费
    InitFeeCharge();

    isUpdateNeeded = false;
//   updateProcess = new UpdateProcess();
//   connect(updateProcess,&UpdateProcess::updateFinish,[this](){
//        this->isUpdateNeeded = true;
//   });
//   connect(updateProcess,&UpdateProcess::updateWrong,[this](){
//       this->isUpdateNeeded = false;
//  });
#ifdef MD5CHECK
    //启动前校验一下md5，方便排查某些无法连接等错误
    GUIUtil::checkAndLogMd5(QCoreApplication::applicationDirPath()+ "/"+NODE_PROC_NAME,NODE_PROC_MD5,
                            QCoreApplication::applicationDirPath()+ "/"+CLIENT_PROC_NAME,CLIENT_PROC_MD5);
#endif
}

HXChain::~HXChain()
{


    if (configFile)
    {
        delete configFile;
        configFile = NULL;
    }

    if (witnessConfig)
    {
        delete witnessConfig;
        witnessConfig = NULL;
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

HXChain*   HXChain::getInstance()
{
    if( goo == NULL)
    {
        goo = new HXChain;
    }
    return goo;
}

void HXChain:: startExe()
{
    connect(nodeProc,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(onNodeExeStateChanged()));

    QStringList strList;
    strList << QString("--data-dir=\"%1\"").arg(HXChain::getInstance()->configFile->value("/settings/chainPath").toString().replace("\\","/"))
            << QString("--rpc-endpoint=127.0.0.1:%1").arg(NODE_RPC_PORT)
#ifndef SAFE_VERSION
//            << "--rewind-on-close"
#endif
            ;

    if( HXChain::getInstance()->configFile->value("/settings/resyncNextTime",false).toBool()
            ||  HXChain::getInstance()->configFile->value("/settings/dbReplay8",true).toBool())
    {
        strList << "--replay";
    }
    HXChain::getInstance()->configFile->setValue("/settings/resyncNextTime",false);
    HXChain::getInstance()->configFile->setValue("/settings/dbReplay8",false);

    nodeProc->start(NODE_PROC_NAME,strList);
    qDebug() << "start" << NODE_PROC_NAME << strList;
    logToFile( QStringList() << "start" << NODE_PROC_NAME << strList);

//    HXChain::getInstance()->initWebSocketManager();
//    emit exeStarted();
}

void HXChain::onNodeExeStateChanged()
{
    qDebug() << "node exe state " << nodeProc->state();

    if(isExiting)   return;

    if(nodeProc->state() == QProcess::Starting)
    {
        qDebug() << QString("%1 is starting").arg(NODE_PROC_NAME);
        logToFile( QStringList() << QString("%1 is starting").arg(NODE_PROC_NAME));
    }
    else if(nodeProc->state() == QProcess::Running)
    {
        qDebug() << QString("%1 is running").arg(NODE_PROC_NAME);
        logToFile( QStringList() << QString("%1 is running").arg(NODE_PROC_NAME));

        connect(&timerForStartExe,SIGNAL(timeout()),this,SLOT(checkNodeExeIsReady()));
        timerForStartExe.start(1000);
    }
    else if(nodeProc->state() == QProcess::NotRunning)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch %1 !").arg(NODE_PROC_NAME));
        commonDialog.pop();
    }
}

void HXChain::onClientExeStateChanged()
{
    if(isExiting)   return;

    if(clientProc->state() == QProcess::Starting)
    {
        qDebug() << QString("%1 is starting").arg(CLIENT_PROC_NAME);
    }
    else if(clientProc->state() == QProcess::Running)
    {
        qDebug() << QString("%1 is running").arg(CLIENT_PROC_NAME);

        HXChain::getInstance()->initWebSocketManager();
        emit exeStarted();
    }
    else if(clientProc->state() == QProcess::NotRunning)
    {
        qDebug() << "client not running" + clientProc->errorString();
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch %1 !").arg(CLIENT_PROC_NAME));
        commonDialog.pop();
    }
}

void HXChain::delayedLaunchClient()
{
    connect(clientProc,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(onClientExeStateChanged()));

    QStringList strList;
    strList << "--wallet-file=" + HXChain::getInstance()->configFile->value("/settings/chainPath").toString().replace("\\","/") + "/wallet.json"
            << QString("--server-rpc-endpoint=ws://127.0.0.1:%1").arg(NODE_RPC_PORT)
            << QString("--rpc-endpoint=127.0.0.1:%1").arg(CLIENT_RPC_PORT);

    clientProc->start(CLIENT_PROC_NAME,strList);
}

void HXChain::checkNodeExeIsReady()
{
    QString str = nodeProc->readAllStandardError();
    if(!str.isEmpty())
    {
        emit exeOutputMessage(str);
        qDebug() << "node exe standardError: " << str ;
        logToFile( QStringList() << "node exe standardError: " << str, 0, "node_output_log.txt" );
    }

    if(str.contains("Chain ID is"))
    {
        timerForStartExe.stop();
        QTimer::singleShot(1000,this,SLOT(delayedLaunchClient()));
        connect(nodeProc, SIGNAL(readyRead()), this, SLOT(readNodeOutput()));
    }
}

void HXChain::readNodeOutput()
{
    QString str = nodeProc->readAllStandardError();
    if(!str.isEmpty())
    {
        emit exeOutputMessage(str);
        logToFile( QStringList() << "node exe standardError: " << str, 0, "node_output_log.txt" );
    }

    QString str2 = nodeProc->readAllStandardOutput();
    if(!str2.isEmpty())
    {
        emit exeOutputMessage(str2);
        logToFile( QStringList() << "node exe standardOutput: " << str2, 0, "node_output_log.txt" );
    }
}

void HXChain::ShowBubbleMessage(const QString &title, const QString &context, int msecs, QSystemTrayIcon::MessageIcon icon)
{
    emit showBubbleSignal(title,context,icon,msecs);
}

QString HXChain::getMinerNameFromId(QString _minerId)
{
    QString result;
    foreach (QString key, minerMap.keys())
    {
        if(minerMap.value(key).minerId == _minerId)
        {
            result = key;
            break;
        }
    }

    return result;
}

QStringList HXChain::getMyCitizens()
{
    QStringList result;
    QStringList citizens = minerMap.keys();
    foreach (QString key, accountInfoMap.keys())
    {
        if(citizens.contains(key))
        {
            result += key;
        }
    }

    return result;
}

QString HXChain::currentContractFee()
{
    return getBigNumberString(contractFee,ASSET_PRECISION);
}

HXChain::TotalContractFee HXChain::parseTotalContractFee(QString result)
{
    result.prepend("{");
    result.append("}");

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(result);

    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba);
    QJsonObject object = parse_doucment.object();
    QJsonArray array = object.take("result").toArray();

    HXChain::TotalContractFee totalFee;
    totalFee.baseAmount = jsonValueToULL(array.at(0).toObject().take("amount"));
    totalFee.step = array.at(1).toInt();

    return totalFee;
}

qint64 HXChain::write(QString cmd)
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QByteArray cmdBa = gbkCodec->fromUnicode(cmd);  // 转为gbk的bytearray
    clientProc->readAll();
    return clientProc->write(cmdBa.data());
}

QString HXChain::read()
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



void HXChain::deleteAccountInConfigFile(QString accountName)
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


void HXChain::getSystemEnvironmentPath()
{
    QStringList environment = QProcess::systemEnvironment();
    QString str;

#ifdef WIN32
    foreach(str,environment)
    {
        if (str.startsWith("APPDATA="))
        {

            walletConfigPath = str.mid(8) + "\\HXIndicator" WALLET_EXE_SUFFIX "_" "1.1.0";
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

            walletConfigPath = str.mid(5) + "/Library/Application Support/HXIndicator" WALLET_EXE_SUFFIX "_" "1.1.0";
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
            walletConfigPath = str.mid(5) + "/HXIndicator"WALLET_EXE_SUFFIX;
            appDataPath = walletConfigPath + "/chaindata";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#endif
}


void HXChain::getContactsFile()
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

void HXChain::parseAccountInfo()
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
//            accountInfoMap.clear();
            QStringList accountNameList;

            QJsonArray resultArray = resultValue.toArray();
            for( int i = 0; i < resultArray.size(); i++)
            {
                QJsonObject object = resultArray.at(i).toObject();
                QString name = object.take("name").toString();

                AccountInfo accountInfo;
                if(accountInfoMap.contains(name))
                {
                    accountInfo = accountInfoMap.value(name);
                }

                accountInfo.id = object.take("id").toString();
                accountInfo.name = name;
                accountInfo.address = object.take("addr").toString();

                if(allGuardMap.contains(accountInfo.name) && allGuardMap.value(accountInfo.name).accountId == accountInfo.id)
                {
                    accountInfo.guardId = allGuardMap.value(accountInfo.name).guardId;
                    accountInfo.isFormalGuard = allGuardMap.value(accountInfo.name).isFormal;
                }

                accountInfoMap.insert(accountInfo.name,accountInfo);

                fetchAccountBalances(accountInfo.name);

                accountNameList.append(accountInfo.name);
            }

            foreach (QString key, accountInfoMap.keys())
            {
                if(!accountNameList.contains(key))
                {
                    accountInfoMap.remove(key);
                }
            }
        }
    }
}

void HXChain::fetchAccountBalances(QString _accountName)
{
    postRPC( "id-get_account_balances-" + _accountName, toJsonFormat( "get_account_balances", QJsonArray() << _accountName));
}

void HXChain::fetchAccountPubKey(QString _accountName)
{
    postRPC( "id+get_pubkey_from_account+" + _accountName, toJsonFormat( "get_pubkey_from_account", QJsonArray() << _accountName));
}

QString HXChain::getAccountBalance(QString _accountName, QString _assetSymbol)
{
    AssetAmountMap map = accountInfoMap.value(_accountName).assetAmountMap;
    QString assetId = getAssetId(_assetSymbol);
    AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(assetId);

    return getBigNumberString(map.value(assetId).amount, assetInfo.precision);
}

QStringList HXChain::getRegisteredAccounts()
{
    QStringList keys = accountInfoMap.keys();
    QStringList accounts;
    foreach (QString key, keys)
    {
        if(accountInfoMap.value(key).id != "0.0.0")
        {
            accounts += key;
        }
    }

    return accounts;
}

QStringList HXChain::getUnregisteredAccounts()
{
    QStringList keys = accountInfoMap.keys();
    QStringList accounts;
    foreach (QString key, keys)
    {
        if(accountInfoMap.value(key).id == "0.0.0")
        {
            accounts += key;
        }
    }

    return accounts;
}

QString HXChain::getExchangeContractAddress(QString _accountName)
{
    QString result;

    foreach (ContractInfo info, accountInfoMap.value(_accountName).contractsVector)
    {
        if(info.hashValue == EXCHANGE_CONTRACT_HASH)
        {
            result = info.contractAddress;
            break;
        }
    }

    return result;
}

QString HXChain::getExchangeContractState(QString _accountName)
{
    QString result;
    foreach (ContractInfo info, accountInfoMap.value(_accountName).contractsVector)
    {
        if(info.hashValue == EXCHANGE_CONTRACT_HASH)
        {
            result = info.state;
            break;
        }
    }

    return result;
}

QStringList HXChain::getMyMultiSigAddresses()
{
    configFile->beginGroup("/multisigAddresses");
    QStringList keys = configFile->childKeys();
    configFile->endGroup();
    return keys;
}


QString HXChain::getAssetId(QString symbol)
{
    QString id;
    foreach (QString key, HXChain::getInstance()->assetInfoMap.keys())
    {
        AssetInfo info = HXChain::getInstance()->assetInfoMap.value(key);
        if( info.symbol == symbol)
        {
            id = key;
            continue;
        }
    }

    return id;
}

QStringList HXChain::getETHAssets()
{
    QStringList result;
    foreach (QString key, HXChain::getInstance()->assetInfoMap.keys())
    {
        AssetInfo info = HXChain::getInstance()->assetInfoMap.value(key);
        if( info.symbol == "ETH" || info.symbol.startsWith("ERC"))
        {
            result += info.symbol;
        }
    }

    return result;
}

void HXChain::getExchangePairs()
{
    if( exchangeQueryCount++ % 10 != 0)     return;
    if(HXChain::getInstance()->accountInfoMap.size() < 1)   return;
    HXChain::getInstance()->postRPC( "id+invoke_contract_offline+getExchangePairs",
                                     toJsonFormat( "invoke_contract_offline",
                                     QJsonArray() << HXChain::getInstance()->accountInfoMap.keys().first() << EXCHANGE_MODE_CONTRACT_ADDRESS
                                                   << "getExchangePairs"  << ""));
}

int HXChain::getExchangePairPrecision(const ExchangePair &pair)
{
    QMap<QString,int> map = { {"BTC",5}, {"ETH",3}, {"HC",1}, {"HX",0}, {"LTC",3}, {"ERCPAX",1}, {"ERCELF",0}};

    int precision = map.value(pair.second) - map.value(pair.first) + 4;
    if(precision > 8)
    {
        precision = 8;
    }
    else if(precision < 2)
    {
        precision = 2;
    }

    return  precision;
}

int HXChain::getExchangeAmountPrecision(QString assetSymbol)
{
    QMap<QString,int> map = { {"BTC",4}, {"ETH",3}, {"HC",2}, {"HX",2}, {"LTC",3}, {"ERCPAX",2}, {"ERCELF",2}};

    int precision = 2;
    if(map.contains(assetSymbol))   precision = map.value(assetSymbol);

    return precision;
}

QStringList HXChain::getAllExchangeAssets()
{
    QStringList result;
    foreach (const ExchangePair& pair, pairInfoMap.keys())
    {
        if( pairInfoMap.value(pair).state != "NOT_INITED")
        {
            if(!result.contains(pair.first))    result.append(pair.first);
            if(!result.contains(pair.second))    result.append(pair.second);
        }
    }

    return result;
}

QList<ExchangePair> HXChain::getExchangePairsByQuoteAsset(QString quoteAssetSymbol)
{
    QList<ExchangePair> result;
    foreach (ExchangePair pair, pairInfoMap.keys())
    {
        if( quoteAssetSymbol.isEmpty() || pair.second == quoteAssetSymbol)
        {
            if(pairInfoMap.value(pair).state == "COMMON")
            {
                result.append(pair);
            }
        }
    }

    return result;
}

bool HXChain::isMyFavoritePair(const ExchangePair &pair)
{
    configFile->beginGroup("/MyExchangePairs");
    QStringList keys = HXChain::getInstance()->configFile->childKeys();
    configFile->endGroup();

    QString str = pair.first + "+" + pair.second;
    return keys.contains(str);
}


bool HXChain::ValidateOnChainOperation()
{
    if(GetBlockSyncFinish()) return true;
    //当前状态不允许链上操作时，弹出警告框
    if(mainFrame)
    {
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("You have not synchronized the latest block. The transaction you create will be outdated and not confirmed!"));
        dia.pop();
    }
    return false;
}

void HXChain::InitFeeCharge()
{//读取手续费文件
    QFile fileExit(":/config/feeCharge.config");
    if(!fileExit.open(QIODevice::ReadOnly)) return ;

    QString jsonStr(fileExit.readAll());
    fileExit.close();

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(jsonStr);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);

    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return ;

    QJsonObject jsonObject = parse_doucment.object();
    //开始分析
    QJsonObject feeObj = jsonObject.value("HXFee").toObject();
    feeChargeInfo.tunnelBindFee = feeObj.value("BindTunnel").toString();
    feeChargeInfo.minerIncomeFee = feeObj.value("MinerIncome").toString();
    feeChargeInfo.minerForeCloseFee = feeObj.value("MinerForeClose").toString();
    feeChargeInfo.minerRedeemFee = feeObj.value("MinerRedeem").toString();
    feeChargeInfo.accountRegisterFee = feeObj.value("AccountRegister").toString();
    feeChargeInfo.poundagePublishFee = feeObj.value("PoundagePublish").toString();
    feeChargeInfo.poundageCancelFee = feeObj.value("PoundageCancel").toString();
    feeChargeInfo.transferFee = feeObj.value("Transfer").toString();
    feeChargeInfo.createCitizenFee = feeObj.value("CreateCitizen").toString();
    feeChargeInfo.ChangeSenatorFee = feeObj.value("ChangeSenator").toString();
    feeChargeInfo.createSenatorFee = feeObj.value("CreateSenator").toString();
    feeChargeInfo.changePaybackFee = feeObj.value("ChangePayback").toString();

    //其他费用
    QJsonObject crossfeeObj = jsonObject.value("CrossFee").toObject();
    feeChargeInfo.withDrawFee = crossfeeObj.value("WithDraw").toString();
    feeChargeInfo.capitalFee = crossfeeObj.value("Capital").toString();
}

void HXChain::autoSaveWalletFile()
{
    if(!HXChain::getInstance()->autoBackupWallet) return;

    QFileInfo fileInfo(appDataPath + "/wallet.json");
    if(fileInfo.size() > 0)
    {
        QFile file(appDataPath + "/wallet.json");

        QString autoBakName = QString("/wallet_%1.json.autobak").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"));
        QFile autoSaveFile(appDataPath + autoBakName);
        qDebug() << "auto save remove " << autoSaveFile.remove();
        qDebug() << "auto save wallet.json " << file.copy(appDataPath + autoBakName);

        QFile autoSaveFile2(walletConfigPath + autoBakName);
        qDebug() << "auto save remove 2 " << autoSaveFile2.remove();
        qDebug() << "auto save wallet.json at configpath" << file.copy(walletConfigPath + autoBakName);
    }


}

void HXChain::fetchTransactions()
{
    if(trxQueryingFinished)
    {
        postRPC( "id+list_transactions+" + QString::number(walletInfo.blockHeight), toJsonFormat( "list_transactions", QJsonArray() << blockTrxFetched << -1));
        trxQueryingFinished = false;
    }

}

void HXChain::parseTransaction(QString result)
{
    result.prepend("{");
    result.append("}");

    QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
    QJsonObject jsonObject = parse_doucment.object();
    QJsonObject object = jsonObject.value("result").toObject();

    TransactionStruct ts;
    ts.transactionId = object.value("trxid").toString();
    ts.blockNum = object.value("block_num").toInt();
    ts.expirationTime = object.value("expiration").toString();

    QJsonArray array = object.value("operations").toArray().at(0).toArray();
    ts.type = array.at(0).toInt();
    QJsonObject operationObject = array.at(1).toObject();
    ts.operationStr = QJsonDocument(operationObject).toJson();
    ts.feeAmount = jsonValueToULL( operationObject.value("fee").toObject().value("amount"));

    if(operationObject.keys().contains("guarantee_id"))
    {
        ts.guaranteeId = operationObject.value("guarantee_id").toString();
        if( transactionDB.getGuaranteeOrder(ts.guaranteeId).id.isEmpty())
        {
            // 如果之前未获取过 就获取一下
            postRPC( "Transaction-get_guarantee_order-" + ts.transactionId, toJsonFormat( "get_guarantee_order", QJsonArray() << ts.guaranteeId));
        }
        else
        {
            TransactionTypeId typeId;
            typeId.type = ts.type;
            typeId.transactionId = ts.transactionId;
            HXChain::getInstance()->transactionDB.addAccountTransactionId(transactionDB.getGuaranteeOrder(ts.guaranteeId).ownerAddress, typeId);
        }
    }

    if(ts.type == TRANSACTION_TYPE_CANCEL_GUARANTEE)
    {
        QString canceledId = operationObject.value("cancel_guarantee_id").toString();
        postRPC( "Transaction-get_guarantee_order-", toJsonFormat( "get_guarantee_order", QJsonArray() << canceledId));
    }

    if(ts.type == TRANSACTION_TYPE_CONTRACT_TRANSFER)
    {
        if(transactionDB.getContractInvokeObject(ts.transactionId).trxId.isEmpty())
        {
            postRPC( "Transaction-get_contract_invoke_object", toJsonFormat( "get_contract_invoke_object", QJsonArray() << ts.transactionId));
        }
    }

    transactionDB.insertTransactionStruct(ts.transactionId,ts);
    qDebug() << "TTTTTTTTTTTTTTT " << ts.type << ts.transactionId  << ts.feeAmount;
    qDebug() << result;

    TransactionTypeId typeId;
    typeId.type = ts.type;
    typeId.transactionId = ts.transactionId;
    switch (typeId.type)
    {
    case TRANSACTION_TYPE_NORMAL:
    {
        // 普通交易
        QString fromAddress = operationObject.take("from_addr").toString();
        QString toAddress   = operationObject.take("to_addr").toString();


        if(isMyAddress(fromAddress))
        {
            transactionDB.addAccountTransactionId(fromAddress, typeId);
        }

        if(isMyAddress(toAddress))
        {
            transactionDB.addAccountTransactionId(toAddress, typeId);
        }
    }
        break;
    case TRANSACTION_TYPE_REGISTER_ACCOUNT:
    {
        // 注册账户
        QString payer = operationObject.take("payer").toString();

        transactionDB.addAccountTransactionId(payer, typeId);
    }
        break;
    case TRANSACTION_TYPE_UPDATE_ACCOUNT:
    {
        // 变更挖矿管理费
        QString addr = operationObject.value("addr").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_BIND_TUNNEL:
    {
        // 绑定tunnel地址
        QString addr = operationObject.take("addr").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_UNBIND_TUNNEL:
    {
        // 解绑tunnel地址
        QString addr = operationObject.take("addr").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_FEED_PRICE:
    {
        // 喂价
        QString addr = operationObject.take("publisher_addr").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_CREATE_MINER:
    {
        // 成为citizen
        QString addr = operationObject.take("miner_address").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_SPONSOR_PROPOSAL:
    {
        // 发起提案
        QString addr = operationObject.take("fee_paying_account").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_PROPOSAL_APPROVE:
    {
        // 提案投票
        QString addr = operationObject.take("fee_paying_account").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_CREATE_GUARD:
    {
        // 创建senator
        QString addr = operationObject.take("fee_pay_address").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_LOCKBALANCE:
    {
        // 质押资产给miner
        QString addr = operationObject.take("lock_balance_addr").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_FORECLOSE:
    {
        // 赎回质押资产
        QString addr = operationObject.take("foreclose_addr").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_SENATOR_LOCK_BALANCE:
    {
        // senator交保证金
        QString addr = operationObject.take("lock_address").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    case TRANSACTION_TYPE_DEPOSIT:
    {
        // 充值交易
        QJsonObject crossChainTrxObject = operationObject.take("cross_chain_trx").toObject();
        QString fromTunnelAddress = crossChainTrxObject.take("from_account").toString();
        QString depositAddress = operationObject.take("deposit_address").toString();

        transactionDB.addAccountTransactionId(fromTunnelAddress, typeId);
        transactionDB.addAccountTransactionId(depositAddress, typeId);
    }
        break;
    case TRANSACTION_TYPE_WITHDRAW:
    {
        // 提现交易
        QString withdrawAddress = operationObject.take("withdraw_account").toString();

        if(isMyAddress(withdrawAddress))
        {
            transactionDB.addAccountTransactionId(withdrawAddress, typeId);
        }

    }
        break;
    case TRANSACTION_TYPE_MINE_INCOME:
    {
        // 质押挖矿收入
        QString owner = operationObject.take("pay_back_owner").toString();

        if(isMyAddress(owner))
        {
            transactionDB.addAccountTransactionId(owner, typeId);
        }

    }
        break;
    case TRANSACTION_TYPE_ISSUE_ASSET:
    {
        // 发行资产
        QString issuer = operationObject.take("issuer_addr").toString();

        transactionDB.addAccountTransactionId(issuer, typeId);
    }
        break;
    case TRANSACTION_TYPE_CONTRACT_REGISTER:
    {
        // 注册合约
        QString ownerAddr = operationObject.take("owner_addr").toString();

        transactionDB.addAccountTransactionId(ownerAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_CONTRACT_INVOKE:
    {
        // 调用合约
        QString callerAddr = operationObject.take("caller_addr").toString();

        transactionDB.addAccountTransactionId(callerAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_CONTRACT_TRANSFER:
    {
        // 转账到合约
        QString callerAddr = operationObject.take("caller_addr").toString();

        transactionDB.addAccountTransactionId(callerAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_CREATE_GUARANTEE:
    {
        // 创建承兑单
        QString ownerAddr = operationObject.take("owner_addr").toString();

        transactionDB.addAccountTransactionId(ownerAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_CANCEL_GUARANTEE:
    {
        // 撤销承兑单
        QString ownerAddr = operationObject.take("owner_addr").toString();

        transactionDB.addAccountTransactionId(ownerAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_OBTAIN_BONUS:
    {
        // 领取分红
        QString ownerAddr = operationObject.take("bonus_owner").toString();

        transactionDB.addAccountTransactionId(ownerAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_SIGN_ETH_MULTI_CREATE:
    {
        QString guardAddr = operationObject.take("guard_sign_address").toString();

        transactionDB.addAccountTransactionId(guardAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_SIGN_ETH_FINAL:
    {
        QString guardAddr = operationObject.take("guard_address").toString();

        transactionDB.addAccountTransactionId(guardAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_SIGN_ETH_COLDHOT_FINAL:
    {
        QString guardAddr = operationObject.take("guard_address").toString();

        transactionDB.addAccountTransactionId(guardAddr, typeId);
    }
        break;
    case TRANSACTION_TYPE_CITIZEN_PROPOSAL:
    {
        QString addr = operationObject.take("fee_paying_account").toString();

        transactionDB.addAccountTransactionId(addr, typeId);
    }
        break;
    default:
        break;
    }
}

void HXChain::checkPendingTransactions()
{
    QStringList trxIds = transactionDB.getPendingTransactions();
    foreach (QString trxId, trxIds)
    {
        postRPC( "id-get_transaction-" + trxId,
                 toJsonFormat( "get_transaction", QJsonArray() << trxId),
                 1);
    }
}

//void HXChain::fetchFormalGuards()
//{
//    postRPC( "id-list_senator_members", toJsonFormat( "list_senator_members", QJsonArray() << "A" << 100));
//}

void HXChain::fetchAllGuards()
{
    postRPC( "id-list_all_senators", toJsonFormat( "list_all_senators", QJsonArray() << "A" << 100));
}

QStringList HXChain::getMyFormalGuards()
{
    QStringList result;

    foreach (QString key, accountInfoMap.keys())
    {
        if(allGuardMap.contains(key))
        {
            AccountInfo accountInfo = accountInfoMap.value(key);
            GuardInfo   guardInfo   = allGuardMap.value(key);
            if(accountInfo.id == guardInfo.accountId && guardInfo.isFormal)
            {
                result += key;
            }
        }
    }

    return result;
}

QStringList HXChain::getMyGuards()
{
    QStringList result;

    foreach (QString key, accountInfoMap.keys())
    {
        if(allGuardMap.contains(key))
        {
            AccountInfo accountInfo = accountInfoMap.value(key);
            GuardInfo   guardInfo   = allGuardMap.value(key);
            if(accountInfo.id == guardInfo.accountId)
            {
                result += key;
            }
        }
    }

    return result;
}

QStringList HXChain::getFormalGuards()
{
    QStringList result;

    foreach (QString key, allGuardMap.keys())
    {
        if(allGuardMap.value(key).isFormal)
        {
            result += key;
        }
    }

    return result;
}

QStringList HXChain::getPermanentSenators()
{
    QStringList result;
    QStringList allKeys = allGuardMap.keys();
    foreach (QString key, allKeys)
    {
//        qDebug()<<"nnnnnnn"<<key<<allGuardMap.value(key).isFormal<<allGuardMap.value(key).senatorType;
        if(allGuardMap.value(key).isFormal && allGuardMap.value(key).senatorType == "PERMANENT")
        {
            result << key;
        }
    }

    return result;
}


GuardMultisigAddress HXChain::getGuardMultisigByPairId(QString assetSymbol, QString guardName, QString pairId)
{
    QString guardAccountId = allGuardMap.value(guardName).accountId;
    QVector<GuardMultisigAddress> v = guardMultisigAddressesMap.value(assetSymbol + "-" + guardAccountId);

    GuardMultisigAddress result;
    foreach (GuardMultisigAddress gma, v)
    {
        if(gma.pairId == pairId)
        {
            result = gma;
        }
    }

    return result;
}

QString HXChain::getGuardNameByHotColdAddress(const QString &hotcoldaddress) const
{
    QMapIterator<QString,QVector<GuardMultisigAddress>> i(guardMultisigAddressesMap);
    while (i.hasNext()) {
        i.next();
        foreach (GuardMultisigAddress gma, i.value()) {
            if(hotcoldaddress == gma.coldAddress || hotcoldaddress == gma.hotAddress){
                QString senatorID = i.key().split("-").back();
                if(senatorID.isEmpty()) return "";
                QMapIterator<QString,GuardInfo> tt(allGuardMap);
                while(tt.hasNext()){
                    tt.next();
                    if(senatorID == tt.value().accountId){
                        return tt.key();
                    }

                }
            }
        }
    }

    return "";
}

void HXChain::fetchGuardAllMultisigAddresses(QString accountId)
{
    QStringList keys = assetInfoMap.keys();
    foreach (QString key, keys)
    {
        if(assetInfoMap.value(key).symbol == ASSET_NAME)   continue;

        QString assetSymbol = assetInfoMap.value(key).symbol;
        postRPC( "id-get_multi_address_obj-" + assetSymbol + "-" + accountId,
                 toJsonFormat( "get_multi_address_obj", QJsonArray() << assetSymbol << accountId));
    }
}

QStringList HXChain::getAssetMultisigUpdatedGuards(QString assetSymbol)
{
    QStringList result;
    QStringList keys = allGuardMap.keys();
    foreach (QString key, keys)
    {
        QString accountId = allGuardMap.value(key).accountId;
        QVector<GuardMultisigAddress> vector = guardMultisigAddressesMap.value(assetSymbol + "-" + accountId);
        foreach (GuardMultisigAddress gma, vector)
        {
            if(gma.pairId == "2.7.0")
            {
                result += accountId;
                break;
            }
        }
    }

    return result;
}

QString HXChain::guardAccountIdToName(QString guardAccountId)
{
    QString result;
    foreach (QString account, allGuardMap.keys())
    {
        if( allGuardMap.value(account).accountId == guardAccountId)
        {
            result = account;
            break;
        }
    }

    return result;
}

QString HXChain::guardAddressToName(QString guardAddress)
{
    QString result;
    foreach (QString account, allGuardMap.keys())
    {
        if( allGuardMap.value(account).address == guardAddress)
        {
            result = account;
            break;
        }
    }

    return result;
}

QString HXChain::guardOrCitizenAddressToName(QString address)
{
    foreach (QString account, allGuardMap.keys())
    {
        if( allGuardMap.value(account).address == address)
        {
            return account;
        }
    }

    foreach (QString account, minerMap.keys())
    {
        if( minerMap.value(account).address == address)
        {
            return account;
        }
    }

    return "";
}

void HXChain::loadAutoWithdrawAmount()
{
    configFile->beginGroup("/AutoWithdrawAmount");
    QStringList keys = configFile->childKeys();
    foreach (QString key, keys)
    {
        double amount = configFile->value(key,0).toDouble();
        senatorAutoWithdrawAmountMap.insert(key, amount);
    }
    configFile->endGroup();
}

double HXChain::getAssetAutoWithdrawLimit(QString symbol)
{
    if(senatorAutoWithdrawAmountMap.contains(symbol))
    {
        return senatorAutoWithdrawAmountMap.value(symbol);
    }

    if(defaultAutoWithdrawAmountMap.contains(symbol))
    {
        return defaultAutoWithdrawAmountMap.value(symbol);
    }

    return 0;
}

void HXChain::autoWithdrawSign()
{
    autoSignCount++;
    if(autoSignCount % 5 != 0)    return;

    if(HXChain::getInstance()->walletInfo.blockAge.contains("second")
            && HXChain::getInstance()->walletInfo.blockHeight > lastSignBlock)
    {
        lastSignBlock = HXChain::getInstance()->walletInfo.blockHeight;

        int signedCount = 0;
        QStringList keys = HXChain::getInstance()->applyTransactionMap.keys();
        foreach (QString key, keys)
        {
            ApplyTransaction at = HXChain::getInstance()->applyTransactionMap.value(key);
            QString generatedTrxId = HXChain::getInstance()->lookupGeneratedTrxByApplyTrxId(at.trxId);
            QStringList guardAddresses = HXChain::getInstance()->lookupSignedGuardsByGeneratedTrxId(generatedTrxId);

            if(!trxSignedGuardCountMap.contains(generatedTrxId))
            {
                trxSignedGuardCountMap.insert(generatedTrxId, guardAddresses.size());
            }
            else if( trxSignedGuardCountMap.value(generatedTrxId) == guardAddresses.size())
            {
                continue;       // 如果跟上次比数量没变 则跳过
            }
            else
            {
                trxSignedGuardCountMap[generatedTrxId] = guardAddresses.size();
            }

            foreach (QString account, HXChain::getInstance()->getMyFormalGuards())
            {

                QString accountAddress = HXChain::getInstance()->accountInfoMap.value(account).address;
                if(!guardAddresses.contains(accountAddress))
                {
                    if(at.amount.toDouble() <= getAssetAutoWithdrawLimit(at.assetSymbol))
                    {
                        if(singedAccountTrxsCountMap.contains(account + "+++" + generatedTrxId))
                        {
                            if(singedAccountTrxsCountMap.value(account + "+++" + generatedTrxId) < 24)
                            {
                                ++singedAccountTrxsCountMap[account + "+++" + generatedTrxId];
                                continue;
                            }
                        }

                        singedAccountTrxsCountMap[account + "+++" + generatedTrxId] = 0;

                        HXChain::getInstance()->postRPC( "id-senator_sign_crosschain_transaction", toJsonFormat( "senator_sign_crosschain_transaction",
                                                                                                                 QJsonArray() << generatedTrxId << account));

                        logToFile( QStringList() << "senator_sign_crosschain_transaction" << generatedTrxId << account);
                        signedCount++;
                        if(signedCount >= 3)    return;
                    }
                }
            }
        }


        keys = HXChain::getInstance()->ethFinalTrxMap.keys();
        foreach (QString key, keys)
        {
            ETHFinalTrx eft = HXChain::getInstance()->ethFinalTrxMap.value(key);

            foreach (QString account, HXChain::getInstance()->getMyFormalGuards())
            {
                AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(account);
                QVector<GuardMultisigAddress> vector = HXChain::getInstance()->guardMultisigAddressesMap.value(eft.symbol + "-" + accountInfo.id);
                foreach(const GuardMultisigAddress& gma, vector)
                {
                    if(gma.hotAddress == eft.signer)
                    {
                        if(singedAccountTrxsCountMap.contains(account + "+++" + eft.trxId))
                        {
                            if(singedAccountTrxsCountMap.value(account + "+++" + eft.trxId) < 24)
                            {
                                ++singedAccountTrxsCountMap[account + "+++" + eft.trxId];
                                continue;
                            }
                        }

                        singedAccountTrxsCountMap[account + "+++" + eft.trxId] = 0;

                        HXChain::getInstance()->postRPC( "autosign-senator_sign_eths_final_trx", toJsonFormat( "senator_sign_eths_final_trx",
                                                                         QJsonArray() << eft.trxId << account));
                        logToFile( QStringList() << "senator_sign_eths_final_trx" << eft.trxId << account);
                        signedCount++;
                        if(signedCount >= 3)    return;
                        break;
                    }
                }
            }
        }
    }
}


void HXChain::fetchCrosschainTransactions()
{
    postRPC( "id-get_crosschain_transaction-" + QString::number(0), toJsonFormat( "get_crosschain_transaction",
                                                                                    QJsonArray() << 0));

    postRPC( "id-get_crosschain_transaction-" + QString::number(1), toJsonFormat( "get_crosschain_transaction",
                                                                                    QJsonArray() << 1));

    postRPC( "id-get_crosschain_transaction-" + QString::number(2), toJsonFormat( "get_crosschain_transaction",
                                                                                    QJsonArray() << 2));

    postRPC( "id-get_crosschain_transaction-" + QString::number(7), toJsonFormat( "get_crosschain_transaction",
                                                                                    QJsonArray() << 7));

}

QString HXChain::lookupGeneratedTrxByApplyTrxId(QString applyTrxId)
{
    QString generatedTrxId;
    QStringList keys = HXChain::getInstance()->generatedTransactionMap.keys();
    foreach (QString key, keys)
    {
        GeneratedTransaction gt = HXChain::getInstance()->generatedTransactionMap.value(key);
        if(gt.ccwTrxIds.contains(applyTrxId))
        {
            generatedTrxId = gt.trxId;
            break;
        }
    }

    return generatedTrxId;
}

QStringList HXChain::lookupSignedGuardsByGeneratedTrxId(QString generatedTrxId)
{
    QStringList result;
    QStringList keys = signTransactionMap.keys();

    foreach (QString key, keys)
    {
        SignTransaction st = signTransactionMap.value(key);
        if(st.generatedTrxId == generatedTrxId)
        {
            result += st.guardAddress;
        }
    }

    return result;
}


void HXChain::fetchMiners()
{
    if(!fetchCitizensFinished)  return;
    fetchCitizensFinished = false;
    postRPC( "id-list_citizens", toJsonFormat( "list_citizens", QJsonArray() << "A" << 1000));
}

void HXChain::fetchCitizenPayBack()
{
    QStringList citizens = minerMap.keys();
    foreach (QString citizen, citizens)
    {
        postRPC( "citizen+get_account+" + citizen, toJsonFormat( "get_account", QJsonArray() << citizen));
    }
}

void HXChain::fetchProposals()
{
    if(HXChain::getInstance()->getPermanentSenators().size() < 1 || minerMap.size() < 1)   return;
    postRPC( "senator-get_proposal_for_voter", toJsonFormat( "get_proposal_for_voter", QJsonArray() << HXChain::getInstance()->getPermanentSenators().first()));
    postRPC( "citizen-get_proposal_for_voter", toJsonFormat( "get_referendum_for_voter", QJsonArray() << minerMap.keys().at(0)));

}

QString HXChain::citizenAccountIdToName(QString citizenAccountId)
{
    QString result;
    foreach (QString account, minerMap.keys())
    {
        if( minerMap.value(account).accountId == citizenAccountId)
        {
            result = account;
            break;
        }
    }

    return result;
}

void HXChain::fetchMyContracts()
{
    if( myContractsQueryCount++ % 10 != 0)     return;
    foreach (QString accountName, accountInfoMap.keys())
    {
        postRPC( "id-get_contracts_hash_entry_by_owner-" + accountName, toJsonFormat( "get_contracts_hash_entry_by_owner", QJsonArray() << accountName));
    }
}

bool HXChain::isMyAddress(QString _address)
{
    QStringList keys = accountInfoMap.keys();
    bool result = false;
    foreach (QString key, keys)
    {
        if(accountInfoMap.value(key).address == _address)   result = true;
    }

    if(!result)
    {
        if(getMyMultiSigAddresses().contains(_address))     result = true;
    }

    return result;
}

QString HXChain::addressToName(QString _address)
{
    QStringList keys = accountInfoMap.keys();

    foreach (QString key, keys)
    {
        if( accountInfoMap.value(key).address == _address)
        {
            return key;
        }
    }

    return "";
}


void HXChain::quit()
{
    isExiting = true;
    if (clientProc)
    {
//        clientProc->close();
        qDebug() << "clientProc: delete";

//        AttachConsole((uint)clientProc->processId());
//        SetConsoleCtrlHandler(NULL, true);
//        GenerateConsoleCtrlEvent( CTRL_C_EVENT ,0);
        clientProc->close();
//        delete clientProc;
        clientProc = NULL;
    }


    if (nodeProc)
    {
//        nodeProc->close();
        qDebug() << "nodeProc: delete";

//        AttachConsole((uint)nodeProc->processId());
//        SetConsoleCtrlHandler(NULL, true);
//        GenerateConsoleCtrlEvent( CTRL_C_EVENT ,0);

        nodeProc->close();
//        delete nodeProc;
        nodeProc = NULL;
    }



    if(isUpdateNeeded)
    {
        //启动外部复制程序
        qDebug()<<"chmod copy";
#ifdef TARGET_OS_MAC
        QProcess::execute("chmod",QStringList()<<"777"<<QCoreApplication::applicationDirPath()+"/Copy");
#endif
        QProcess *copproc = new QProcess();
        copproc->startDetached(COPY_PROC_NAME);
    }
}

void HXChain::updateJsonDataMap(QString id, QString data)
{
    mutexForJsonData.lock();

    jsonDataMap.insert( id, data);
    emit jsonDataUpdated(id);

    mutexForJsonData.unlock();

}

QString HXChain::jsonDataValue(QString id)
{

    mutexForJsonData.lock();

    QString value = jsonDataMap.value(id);

    mutexForJsonData.unlock();

    return value;
}

double HXChain::getPendingAmount(QString name)
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

QString HXChain::getPendingInfo(QString id)
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



//bool HXChain::rpcReceivedOrNotMapValue(QString key)
//{
//    mutexForRpcReceiveOrNot.lock();
//    bool received = rpcReceivedOrNotMap.value(key);
//    mutexForRpcReceiveOrNot.unlock();
//    return received;
//}

//void HXChain::rpcReceivedOrNotMapSetValue(QString key, bool received)
//{
//    mutexForRpcReceiveOrNot.lock();
//    rpcReceivedOrNotMap.insert(key, received);
//    mutexForRpcReceiveOrNot.unlock();
//}


void HXChain::appendCurrentDialogVector( QWidget * w)
{
    currentDialogVector.append(w);
}

void HXChain::removeCurrentDialogVector( QWidget * w)
{
    currentDialogVector.removeAll(w);
}

void HXChain::hideCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        w->hide();
    }
}

void HXChain::showCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        qDebug() << "showCurrentDialog :" << w;
        w->show();
        w->move( mainFrame->pos());  // lock界面可能会移动，重新显示的时候要随之移动
    }
}

void HXChain::resetPosOfCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        w->move( mainFrame->pos());
    }
}

void HXChain::initWebSocketManager()
{
    wsManager = new WebSocketManager;
    wsManager->start();
    wsManager->moveToThread(wsManager);

    connect(this, SIGNAL(rpcPosted(QString,QString,int)), wsManager, SLOT(processRPCs(QString,QString,int)));

}

void HXChain::postRPC(QString _rpcId, QString _rpcCmd, int _priority)
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

    emit rpcPosted(_rpcId, _rpcCmd, _priority);
}

double roundDown(double decimal, int precision)
{
    double result = QString::number(decimal,'f',precision).toDouble();
    qDebug() << result << decimal << (result > decimal + 1e-9);
    if( result > decimal + 1e-9)
    {
        result = result - qPow(10, 0 - precision);
    }

    return result;
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

QString removeFrontZeros(QString number)     // 去掉整数最前面的0
{
    while (number.startsWith('0'))
    {
        number.remove(0,1);
    }

    if( number.isEmpty())   number = "0";

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


QString decimalToIntegerStr(QString number, int precision)
{
    int pos = number.indexOf(".");
    if( pos == -1)  pos = number.size();

    number.remove(".");
    int size = number.size();

    QString zeroStr;
    zeroStr.fill('0', precision - (size - pos) );

    number.append(zeroStr);
    number = number.mid(0, pos + precision);     // 万一原数据小数位数超过precision  舍去超过的部分

    return removeFrontZeros(number);
}

AddressType checkAddress(QString address, AddressFlags type)
{
    if(type & AccountAddress)
    {
        if(address.startsWith(ACCOUNT_ADDRESS_PREFIX)  && (address.size() == QString(ACCOUNT_ADDRESS_PREFIX).size() + 33 ||
                                                           address.size() == QString(ACCOUNT_ADDRESS_PREFIX).size() + 32))
        {
            return AccountAddress;
        }
    }

    if(type & ContractAddress)
    {
        if(address.startsWith(CONTRACT_ADDRESS_PREFIX)  && (address.size() == QString(CONTRACT_ADDRESS_PREFIX).size() + 33 ||
                                                            address.size() == QString(CONTRACT_ADDRESS_PREFIX).size() + 32))
        {
            return ContractAddress;
        }
    }

    if(type & MultiSigAddress)
    {
        if(address.startsWith(MULTISIG_ADDRESS_PREFIX)  && (address.size() == QString(MULTISIG_ADDRESS_PREFIX).size() + 33 ||
                                                            address.size() == QString(MULTISIG_ADDRESS_PREFIX).size() + 32))
        {
            return MultiSigAddress;
        }
    }

    if(type & PubKey)
    {
        if(address.startsWith(PUBKEY_PREFIX)  && (address.size() == QString(PUBKEY_PREFIX).size() + 50))
        {
            return PubKey;
        }
    }

    return InvalidAddress;
}

void moveWidgetToScreenCenter(QWidget *w)
{
    QRect rect = QApplication::desktop()->availableGeometry();
    w->setGeometry( (rect.width() - w->width()) / 2, (rect.height() - w->height()) / 2,
                 w->width(), w->height());
}

QString toJsonFormat(QString instruction,
                      QJsonArray parameters)
{
    QJsonObject object;
    object.insert("id", 32800);
    object.insert("method", instruction);
    object.insert("params",parameters);

    return QJsonDocument(object).toJson();
}

QDataStream &operator >>(QDataStream &in, TransactionStruct &data)
{
    in >> data.transactionId >> data.type >> data.blockNum >> data.expirationTime >> data.operationStr >> data.feeAmount >> data.guaranteeId >> data.trxState;
    return in;
}

QDataStream &operator <<(QDataStream &out, const TransactionStruct &data)
{
    out << data.transactionId << data.type << data.blockNum << data.expirationTime << data.operationStr << data.feeAmount << data.guaranteeId << data.trxState;
    return out;
}

QDataStream &operator >>(QDataStream &in, TransactionTypeId &data)
{
    in >> data.type >> data.transactionId;
    return in;

}

QDataStream &operator <<(QDataStream &out, const TransactionTypeId &data)
{
    out << data.type << data.transactionId;
    return out;

}

QDataStream &operator >>(QDataStream &in, GuaranteeOrder &data)
{
    in >> data.id >> data.ownerAddress >> data.chainType >> data.time >> data.originAssetAmount.amount >> data.originAssetAmount.assetId
            >> data.targetAssetAmount.amount >> data.targetAssetAmount.assetId >> data.finishedAssetAmount.amount >> data.finishedAssetAmount.assetId
            >> data.records >> data.finished;
    return in;
}

QDataStream &operator <<(QDataStream &out, const GuaranteeOrder &data)
{
    out << data.id << data.ownerAddress << data.chainType << data.time << data.originAssetAmount.amount << data.originAssetAmount.assetId
            << data.targetAssetAmount.amount << data.targetAssetAmount.assetId << data.finishedAssetAmount.amount << data.finishedAssetAmount.assetId
            << data.records << data.finished;
    return out;
}

QDataStream &operator >>(QDataStream &in, ContractInvokeObject &data)
{
    in >> data.id >> data.trxId >> data.invoker >> data.execSucceed >> data.actualFee;
    return in;
}

QDataStream &operator <<(QDataStream &out, const ContractInvokeObject &data)
{
    out << data.id << data.trxId << data.invoker << data.execSucceed << data.actualFee;
    return out;
}

unsigned long long jsonValueToULL(QJsonValue v)
{
    unsigned long long result = 0;
    if(v.isString())
    {
        result = v.toString().toULongLong();
    }
    else
    {
//        result = QString::number(v.toDouble(),'g',16).toULongLong();
        result = v.toVariant().toULongLong();
    }

    return result;
}


long long jsonValueToLL(QJsonValue v)
{
    long long result = 0;
    if(v.isString())
    {
        result = v.toString().toLongLong();
    }
    else
    {
//        result = QString::number(v.toDouble(),'g',16).toLongLong();
        result = v.toVariant().toLongLong();
    }

    return result;
}

double jsonValueToDouble(QJsonValue v)
{
    double result = 0;
    if(v.isString())
    {
        result = v.toString().toDouble();
    }
    else
    {
        result = v.toDouble();
    }

    return result;
}

bool operator ==(const ContractInfo &c1, const ContractInfo &c2)
{
    return c1.contractAddress == c2.contractAddress;
}


void tableWidgetSetItemZebraColor(QTableWidget *w, int alignment)
{
    for(int row = 0; row < w->rowCount(); row++)
    {
        for(int column = 0; column < w->columnCount(); column++)
        {
            QTableWidgetItem* item = w->item(row,column);
            if(item)
            {
                item->setTextAlignment(alignment);
                if(row % 2)
                {
                    item->setBackgroundColor(QColor(238,236,245));
                }
                else
                {
                    item->setBackgroundColor(QColor(243,241,250));
                }
            }

            QWidget* cellWidget = w->cellWidget(row,column);
            if(cellWidget)
            {
                QString itemColor = (row % 2) ? "rgb(238,236,245)" : "rgb(243,241,250)";

                if( QString(cellWidget->metaObject()->className()) == "ToolButtonWidget")
                {
                    ToolButtonWidget* tbw = qobject_cast<ToolButtonWidget*>(cellWidget);
                    tbw->setBackgroundColor(itemColor);
                }
                else if( QString(cellWidget->metaObject()->className()) == "FeeGuaranteeWidget")
                {
                    FeeGuaranteeWidget* fgw = qobject_cast<FeeGuaranteeWidget*>(cellWidget);
                    fgw->setBackgroundColor(itemColor);
                }
                else if( QString(cellWidget->metaObject()->className()) == "AssetIconItem")
                {
                    AssetIconItem* ait = qobject_cast<AssetIconItem*>(cellWidget);
                    ait->setBackgroundColor(itemColor);
                }
                else if( QString(cellWidget->metaObject()->className()) == "ToolButtonWidgetItem")
                {
                    ToolButtonWidgetItem* tbwi = qobject_cast<ToolButtonWidgetItem*>(cellWidget);
                    tbwi->setBackgroundColor(itemColor);
                }
                else if( QString(cellWidget->metaObject()->className()) == "VoteStateLabel")
                {
                    QLabel* vsl = qobject_cast<QLabel*>(cellWidget);
                    vsl->setStyleSheet(QString("QLabel{background-color:%1;}").arg(itemColor));
                }
            }
        }
    }
}

QString toLocalTime(QString timeStr)
{
    timeStr.replace("T"," ");
    QDateTime dateTime = QDateTime::fromString(timeStr,"yyyy-MM-dd hh:mm:ss");
    dateTime.setTimeSpec(Qt::UTC);
    QDateTime localTime = dateTime.toLocalTime();
    return localTime.toString("yyyy-MM-dd hh:mm:ss");
}




QString revertERCSymbol(QString symbol)
{
    if(symbol.startsWith("ERC"))
    {
        return symbol.mid(QString("ERC").size());
    }
    else
    {
        return symbol;
    }
}

QString getRealAssetSymbol(QString symbol)
{
    if(ERCAssets.contains(symbol))
    {
        return "ERC" + symbol;
    }
    else
    {
        return symbol;
    }

}

int checkUseGuaranteeOrderType(QString payer, QString currentAddress, QString ownerAddress)
{
    int result = 0;
    if(ownerAddress.isEmpty())
    {
        // 没使用承兑单
        result = 0;
    }
    else if(currentAddress == ownerAddress)
    {
        if(payer == currentAddress)
        {
            // 手续费支付者是自己，说明是使用了自己的承兑单
            result = 3;
        }
        else
        {
            // 手续费支付者不是自己，说明是别人使用了我的承兑单
            result = 2;
        }
    }
    else
    {
        // 当前账户不是承兑单发布者，说明使用了别人的承兑单
        result = 1;
    }

    return result;
}

QString toEasyRead(unsigned long long number, int precision, int effectiveBitsNum)
{
    QString str = QString::number(number);
    if(str.size() <= precision + 3)
    {
        return getBigNumberString(number, precision);
    }
    else if(str.size() <= precision + 6)
    {
        str = getBigNumberString(number, precision + 3);
        str = str.left(effectiveBitsNum + 1);       // +1 是因为有小数点
        str += "K";
        return str;
    }
    else if(str.size() <= precision + 9)
    {
        str = getBigNumberString(number, precision + 6);
        str = str.left(effectiveBitsNum + 1);       // +1 是因为有小数点
        str += "M";
        return str;
    }
    else
    {
        str = getBigNumberString(number, precision + 6);
        str = str.left(str.indexOf("."));       // +1 是因为有小数点
        str += "M";
        return str;
    }
}



