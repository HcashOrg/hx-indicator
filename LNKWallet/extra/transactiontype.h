#ifndef TRANSACTIONTYPE_H
#define TRANSACTIONTYPE_H

// 不同类型交易对应的operation类型
#define TRANSACTION_TYPE_NORMAL                 0       // 普通交易
#define TRANSACTION_TYPE_REGISTER_ACCOUNT       5       // 注册账户
#define TRANSACTION_TYPE_BIND_TUNNEL            10      // 绑定tunnel地址
#define TRANSACTION_TYPE_UNBIND_TUNNEL          11      // 解绑tunnel地址
#define TRANSACTION_TYPE_LOCKBALANCE            55      // 质押资产给miner
#define TRANSACTION_TYPE_FORECLOSE              56      // 赎回质押资产
#define TRANSACTION_TYPE_DEPOSIT                60      // 充值交易（多签转link-BTC）
#define TRANSACTION_TYPE_WITHDRAW               61      // 提现交易
#define TRANSACTION_TYPE_MINE_INCOME            73      // 质押挖矿收入
#define TRANSACTION_TYPE_CONTRACT_REGISTER      76      // 注册合约
#define TRANSACTION_TYPE_CONTRACT_INVOKE        79      // 调用合约
#define TRANSACTION_TYPE_CONTRACT_TRANSFER      81      // 转账到合约
#define TRANSACTION_TYPE_CREATE_GUARANTEE       82      // 创建承兑单
#define TRANSACTION_TYPE_CANCEL_GUARANTEE       83      // 撤销承兑单

#endif // TRANSACTIONTYPE_H
