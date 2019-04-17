#ifndef TRANSACTIONTYPE_H
#define TRANSACTIONTYPE_H

// 不同类型交易对应的operation类型
#define TRANSACTION_TYPE_NORMAL                 0       // 普通交易
#define TRANSACTION_TYPE_REGISTER_ACCOUNT       5       // 注册账户
#define TRANSACTION_TYPE_UPDATE_ACCOUNT         6       // 更新账户信息（挖矿管理费）
#define TRANSACTION_TYPE_BIND_TUNNEL            10      // 绑定tunnel地址
#define TRANSACTION_TYPE_UNBIND_TUNNEL          11      // 解绑tunnel地址
#define TRANSACTION_TYPE_UPDATE_ASSET_PK        12      // guard更新冷热钱包多签地址私钥
#define TRANSACTION_TYPE_FEED_PRICE             23      // 喂价
#define TRANSACTION_TYPE_CREATE_MINER           24      // 成为citizen
#define TRANSACTION_TYPE_SPONSOR_PROPOSAL       28      // 发起提案
#define TRANSACTION_TYPE_PROPOSAL_APPROVE       29      // 提案投票
#define TRANSACTION_TYPE_CREATE_GUARD           35      // 创建senator
#define TRANSACTION_TYPE_FORMAL_GUARD           36      // 成为正式guard
#define TRANSACTION_TYPE_RESIGN_GUARD           38      // 使guard辞职
#define TRANSACTION_TYPE_LOCKBALANCE            55      // 投票资产给miner
#define TRANSACTION_TYPE_FORECLOSE              56      // 赎回投票资产
#define TRANSACTION_TYPE_SENATOR_LOCK_BALANCE   57      // senator交保证金
#define TRANSACTION_TYPE_CANCEL_WITHDRAW        59      // 取消提现
#define TRANSACTION_TYPE_DEPOSIT                60      // 充值交易（多签转HX-BTC）
#define TRANSACTION_TYPE_WITHDRAW               61      // 提现交易
#define TRANSACTION_TYPE_WITHDRAW_CROSSCHAIN    62
#define TRANSACTION_TYPE_WITHDRAW_SIGN          63      // 提现交易签名
#define TRANSACTION_TYPE_WITHDRAW_FINAL         64      // 提现交易final
#define TRANSACTION_TYPE_COLDHOT                66      // 冷热钱包转账
#define TRANSACTION_TYPE_COLDHOT_CROSSCHAIN     67
#define TRANSACTION_TYPE_COLDHOT_SIGN           68      // 冷热钱包转账签名
#define TRANSACTION_TYPE_COLDHOT_FINAL          69      // 冷热钱包转账final
#define TRANSACTION_TYPE_COLDHOT_CANCEL         72      // guard取消冷热钱包互转提案
#define TRANSACTION_TYPE_MINE_INCOME            73      // 投票挖矿收入
#define TRANSACTION_TYPE_CHANGE_ASSET_ACCOUNT   74      // 变更冷热钱包多签地址 （只在提案的子op中）
#define TRANSACTION_TYPE_ISSUE_ASSET            75      // 发行资产
#define TRANSACTION_TYPE_CONTRACT_REGISTER      76      // 注册合约
#define TRANSACTION_TYPE_CONTRACT_INVOKE        79      // 调用合约
#define TRANSACTION_TYPE_CONTRACT_TRANSFER      81      // 转账到合约
#define TRANSACTION_TYPE_PROPOSAL_CONTRACT_TRANSFER_FEE  82      // 修改合约转出手续费提案
#define TRANSACTION_TYPE_CREATE_GUARANTEE       83      // 创建承兑单
#define TRANSACTION_TYPE_CANCEL_GUARANTEE       84      // 撤销承兑单
#define TRANSACTION_TYPE_WITHDRAW_CANCEL        85      // guard取消提现提案
#define TRANSACTION_TYPE_SET_PUBLISHER          86      // 设置喂价人
#define TRANSACTION_TYPE_CROSSCHAIN_FEE         87      // 设置跨链提现费用
#define TRANSACTION_TYPE_OBTAIN_BONUS           88      // 领取分红
#define TRANSACTION_TYPE_SET_LOCKBALANCE        89      // 设置senator的保证金
#define TRANSACTION_TYPE_SIGN_ETH_MULTI_CREATE  97      // senator签名创建以太多签账户 senator_sign_eths_multi_account_create_trx
#define TRANSACTION_TYPE_SIGN_ETH_FINAL         98      // senator签名以太交易 senator_sign_eths_final_trx
#define TRANSACTION_TYPE_SIGN_ETH_COLDHOT_FINAL 100     // senator签名以太冷热钱包转账交易 senator_sign_eths_final_trx
#define TRANSACTION_TYPE_CITIZEN_PROPOSAL       101     // citizen发起提案
#define TRANSACTION_TYPE_CITIZEN_CHANGE_SENATOR 102     // citizen发起更换senator的提案
#define TRANSACTION_TYPE_BLACKLIST              105     // 黑名单提案
#define TRANSACTION_TYPE_PROPOSAL_CHANGE_GAS    115     // 改变eth gas提案

#endif // TRANSACTIONTYPE_H
