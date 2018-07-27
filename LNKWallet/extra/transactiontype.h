#ifndef TRANSACTIONTYPE_H
#define TRANSACTIONTYPE_H

// 不同类型交易对应的operation类型
#define TRANSACTION_TYPE_NORMAL                 0       // 普通交易
#define TRANSACTION_TYPE_REGISTER_ACCOUNT       5       // 注册账户
#define TRANSACTION_TYPE_BIND_TUNNEL            10      // 绑定tunnel地址
#define TRANSACTION_TYPE_UNBIND_TUNNEL          11      // 解绑tunnel地址
#define TRANSACTION_TYPE_UPDATE_ASSET_PK        12      // guard更新冷热钱包多签地址私钥
#define TRANSACTION_TYPE_FEED_PRICE             23      // 喂价
#define TRANSACTION_TYPE_SPONSOR_PROPOSAL       28      // 发起提案
#define TRANSACTION_TYPE_PROPOSAL_APPROVE       29      // 提案投票
#define TRANSACTION_TYPE_CREATE_GUARD           36      // 创建guard
#define TRANSACTION_TYPE_LOCKBALANCE            55      // 质押资产给miner
#define TRANSACTION_TYPE_FORECLOSE              56      // 赎回质押资产
#define TRANSACTION_TYPE_DEPOSIT                60      // 充值交易（多签转link-BTC）
#define TRANSACTION_TYPE_WITHDRAW               61      // 提现交易
#define TRANSACTION_TYPE_WITHDRAW_CROSSCHAIN    62
#define TRANSACTION_TYPE_WITHDRAW_SIGN          63      // 提现交易签名
#define TRANSACTION_TYPE_COLDHOT                66      // 冷热钱包转账
#define TRANSACTION_TYPE_COLDHOT_CROSSCHAIN     67
#define TRANSACTION_TYPE_COLDHOT_SIGN           68      // 冷热钱包转账签名
#define TRANSACTION_TYPE_COLDHOT_CANCEL         72      // guard取消冷热钱包互转提案
#define TRANSACTION_TYPE_MINE_INCOME            73      // 质押挖矿收入
#define TRANSACTION_TYPE_CHANGE_ASSET_ACCOUNT   74      // 变更冷热钱包多签地址 （只在提案的子op中）
#define TRANSACTION_TYPE_ISSUE_ASSET            75      // 发行资产
#define TRANSACTION_TYPE_CONTRACT_REGISTER      76      // 注册合约
#define TRANSACTION_TYPE_CONTRACT_INVOKE        79      // 调用合约
#define TRANSACTION_TYPE_CONTRACT_TRANSFER      81      // 转账到合约
#define TRANSACTION_TYPE_CREATE_GUARANTEE       82      // 创建承兑单
#define TRANSACTION_TYPE_CANCEL_GUARANTEE       83      // 撤销承兑单
#define TRANSACTION_TYPE_WITHDRAW_CANCEL        84      // guard取消提现提案
#define TRANSACTION_TYPE_SET_PUBLISHER          85      // 设置喂价人
#define TRANSACTION_TYPE_OBTAIN_BONUS           88      // 领取分红

#endif // TRANSACTIONTYPE_H
