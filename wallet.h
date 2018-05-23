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
#include <QMap>
#include <QSettings>
#include <QProcess>
#include <QMutex>
#include <QDialog>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QPainter>
#include <QDateTime>
#include <QVector>
#include <QTimer>

#include "frame.h"
#include "extra/style.h"
#include "extra/transactiondb.h"
#include "extra/transactiontype.h"

#define ASSET_NAME "LNK"
#define ACCOUNT_ADDRESS_PREFIX  "LNK"
#define ASSET_PRECISION 5
#define WALLET_VERSION "1.0.0"           // 版本号
#define AUTO_REFRESH_TIME 5000           // 自动刷新时间(ms)
#define EXCHANGE_CONTRACT_HASH  "f138267ad4c454097a06fb2d80518ea7e8ef7370"

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


//  密码输入错误5次后 锁定一段时间 (秒)
#define PWD_LOCK_TIME  7200
#define NODE_RPC_PORT   50320
#define CLIENT_RPC_PORT 50321

class WorkerThreadManager;
class WebSocketManager;


static QMutex mutexForJsonData;
static QMutex mutexForPending;
static QMutex mutexForConfigFile;
static QMutex mutexForRegisterMap;
static QMutex mutexForBalanceMap;
static QMutex mutexForAddressMap;

struct AssetAmount
{
    QString assetId;
    unsigned long long amount = 0;
};
typedef QMap<QString,AssetAmount> AssetAmountMap;

struct ContractInfo
{
    QString contractAddress;
    QString hashValue;
    QString state;

    friend bool operator == (const ContractInfo& c1, const ContractInfo& c2);
};

struct AccountInfo
{
    QString name;
    QString address;
    QString id;

    AssetAmountMap   assetAmountMap;
    QVector<ContractInfo> contractsVector;
};

struct WalletInfo
{
    int blockHeight = 0;
    QString blockId;
    QString blockAge;
    QString chainId;
//    QStringList activeMiners;
};

struct Miner
{
    QString name;
    QString minerId;
};

typedef QMap<int,unsigned long long>  AssetBalanceMap;
typedef QMap<QString,unsigned long long>     ExchangeContractBalances;
struct AssetInfo
{
    QString id;
    QString symbol;
    QString issuer;
    int precision;
    unsigned long long maxSupply;
};

struct TransactionStruct
{
    QString transactionId;
    int     type = -1;
    int     blockNum = -1;
    QString expirationTime;
    QString operationStr;

public:

    friend QDataStream& operator >>(QDataStream &in,TransactionStruct& data);
    friend QDataStream& operator <<(QDataStream &out,const TransactionStruct& data);
};

struct TransactionTypeId
{
    int type;
    QString transactionId;

public:
    bool operator ==(const TransactionTypeId &_transactionTypeId) const
    {
        return this->transactionId == _transactionTypeId.transactionId;
    }

    friend QDataStream& operator >>(QDataStream &in,TransactionTypeId& data);
    friend QDataStream& operator <<(QDataStream &out,const TransactionTypeId& data);
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



struct TwoAddresses     // owner_address 和 active_address
{
    QString ownerAddress;
    QString activeAddress;
};

//朱正天---手续费
struct FeeChargeInfo
{
    FeeChargeInfo()
    {
        minerRegisterFee = "0";
        minerIncomeFee = "0";
        minerForeCloseFee = "0";
        minerRedeemFee = "0";
        tunnelBindFee = "0";
        poundagePublishFee = "0";
        poundageCancelFee = "0";
        transferFee = "0";

        withDrawFee = "0";
        capitalFee = "0";
    }

    QString minerRegisterFee;//挖矿注册手续费
    QString minerIncomeFee;//领取收益手续费
    QString minerForeCloseFee;//质押资产手续费
    QString minerRedeemFee;//赎回资产手续费
    QString tunnelBindFee;//绑定通道账户手续费
    QString poundagePublishFee;//发布承兑单手续费
    QString poundageCancelFee;//撤销承兑单手续费
    QString transferFee;//转账手续费

    QString withDrawFee;//提现手续费--
    QString capitalFee;//资金划转手续费
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
    QProcess* nodeProc;
    QProcess* clientProc;
    QTimer    timerForStartExe;
private slots:
    void onNodeExeStateChanged();
    void onClientExeStateChanged();
    void delayedLaunchClient();
    void checkNodeExeIsReady();
signals:
    void exeStarted();

    void showBubbleSignal(const QString &,const QString &,QSystemTrayIcon::MessageIcon, int);
public:
    void ShowBubbleMessage(const QString &title,const QString &context, int msecs = 10000,QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information);
public:
    WalletInfo walletInfo;

    bool importedWalletNeedToAddTrackAddresses = false;     // 如果是导入的钱包 需要添加到trackaddress

    QVector<Miner> minersVector;
    QString getMinerNameFromId(QString _minerId);

    int lockMinutes;   // 自动锁定时间
    bool notProduce;   // 是否产块/记账
    bool minimizeToTray;  // 是否最小化到托盘
    bool closeToMinimize; // 是否点击关闭最小化
    bool resyncNextTime;    // 下次启动时是否重新同步
    QString language;   // 语言
    QString feeType;//手续费币种
    bool IsBackupNeeded;//是否需要备份钱包
    QString feeOrderID;//承兑单id
    bool autoDeposit;//充值自动划转
    QString currentAccount; // 保存当前账户  切换页面的时候默认选择当前账户
    FeeChargeInfo feeChargeInfo;//手续费情况
    unsigned long long transactionFee;

    unsigned long long contractFee;     // 合约单步执行费用
    QString currentContractFee();

    struct TotalContractFee
    {
        unsigned long long baseAmount = 0;
        int step;
    };
    TotalContractFee parseTotalContractFee(QString result);

    QMap<QString,QString> balanceMap;
    QMap<QString,TwoAddresses> addressMap;
    QMap<QString,QString> registerMap;
    QStringList delegateList;
    bool hasDelegateSalary;
    QMap<QString,double> delegateSalaryMap;

    TwoAddresses getAddress(QString);
    QString addressToName(QString address);

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

    void appendCurrentDialogVector(QWidget*);
    void removeCurrentDialogVector(QWidget *);
    void hideCurrentDialog();
    void showCurrentDialog();
    void resetPosOfCurrentDialog();

    WebSocketManager* wsManager;
    void initWebSocketManager();

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

    QString middlewarePath;     // 中间件

    QMap<QString,AccountInfo>   accountInfoMap;
    QMap<QString,ExchangeContractBalances>  accountExchangeContractBalancesMap;
    void parseAccountInfo();
    void fetchAccountBalances(QString _accountName);
    void fetchMyContracts();
    bool    isMyAddress(QString _address);
    QString getAccountBalance(QString _accountName, QString _assetSymbol);
    QStringList getRegisteredAccounts();
    QStringList getUnregisteredAccounts();
    QString getExchangeContractAddress(QString _accountName);   // 没有兑换合约则返回空  有多个返回第一个
    QString getExchangeContractState(QString _accountName);   // 没有兑换合约则返回空  有多个返回第一个

    QMap<QString,AssetInfo>   assetInfoMap;
    void parseAssetInfo();
    QString getAssetId(QString symbol);

    QMap<QString,TransactionsInfoVector> transactionsMap;   // key是 "账户名_资产符号" 形式
    void parseTransactions(QString result, QString accountName = "ALL");
private:
    void InitFeeCharge();//设置手续费
public:
    void addTrackAddress(QString _address);     // 在chaindata/config.ini中添加 track-address


public:
    TransactionDB transactionDB;
    void fetchTransactions();

public:
    QMap<QString,MultiSigInfo>  multiSigInfoMap;

    QMap<QString,TransactionsInfoVector> multiSigTransactionsMap;   // key是多签地址
    void parseMultiSigTransactions(QString result, QString multiSigAddress);

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
QString getBigNumberString(unsigned long long number,int precision);
QString decimalToIntegerStr(QString number, int precision);

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

QString toJsonFormat(QString instruction, QJsonArray parameters);
unsigned long long jsonValueToULL(QJsonValue v);

#endif // LNK_H

