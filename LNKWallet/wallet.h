#ifndef WALLET_H
#define WALLET_H
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
#include <QGraphicsEffect>
#include <QTableWidget>
#include <QLabel>
#include <math.h>

#include "frame.h"
#include "extra/style.h"
#include "extra/transactiondb.h"
#include "extra/transactiontype.h"
#include "extra/WitnessConfig.h"

#define ASSET_NAME "HX"
#define ACCOUNT_ADDRESS_PREFIX  "HXN"
#define CONTRACT_ADDRESS_PREFIX "HXC"
#define MULTISIG_ADDRESS_PREFIX "HXM"
#define PUBKEY_PREFIX "HX"
#define ASSET_PRECISION 5
#define WALLET_VERSION "1.0.9"           // 版本号
#define AUTO_REFRESH_TIME 5000           // 自动刷新时间(ms)
#define EXCHANGE_CONTRACT_HASH  "c0192642072e9ca233df0fd2aa99ee1c50f7ba17"

#ifdef  TEST_WALLET
#define WALLET_EXE_SUFFIX   "_test"
#else
#define WALLET_EXE_SUFFIX   ""
#endif

#ifdef  WIN32
#define WALLET_EXE_FILE     "HX"WALLET_EXE_SUFFIX".exe"
#else
#define WALLET_EXE_FILE     "./HX" WALLET_EXE_SUFFIX
#endif


#define NODE_RPC_PORT   50320
#define CLIENT_RPC_PORT 50321

class WorkerThreadManager;
class WebSocketManager;

class WitnessConfig;

static QMutex mutexForJsonData;
static QMutex mutexForPending;
static QMutex mutexForConfigFile;

#include <mutex>
static std::mutex infoMutex;

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
    QString guardId;        // "1.5.X" 为空则不是guard
    bool    isFormalGuard = false;

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


typedef QMap<int,unsigned long long>  AssetBalanceMap;
typedef QMap<QString,unsigned long long>     ExchangeContractBalances;
struct AssetInfo
{
    QString id;
    QString symbol;
    QString issuer;
    int precision = 0;
    unsigned long long maxSupply = 0;
    QStringList publishers;
    AssetAmount baseAmount;
    AssetAmount quoteAmount;
    QString currentFeedTime;

    QString multisigAddressId;
    QString hotAddress;     // 热钱包地址
    QString coldAddress;    // 冷钱包地址
    int     effectiveBlock = -1; // 生效区块

    unsigned long long fee = 0;
    unsigned long long withdrawLimit = 0;
};

struct GuardInfo
{
    QString guardId;
    QString accountId;
    QString voteId;
    bool    isFormal = true;
    QString address;
};

struct GuardMultisigAddress
{
    QString multisigAddressObjectId;
    QString hotAddress;     // 热钱包地址
    QString hotPubKey;      // 热钱包公钥
    QString coldAddress;    // 冷钱包地址
    QString coldPubKey;     // 冷钱包公钥
    QString pairId;         // 对应的多签地址id  如果是"2.7.0" 就是正在更新中
};

struct MinerInfo
{
    QString minerId;
    QString accountId;
    QString signingKey;
    int totalMissed = 0;
    int totalProduced = 0;
    int lastBlock = -1;
    double participationRate = 0;
    QVector<AssetAmount>    lockBalances;
};

struct ProposalInfo
{
    QString proposalId;     // 提案id
    QString proposer;       // 发起提案的人
    QString expirationTime; // 失效时间
    QString transactionStr; // 提案的交易
    QStringList    approvedKeys;       // 已投支持票的人
    QStringList    disapprovedKeys;    // 已投反对票的人
    QStringList    requiredAccounts;   // 需要的投票人
    QString type;
    int proposalOperationType = -1;
};

struct TransactionStruct
{
    QString transactionId;
    int     type = -1;
    int     blockNum = -1;
    QString expirationTime;
    QString operationStr;
    unsigned long long feeAmount = 0;
    QString guaranteeId;

public:

    friend QDataStream& operator >>(QDataStream &in,TransactionStruct& data);
    friend QDataStream& operator <<(QDataStream &out,const TransactionStruct& data);
};

struct TransactionTypeId
{
    int type = -2;
    QString transactionId;

public:
    bool operator ==(const TransactionTypeId &_transactionTypeId) const
    {
        return this->transactionId == _transactionTypeId.transactionId;
    }

    friend QDataStream& operator >>(QDataStream &in,TransactionTypeId& data);
    friend QDataStream& operator <<(QDataStream &out,const TransactionTypeId& data);
};

struct GuaranteeOrder
{
    QString id;
    QString ownerAddress;
    QString chainType;
    QString time;
    AssetAmount originAssetAmount;
    AssetAmount targetAssetAmount;
    AssetAmount finishedAssetAmount;
    QStringList records;
    bool finished = false;

public:
    bool operator ==(const GuaranteeOrder &_guaranteeOrder) const
    {
        return this->id == _guaranteeOrder.id;
    }

    friend QDataStream& operator >>(QDataStream &in,GuaranteeOrder& data);
    friend QDataStream& operator <<(QDataStream &out,const GuaranteeOrder& data);
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


//朱正天---手续费
struct FeeChargeInfo
{
    FeeChargeInfo()
    {
        accountRegisterFee = "0";
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

    QString accountRegisterFee;//账号注册手续费
    QString minerIncomeFee;//领取收益手续费
    QString minerForeCloseFee;//质押资产手续费
    QString minerRedeemFee;//赎回资产手续费
    QString tunnelBindFee;//绑定通道账户手续费
    QString poundagePublishFee;//发布承兑单手续费
    QString poundageCancelFee;//撤销承兑单手续费
    QString transferFee;//转账手续费
    QString createCitizenFee;

    QString withDrawFee;//提现手续费--
    QString capitalFee;//资金划转手续费
};

class HXChain : public QObject
{
    Q_OBJECT
public:
    ~HXChain();
    static HXChain*   getInstance();


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
    bool autoWithdrawConfirm = false;
    FeeChargeInfo feeChargeInfo;//手续费情况

    QString currentAccount; // 保存当前账户  切换页面的时候默认选择当前账户
    QString currentSellAssetId;
    QString currentBuyAssetId;

    unsigned long long contractFee;     // 合约单步执行费用
    QString currentContractFee();

    struct TotalContractFee
    {
        unsigned long long baseAmount = 0;
        int step;
    };
    TotalContractFee parseTotalContractFee(QString result);

    void deleteAccountInConfigFile(QString);
    void updateJsonDataMap(QString id, QString data);
    QString jsonDataValue(QString id);
    double getPendingAmount(QString name);
    QString getPendingInfo(QString id);

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
    WitnessConfig* witnessConfig = NULL;
//    void loadAccountInfo();

    QString appDataPath;
    QString walletConfigPath;

    QFile* contactsFile;
    QFile* pendingFile;


    QDialog* currentDialog;  // 如果不为空 则指向当前最前面的不属于frame的dialog
                             // 为的是自动锁定的时候hide该dialog

    Frame* mainFrame = NULL; // 指向主窗口的指针

    bool isUpdateNeeded;
    void SetUpdateNeeded(bool need){isUpdateNeeded = need;}

    int currentPort;          // 当前rpc 端口
    QString localIP;   // 保存 peerinfo 获得的本机IP和端口

    QString middlewarePath;     // 中间件

    QMap<QString,AccountInfo>   accountInfoMap;
    QMap<QString,ExchangeContractBalances>  accountExchangeContractBalancesMap;
    void parseAccountInfo();
    void fetchAccountBalances(QString _accountName);
    void fetchMyContracts();
    bool    isMyAddress(QString _address);
    QString addressToName(QString _address);
    QString getAccountBalance(QString _accountName, QString _assetSymbol);
    QStringList getRegisteredAccounts();
    QStringList getUnregisteredAccounts();
    QString getExchangeContractAddress(QString _accountName);   // 没有兑换合约则返回空  有多个返回第一个
    QString getExchangeContractState(QString _accountName);   // 没有兑换合约则返回空  有多个返回第一个

    QMap<QString,AssetInfo>   assetInfoMap;
    QString getAssetId(QString symbol);
    QStringList getETHAssets();

    QMap<QString,int>   crosschainWithdrawStateMap;
    void fetchCrosschainTransactions();
    void clearCrosschainWithdrawStateMapByState(int state);

private:
    bool isBlockSyncFinish;
public:
    //在bottombar中使用，外部勿用
    void SetBlockSyncFinish(bool fi){std::lock_guard<std::mutex> infoLock(infoMutex); isBlockSyncFinish = fi;}
    bool GetBlockSyncFinish()const{return isBlockSyncFinish;}
    //外部使用
    bool ValidateOnChainOperation();
private:
    void InitFeeCharge();//设置手续费
public:
    void addTrackAddress(QString _address);     // 在chaindata/config.ini中添加 track-address

    void autoSaveWalletFile();      // 自动备份钱包文件

public:
    TransactionDB transactionDB;
    void fetchTransactions();
    void parseTransaction(QString result);
    void checkPendingTransactions();    // 查看pending的交易有没有被确认， 如果过期了就从DB删掉

    QMap<QString,GuardInfo>   allGuardMap;
//    void fetchFormalGuards();
    void fetchAllGuards();
    QStringList getMyFormalGuards();
    QStringList getMyGuards();
    QStringList getFormalGuards();
    QMap<QString,QVector<GuardMultisigAddress>> guardMultisigAddressesMap;  // key是 资产名-guard账户Id 的形式 比如 BTC-1.2.23
    GuardMultisigAddress getGuardMultisigByPairId(QString assetSymbol, QString guardName, QString pairId);      // 从guardMultisigAddressesMap，找到对应的
    void fetchGuardAllMultisigAddresses(QString accountId);
    QStringList getAssetMultisigUpdatedGuards(QString assetSymbol);         // 获取多签地址正在更新的guardId
    QString guardAccountIdToName(QString guardAccountId);
    QString guardAddressToName(QString guardAddress);

    QMap<QString,MinerInfo>     minerMap;
    void fetchMiners();
    QString getMinerNameFromId(QString _minerId);
    QStringList getMyCitizens();

    QMap<QString,ProposalInfo>   senatorProposalInfoMap;        // key是proposal id
    QMap<QString,ProposalInfo>   citizenProposalInfoMap;        // key是proposal id
    void fetchProposals();
    QString citizenAccountIdToName(QString citizenAccountId);


signals:
    void jsonDataUpdated(QString);

    void rpcPosted(QString rpcId, QString rpcCmd);

private:

    HXChain();
    static HXChain* goo;
    QMap<QString,QString> jsonDataMap;   //  各指令的id,各指令的返回

    void getSystemEnvironmentPath();

    class CGarbo // 它的唯一工作就是在析构函数中删除 的实例
    {
    public:
        ~CGarbo()
        {
            if (HXChain::goo)
                delete HXChain::goo;
        }
    };
    static CGarbo Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
};

QString doubleTo5Decimals(double number);
double roundDown(double decimal, int precision = 0);        // 根据精度 向下取"整"
QString removeLastZeros(QString number);        // qstring::number() 对小数的处理有问题  使用std::to_string() 然后把后面的0去掉
QString getBigNumberString(unsigned long long number,int precision);
QString decimalToIntegerStr(QString number, int precision);
void tableWidgetSetItemZebraColor(QTableWidget* w, int alignment = Qt::AlignCenter);
QString toLocalTime(QString timeStr);

enum AddressType
{
    AccountAddress = 0x01,
    ContractAddress = 0x02,
    MultiSigAddress = 0x04,
    PubKey = 0x08,
    InvalidAddress = 0x00
};
Q_DECLARE_FLAGS(AddressFlags, AddressType)
Q_DECLARE_OPERATORS_FOR_FLAGS(AddressFlags)
AddressType   checkAddress(QString address, AddressFlags type = AccountAddress);

void moveWidgetToScreenCenter(QWidget* w);

QString toJsonFormat(QString instruction, QJsonArray parameters);
unsigned long long jsonValueToULL(QJsonValue v);
double jsonValueToDouble(QJsonValue v);

#endif // WALLET_H

