#ifndef TRANSACTIONTYPE_H
#define TRANSACTIONTYPE_H

// 不同类型交易对应的operation类型
#define TRANSACTION_TYPE_NORMAL         0       // 普通交易
#define TRANSACTION_TYPE_DEPOSIT        60      // 充值交易（多签转link-BTC）
#define TRANSACTION_TYPE_WITHDRAW       61      // 提现交易
#define TRANSACTION_TYPE_MINE_INCOME    73      // 质押挖矿收入


#endif // TRANSACTIONTYPE_H
