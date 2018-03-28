/*
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
         佛祖保佑       永无BUG
*/

#ifndef LNK_H
#define LNK_H
#include <QObject>
#include <QMap>
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QMutex>
#include <QDialog>

#include "extra/transactioindb.h"
#include "frame.h"
#include "extra/style.h"
#include "extra/contractserverinfomanager.h"

#define ASSET_NAME "LNK"
#define ACCOUNT_ADDRESS_PREFIX  "LNK"
#define ASSET_PRECISION 100000
#define WALLET_VERSION "1.0.0"           // 版本号
#define AUTO_REFRESH_TIME 5000           // 自动刷新时间(ms)

#ifdef  TEST_WALLET
#define WALLET_EXE_SUFFIX   "_test"
#else
#define WALLET_EXE_SUFFIX   ""
#endif

#ifdef  WIN32
#define WALLET_EXE_FILE     "UB"WALLET_EXE_SUFFIX".exe"
#else
#define WALLET_EXE_FILE     "./UB"WALLET_EXE_SUFFIX
#endif


#define COLLECT_CONTRACT_TRANSACTION_INTERVAL  1000   // 采集合约交易时间间隔
#define ONCE_COLLECT_COUNT                      10    // 一次最多采集多少块


//  密码输入错误5次后 锁定一段时间 (秒)
#define PWD_LOCK_TIME  7200
#define NODE_RPC_PORT   50320
#define CLIENT_RPC_PORT 50321

#define FEEDER_CONTRACT_ADDRESS "price_feeder"      // 喂价合约地址


class QTimer;
class WorkerThreadManager;
class WebSocketManager;

static QMutex mutexForJsonData;
static QMutex mutexForPending;
static QMutex mutexForConfigFile;
static QMutex mutexForMainpage;
static QMutex mutexForPendingFile;
static QMutex mutexForDelegateList;
static QMutex mutexForRegisterMap;
static QMutex mutexForBalanceMap;
static QMutex mutexForAddressMap;
static QMutex mutexForRpcReceiveOrNot;


typedef QMap<int,unsigned long long>  AssetBalanceMap;
typedef QMap<QString,unsigned long long>  ContractBalanceMap;
struct AssetInfo
{
    int id;
    QString symbol;
    QString owner;
    QString name;
    QString description;
    unsigned long long precision;
    unsigned long long maxSupply;
    unsigned long long currentSupply;
    QString registrationDate;
};
struct AssetAmount
{
    int assetId;
    unsigned long long amount = 0;
};
struct ERC20TokenInfo
{
    QString contractAddress;
    QString contractName;
    QString level;
    QString symbol;
    QString name;
    QString admin;
    QString state;
    QString description;
    unsigned long long precision = 1;
    unsigned long long totalSupply = 0;
    int collectedBlockHeight = 0;   // 该合约已经采集到的高度
//    int interval = MAX_COLLECT_COUNT;             // 每次采集块数
//    int noResponseTimes = 0;        // 无返回次数 超过30重新发起 并且interval减半

    bool    isSmart = false;        // 是否是智能资产
    QString mortgageRate;       // 准备金率
    QString anchorTokenSymbol;
    unsigned long long amountCanMint;
    unsigned long long anchorTokenPrecision;
    unsigned long long anchorRatio;
    unsigned long long contractBalance;
    unsigned long long unusedBalance;
    int     baseMortgageRate = 0;
    int     lastTransferBlockNumber;


    QString price;      // 以下来自服务端
    QString website;
    int hotValue = 0;
    QString logoPath;
    int logoVersion = 0;
    QString summary;

};

struct Entry
{
    QString fromAccount;
    QString toAccount;
    AssetAmount amount;
    QString memo;
    QVector<AssetAmount>    runningBalances;
};
struct TransactionInfo
{
    QString trxId;
    bool isConfirmed;
    bool isMarket;
    bool isMarketCancel;
    int blockNum;
    QVector<Entry>  entries;
//    QString fromAaccount;
//    QString toAccount;
//    double amount;      //  交易金额
//    int assetId;        //  交易金额的资产类型
//    QString memo;
//    AssetBalanceMap runningBalances;
    unsigned long long fee;
    int    feeId;
    QString timeStamp;
};
typedef QVector<TransactionInfo>  TransactionsInfoVector;

struct ContractTransaction
{
    QString contractAddress;
    QString method;
    QString trxId;
    int blockNum;
    QString fromAddress;
    QString toAddress;
    unsigned long long amount;
    QString timeStamp;
    unsigned long long fee;

public:

    friend QDataStream& operator >>(QDataStream &in,ContractTransaction& data);
    friend QDataStream& operator <<(QDataStream &out,const ContractTransaction& data);
};

typedef QVector<ContractTransaction>  ContractTransactionVector;
struct AccountContractTransactions
{
    int collectedBlockNum = 0;
    ContractTransactionVector transactionVector;

public:
    friend QDataStream& operator >>(QDataStream &in,AccountContractTransactions& data);
    friend QDataStream& operator <<(QDataStream &out,const AccountContractTransactions& data);
};



struct TwoAddresses     // owner_address 和 active_address
{
    QString ownerAddress;
    QString activeAddress;
};

struct MultiSigInfo
{
    QString multiSigAddress;
    int requires;
    QStringList owners;
    AssetBalanceMap balanceMap;
};

class UBChain : public QObject
{
    Q_OBJECT
public:
    ~UBChain();
    static UBChain*   getInstance();


public:
    // 启动底层节点程序和client程序
    void startExe();
    qint64 write(QString);
    bool isExiting;
    void quit();
    QString read();
    QProcess* proc;
    QProcess* nodeProc;
    QProcess* clientProc;
private slots:
    void onNodeExeStateChanged();
    void onClientExeStateChanged();
signals:
    void exeStarted();


public:
    int lockMinutes;   // 自动锁定时间
    bool notProduce;   // 是否产块/记账
    bool minimizeToTray;  // 是否最小化到托盘
    bool closeToMinimize; // 是否点击关闭最小化
    bool resyncNextTime;    // 下次启动时是否重新同步
    QString language;   // 语言
    bool needToScan;   // 在当前scan完成后是否还需要scan
    QString currentAccount; // 保存当前账户  切换页面的时候默认选择当前账户
    QString currentTokenAddress;   // 当前选择的资产
    unsigned long long transactionFee;

    QMap<QString,QString> balanceMap;
    QMap<QString,TwoAddresses> addressMap;
    QMap<QString,QString> registerMap;
    QStringList delegateList;
    bool hasDelegateSalary;
    QMap<QString,double> delegateSalaryMap;
    QMap<QString,bool> rpcReceivedOrNotMap;  // 标识rpc指令是否已经返回了，如果还未返回则忽略

    TwoAddresses getAddress(QString);
    bool    isMyAddress(QString address);
    QString addressToName(QString address);

    QString getBalance(QString);
    QString getRegisterTime(QString);
    void deleteAccountInConfigFile(QString);
    void updateJsonDataMap(QString id, QString data);
    QString jsonDataValue(QString id);
    double getPendingAmount(QString name);
    QString getPendingInfo(QString id);

    QString registerMapValue(QString key);
    void registerMapInsert(QString key, QString value);
    int registerMapRemove(QString key);
    QString balanceMapValue(QString key);
    void balanceMapInsert(QString key, QString value);
    int balanceMapRemove(QString key);
    TwoAddresses addressMapValue(QString key);
    void addressMapInsert(QString key, TwoAddresses value);
    int addressMapRemove(QString key);
    bool rpcReceivedOrNotMapValue(QString key);
    void rpcReceivedOrNotMapSetValue(QString key, bool received);

    void appendCurrentDialogVector(QWidget*);
    void removeCurrentDialogVector(QWidget *);
    void hideCurrentDialog();
    void showCurrentDialog();
    void resetPosOfCurrentDialog();

    WebSocketManager* wsManager;

    void initWebSocketManager();
    WorkerThreadManager* workerManager;  // 处理rpc worker thread
    void initWorkerThreadManager();
    QThread* threadForWorkerManager;


    void postRPC(QString _rpcId, QString _rpcCmd);

    void getContactsFile();  // contacts.dat 改放到数据路径

    QVector<QWidget*> currentDialogVector;  // 保存不属于frame的dialog
                                            // 为的是自动锁定的时候hide这些dialog

    QSettings *configFile;
//    void loadAccountInfo();

    QString appDataPath;
    QString walletConfigPath;

    QFile* contactsFile;
    QFile* pendingFile;


    QDialog* currentDialog;  // 如果不为空 则指向当前最前面的不属于frame的dialog
                             // 为的是自动锁定的时候hide该dialog

    Frame* mainFrame = NULL; // 指向主窗口的指针

    int currentPort;          // 当前rpc 端口

    QString localIP;   // 保存 peerinfo 获得的本机IP和端口


    QMap<QString,AssetBalanceMap> accountBalanceMap;
    void parseBalance();

    QMap<int,AssetInfo>  assetInfoMap;
    void parseAssetInfo();
    int getAssetId(QString symbol);

    QMap<QString,TransactionsInfoVector> transactionsMap;   // key是 "账户名_资产符号" 形式
    void parseTransactions(QString result, QString accountName = "ALL");

    void scanLater();


    // 合约资产
    QMap<QString,ContractBalanceMap> accountContractBalanceMap;
    void updateAccountContractBalance(QString accountAddress, QString contractAddress);
    void updateAllContractBalance();

    QMap<QString,ERC20TokenInfo>  ERC20TokenInfoMap;
    void updateERC20TokenInfo(QString contract);
    void updateAllToken();

    int currentBlockHeight = 0;     // info 获得的当前区块高度

public:
    ContractServerInfoManager contractServerInfoManager;

public:
    TransactioinDB transactionDB;
    QMap<QString,AccountContractTransactions> accountContractTransactionMap;  // key是 "账户地址-合约地址" 形式
    void collectContractTransactions(QString contractAddress);
    void collectContracts();
    void getAllContractCollectedBlockHeight();
    void setContractCollectedBlockHeight(QString _contractAddress, int _blockHeight);
    QString currentCollectingContractAddress;
    void setCurrentCollectingContract(QString _contractAddress);
    bool isCollecting = false;
    void startCollectContractTransactions();
    bool checkContractTransactionExist(QString _key, QString _trxId);
    void setRecollect();        // 创建账户以及导入账户后 每个合约重新采集

public slots:
    void readAllAccountContractTransactions();              // 数据从db => 内存
    void saveAllAccountContractTransactions();              // 数据从内存 => db

public:
    QMap<QString,QString>   tokenPriceMap;      // 当前喂价
    QStringList feeders;
    QStringList getMyFeederAccount(); // 返回钱包中是feeder的账户
    QStringList tokens;
    void getTokensType();
    void getFeeders();

    QStringList delegateAccounts;

    QStringList   myConcernedContracts;
    QStringList   getConcernedContracts();
    void          addConcernedContract(QString contractAddress);
    void          removeConcernedContract(QString contractAddress);

    QStringList   allowedContractHashList;


    QMap<QString,MultiSigInfo>  multiSigInfoMap;

    QMap<QString,TransactionsInfoVector> multiSigTransactionsMap;   // key是多签地址
    void parseMultiSigTransactions(QString result, QString multiSigAddress);

public slots:
    void scan();


signals:
    void jsonDataUpdated(QString);

    void rpcPosted(QString rpcId, QString rpcCmd);

private:

    UBChain();
    static UBChain* goo;
    QMap<QString,QString> jsonDataMap;   //  各指令的id,各指令的返回

    void getSystemEnvironmentPath();

    class CGarbo // 它的唯一工作就是在析构函数中删除 的实例
    {
    public:
        ~CGarbo()
        {
            if (UBChain::goo)
                delete UBChain::goo;
        }
    };
    static CGarbo Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
};

QString doubleTo5Decimals(double number);
double roundDown(double decimal, int precision = 0);        // 根据精度 向下取"整"
QString removeLastZeros(QString number);        // qstring::number() 对小数的处理有问题  使用std::to_string() 然后把后面的0去掉
QString getBigNumberString(unsigned long long number,unsigned long long precision);

enum AddressType
{
    AccountAddress = 0x01,
    ContractAddress = 0x02,
    MultiSigAddress = 0x04,
    ScriptAddress = 0x08,
    InvalidAddress = 0x00
};
Q_DECLARE_FLAGS(AddressFlags, AddressType)
Q_DECLARE_OPERATORS_FOR_FLAGS(AddressFlags)
AddressType   checkAddress(QString address, AddressFlags type = AccountAddress);

void moveWidgetToScreenCenter(QWidget* w);

QString toJsonFormat(QString instruction, QStringList parameters);

#endif // LNK_H

