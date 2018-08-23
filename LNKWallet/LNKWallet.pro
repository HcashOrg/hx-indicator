#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T14:17:06
#
#-------------------------------------------------

QT       += core gui xml network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HXIndicator
TEMPLATE = app

DEFINES += TEST_WALLET

win32{
    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01

    LIBS += -lDbgHelp
    LIBS += User32.Lib
    LIBS += -limm32
    LIBS += -lShLwApi
}
macx{
    ICON = HX.icns
    QMAKE_MAC_SDK = macosx10.12
}
LIBS += -L$$PWD -lqrencode

LIBS += -L$$PWD/leveldb -lleveldb
INCLUDEPATH += $$PWD/leveldb/include/



SOURCES += main.cpp\
        firstlogin.cpp \
    normallogin.cpp \
    frame.cpp \
    mainpage.cpp \
    bottombar.cpp \
    setdialog.cpp \
    lockpage.cpp \
    consoledialog.cpp \
    titlebar.cpp \
    waitingforsync.cpp \
    commondialog.cpp \
    showcontentdialog.cpp \
    namedialog.cpp \
    exportdialog.cpp \
    importdialog.cpp \
    control/myprogressbar.cpp \
    control/shadowwidget.cpp \
    selectwalletpathwidget.cpp \
    control/mycheckbtn.cpp \
    control/qrcodedialog.cpp \
    control/qrcodewidget.cpp \
    multisig/multisigpage.cpp \
    multisig/addmultisigdialog.cpp \
    multisig/choosemultisigoperationdialog.cpp \
    multisig/multisiginfodialog.cpp \
    multisig/createmultisigdialog.cpp \
    multisig/addownerdialog.cpp \
    multisig/withdrawmultisigdialog.cpp \
    multisig/signmultisigdialog.cpp \
    multisig/multisigtransactionpage.cpp \
    dialog/checkpwddialog.cpp \
    AES/aes.cpp \
    AES/aesencryptor.cpp \
    dialog/exportsetpwddialog.cpp \
    dialog/importenterpwddialog.cpp \
    dialog/functionbarchoicedialog.cpp \
    extra/guiutil.cpp \
    control/feedassetcellwidget.cpp \
    neworimportwalletwidget.cpp \
    websocketmanager.cpp \
    dialog/backupwalletdialog.cpp \
    wallet.cpp \
    contact/ContactAddWidget.cpp \
    contact/ContactBriefWidget.cpp \
    contact/ContactDataUtil.cpp \
    contact/ContactInfoTitleWidget.cpp \
    contact/ContactInfoWidget.cpp \
    contact/ContactSearchWidget.cpp \
    contact/ContactTreeWidget.cpp \
    contact/ContactWidget.cpp \
    functionBar/FunctionAccountWidget.cpp \
    functionBar/FunctionAdvanceWidget.cpp \
    functionBar/FunctionWidget.cpp \
    poundage/PoundageWidget.cpp \
    poundage/PublishPoundageWidget.cpp \
    poundage/PoundageDataUtil.cpp \
    miner/minerpage.cpp \
    miner/registerdialog.cpp \
    miner/locktominerdialog.cpp \
    miner/foreclosedialog.cpp   \
    poundage/PoundageShowWidget.cpp \
    poundage/PoundageShowTableModel.cpp \
    poundage/GeneralComboBoxDelegate.cpp \
    poundage/PageScrollWidget.cpp \
    depositpage/DepositPage.cpp \
    depositpage/DepositDataUtil.cpp \
    depositpage/DepositQrcodeWidget.cpp \
    depositpage/DepositRecrdWideget.cpp\
    exchange/myexchangecontractpage.cpp \
    withdrawpage/WithdrawPage.cpp \
    withdrawpage/WithdrawDataUtil.cpp \
    withdrawpage/WithdrawInputWidget.cpp \
    exchange/selldialog.cpp \
    exchange/depositexchangecontractdialog.cpp \
    exchange/withdrawexchangecontractdialog.cpp \
    withdrawpage/WithdrawConfirmWidget.cpp \
    capitalTransferPage/CapitalTransferPage.cpp \
    capitalTransferPage/PasswordConfirmWidget.cpp \
    capitalTransferPage/CapitalTransferDataUtil.cpp \
    contact/ContactInfoHistoryWidget.cpp \
    extra/transactiondb.cpp         \
    AccountManagerWidget.cpp \
    ToolButtonWidget.cpp \
    HelpWidget.cpp \
    transfer/transferconfirmdialog.cpp \
    transfer/transferpage.cpp \
    transfer/transferrecordwidget.cpp \
    withdrawpage/withdrawrecordwidget.cpp \
    functionBar/FunctionExchangeWidget.cpp \
    transfer/ContactChooseWidget.cpp \
    transfer/BlurWidget.cpp \
    extra/HttpManager.cpp \
    exchange/OnchainOrderPage.cpp \
    FeeChooseWidget.cpp \
    exchange/BuyOrderWidget.cpp \
    depositpage/DepositAutomatic.cpp \
    notifyWidget/MessageNotifyWidget.cpp \
    exchange/contractbalancewidget.cpp  \
    depositpage/FeeChargeWidget.cpp \
    exchange/WithdrawOrderDialog.cpp \
    capitalTransferPage/CapitalConfirmWidget.cpp \
    alltransactionwidget.cpp \
    KeyDataUtil.cpp \
    update/UpdateNetWork.cpp \
    update/UpdateProcess.cpp \
    update/UpdateProgressUtil.cpp \
    dialog/ErrorResultDialog.cpp \
    dialog/TransactionResultDialog.cpp \
    crossmark/crosscapitalmark.cpp \
    crossmark/AddressValidate.cpp \
    control/AssetIconItem.cpp \
    control/BlankDefaultWidget.cpp \
    control/CustomShadowEffect.cpp \
    guard/AssetPage.cpp \
    functionBar/FunctionGuardWidget.cpp \
    guard/AssetChangeHistoryWidget.cpp \
    guard/GuardKeyManagePage.cpp \
    guard/GuardKeyUpdatingInfoDialog.cpp \
    guard/ProposalPage.cpp \
    guard/ProposalDetailDialog.cpp \
    guard/WithdrawConfirmPage.cpp \
    guard/FeedPricePage.cpp \
    control/AccountInfoWidget.cpp \
    guard/FeedPriceDialog.cpp \
    guard/ColdHotTransferPage.cpp \
    guard/ColdHotTransferDialog.cpp \
    guard/GuardAccountPage.cpp \
    guard/GuardIncomePage.cpp \
    guard/ChangeCrosschainAddressDialog.cpp \
    guard/WithdrawInfoDialog.cpp \
    guard/ColdHotInfoDialog.cpp \
    bonus/BonusPage.cpp \
    dapp/ContractTokenPage.cpp \
    dapp/CreateTokenDialog.cpp \
    dapp/AddTokenDialog.cpp \
    dapp/TokenTransferWidget.cpp \
    dapp/TokenHistoryWidget.cpp \
    functionBar/FunctionCitizenWidget.cpp \
    citizen/CitizenAccountPage.cpp \
    citizen/CitizenProposalPage.cpp \
    citizen/CreateCitizenDialog.cpp \
    extra/WitnessConfig.cpp


HEADERS  += firstlogin.h \
    normallogin.h \
    frame.h \
    mainpage.h \
    bottombar.h \
    setdialog.h \
    lockpage.h \
    consoledialog.h \
    titlebar.h \
    waitingforsync.h \
    commondialog.h \
    showcontentdialog.h \
    namedialog.h \
    exportdialog.h \
    importdialog.h \
    control/myprogressbar.h \
    control/shadowwidget.h \
    selectwalletpathwidget.h \
    control/mycheckbtn.h \
    control/qrcodedialog.h \
    control/qrcodewidget.h \
    control/qrencode.h \
    multisig/multisigpage.h \
    multisig/addmultisigdialog.h \
    multisig/choosemultisigoperationdialog.h \
    multisig/multisiginfodialog.h \
    multisig/createmultisigdialog.h \
    multisig/addownerdialog.h \
    multisig/withdrawmultisigdialog.h \
    multisig/signmultisigdialog.h \
    multisig/multisigtransactionpage.h \
    dialog/checkpwddialog.h \
    AES/aes.h \
    AES/aesencryptor.h \
    dialog/exportsetpwddialog.h \
    dialog/importenterpwddialog.h \
    dialog/functionbarchoicedialog.h \
    extra/style.h \
    extra/guiutil.h \
    control/feedassetcellwidget.h \
    neworimportwalletwidget.h \
    websocketmanager.h \
    dialog/backupwalletdialog.h \
    wallet.h \
    contact/ContactAddWidget.h \
    contact/ContactBriefWidget.h \
    contact/ContactDataUtil.h \
    contact/ContactInfoTitleWidget.h \
    contact/ContactInfoWidget.h \
    contact/ContactSearchWidget.h \
    contact/ContactTreeWidget.h \
    contact/ContactWidget.h \
    functionBar/FunctionAccountWidget.h \
    functionBar/FunctionAdvanceWidget.h \
    functionBar/FunctionWidget.h \
    poundage/PoundageWidget.h \
    poundage/PublishPoundageWidget.h \
    poundage/PoundageDataUtil.h \
    miner/minerpage.h \
    miner/registerdialog.h \
    miner/locktominerdialog.h \
    miner/foreclosedialog.h \
    poundage/PoundageShowWidget.h \
    poundage/PoundageShowTableModel.h \
    poundage/GeneralComboBoxDelegate.h \
    poundage/PageScrollWidget.h \
    depositpage/DepositPage.h \
    exchange/myexchangecontractpage.h   \
    depositpage/DepositPage.h \
    depositpage/DepositDataUtil.h \
    depositpage/DepositQrcodeWidget.h \
    depositpage/DepositRecrdWideget.h\
    withdrawpage/WithdrawPage.h \
    withdrawpage/WithdrawDataUtil.h \
    withdrawpage/WithdrawInputWidget.h \
    exchange/selldialog.h \
    exchange/depositexchangecontractdialog.h \
    exchange/withdrawexchangecontractdialog.h \
    withdrawpage/WithdrawConfirmWidget.h \
    capitalTransferPage/CapitalTransferPage.h \
    capitalTransferPage/PasswordConfirmWidget.h \
    capitalTransferPage/CapitalTransferDataUtil.h \
    contact/ContactInfoHistoryWidget.h \
    extra/transactiondb.h \
    AccountManagerWidget.h \
    ToolButtonWidget.h \
    HelpWidget.h \
    transfer/transferconfirmdialog.h \
    transfer/transferpage.h \
    transfer/transferrecordwidget.h \
    extra/transactiontype.h \
    withdrawpage/withdrawrecordwidget.h \
    functionBar/FunctionExchangeWidget.h \
    transfer/ContactChooseWidget.h \
    transfer/BlurWidget.h \
    extra/HttpManager.h \
    exchange/OnchainOrderPage.h \
    FeeChooseWidget.h \
    exchange/BuyOrderWidget.h \
    depositpage/DepositAutomatic.h \
    notifyWidget/MessageNotifyWidget.h \
    exchange/contractbalancewidget.h \
    depositpage/FeeChargeWidget.h \
    exchange/WithdrawOrderDialog.h \
    capitalTransferPage/CapitalConfirmWidget.h \
    alltransactionwidget.h \
    KeyDataUtil.h \
    update/UpdateNetWork.h \
    update/UpdateProcess.h \
    update/UpdateProgressUtil.h \
    dialog/ErrorResultDialog.h \
    dialog/TransactionResultDialog.h \
    crossmark/crosscapitalmark.h \
    crossmark/AddressValidate.h \
    control/AssetIconItem.h \
    control/BlankDefaultWidget.h \
    control/CustomShadowEffect.h \
    guard/AssetPage.h \
    functionBar/FunctionGuardWidget.h \
    guard/AssetChangeHistoryWidget.h \
    guard/GuardKeyManagePage.h \
    guard/GuardKeyUpdatingInfoDialog.h \
    guard/ProposalPage.h \
    guard/ProposalDetailDialog.h \
    guard/WithdrawConfirmPage.h \
    guard/FeedPricePage.h \
    control/AccountInfoWidget.h \
    guard/FeedPriceDialog.h \
    guard/ColdHotTransferPage.h \
    guard/ColdHotTransferDialog.h \
    guard/GuardAccountPage.h \
    guard/GuardIncomePage.h \
    guard/ChangeCrosschainAddressDialog.h \
    guard/WithdrawInfoDialog.h \
    guard/ColdHotInfoDialog.h \
    bonus/BonusPage.h \
    dapp/ContractTokenPage.h \
    dapp/CreateTokenDialog.h \
    dapp/AddTokenDialog.h \
    dapp/TokenTransferWidget.h \
    dapp/TokenHistoryWidget.h \
    functionBar/FunctionCitizenWidget.h \
    citizen/CitizenAccountPage.h \
    citizen/CitizenProposalPage.h \
    citizen/CreateCitizenDialog.h \
    extra/WitnessConfig.h

FORMS    += firstlogin.ui \
    normallogin.ui \
    mainpage.ui \
    bottombar.ui \
    setdialog.ui \
    lockpage.ui \
    consoledialog.ui \
    titlebar.ui \
    waitingforsync.ui \
    commondialog.ui \
    showcontentdialog.ui \
    namedialog.ui \
    exportdialog.ui \
    importdialog.ui \
    control/shadowwidget.ui \
    selectwalletpathwidget.ui \
    control/qrcodedialog.ui \
    multisig/multisigpage.ui \
    multisig/addmultisigdialog.ui \
    multisig/choosemultisigoperationdialog.ui \
    multisig/multisiginfodialog.ui \
    multisig/createmultisigdialog.ui \
    multisig/addownerdialog.ui \
    multisig/withdrawmultisigdialog.ui \
    multisig/signmultisigdialog.ui \
    multisig/multisigtransactionpage.ui \
    dialog/checkpwddialog.ui \
    dialog/exportsetpwddialog.ui \
    dialog/importenterpwddialog.ui \
    dialog/functionbarchoicedialog.ui \
    control/feedassetcellwidget.ui \
    neworimportwalletwidget.ui \
    dialog/backupwalletdialog.ui \
    contact/ContactAddWidget.ui \
    contact/ContactBriefWidget.ui \
    contact/ContactInfoTitleWidget.ui \
    contact/ContactInfoWidget.ui \
    contact/ContactSearchWidget.ui \
    contact/ContactWidget.ui \
    functionBar/FunctionAccountWidget.ui \
    functionBar/FunctionAdvanceWidget.ui \
    functionBar/FunctionWidget.ui \
    contact/ContactTreeWidget.ui \
    poundage/PoundageWidget.ui \
    poundage/PublishPoundageWidget.ui \
    miner/minerpage.ui \
    miner/registerdialog.ui \
    miner/locktominerdialog.ui \
    miner/foreclosedialog.ui    \
    poundage/PoundageShowWidget.ui \
    poundage/PageScrollWidget.ui \
    depositpage/DepositPage.ui \
    exchange/myexchangecontractpage.ui  \
    depositpage/DepositPage.ui \
    depositpage/DepositQrcodeWidget.ui \
    depositpage/DepositRecrdWideget.ui\
    withdrawpage/WithdrawPage.ui \
    withdrawpage/WithdrawInputWidget.ui \
    exchange/selldialog.ui \
    exchange/depositexchangecontractdialog.ui \
    exchange/withdrawexchangecontractdialog.ui \
    withdrawpage/WithdrawConfirmWidget.ui \
    capitalTransferPage/CapitalTransferPage.ui \
    capitalTransferPage/PasswordConfirmWidget.ui \
    contact/ContactInfoHistoryWidget.ui \
    AccountManagerWidget.ui \
    ToolButtonWidget.ui \
    HelpWidget.ui \
    transfer/transferconfirmdialog.ui \
    transfer/transferpage.ui \
    transfer/transferrecordwidget.ui \
    withdrawpage/withdrawrecordwidget.ui \
    functionBar/FunctionExchangeWidget.ui \
    transfer/ContactChooseWidget.ui \
    transfer/BlurWidget.ui \
    exchange/OnchainOrderPage.ui \
    FeeChooseWidget.ui \
    exchange/BuyOrderWidget.ui \
    notifyWidget/MessageNotifyWidget.ui \
    exchange/contractbalancewidget.ui \
    depositpage/FeeChargeWidget.ui \
    exchange/WithdrawOrderDialog.ui \
    capitalTransferPage/CapitalConfirmWidget.ui \
    alltransactionwidget.ui \
    dialog/ErrorResultDialog.ui \
    dialog/TransactionResultDialog.ui \
    control/AssetIconItem.ui \
    control/BlankDefaultWidget.ui \
    guard/AssetPage.ui \
    functionBar/FunctionGuardWidget.ui \
    guard/AssetChangeHistoryWidget.ui \
    guard/GuardKeyManagePage.ui \
    guard/GuardKeyUpdatingInfoDialog.ui \
    guard/ProposalPage.ui \
    guard/ProposalDetailDialog.ui \
    guard/WithdrawConfirmPage.ui \
    guard/FeedPricePage.ui \
    control/AccountInfoWidget.ui \
    guard/FeedPriceDialog.ui \
    guard/ColdHotTransferPage.ui \
    guard/ColdHotTransferDialog.ui \
    guard/GuardAccountPage.ui \
    guard/GuardIncomePage.ui \
    guard/ChangeCrosschainAddressDialog.ui \
    guard/WithdrawInfoDialog.ui \
    guard/ColdHotInfoDialog.ui \
    bonus/BonusPage.ui \
    dapp/ContractTokenPage.ui \
    dapp/CreateTokenDialog.ui \
    dapp/AddTokenDialog.ui \
    dapp/TokenTransferWidget.ui \
    dapp/TokenHistoryWidget.ui \
    functionBar/FunctionCitizenWidget.ui \
    citizen/CitizenAccountPage.ui \
    citizen/CitizenProposalPage.ui \
    citizen/CreateCitizenDialog.ui

win32{
    DISTFILES += logo.rc
    RC_FILE = logo.rc
}

RESOURCES += \
    wallet.qrc \
    contact/resources.qrc \
    functionBar/functionbar.qrc

TRANSLATIONS +=   wallet_simplified_Chinese.ts  wallet_English.ts

DISTFILES += \
    contact/search.png \
    leveldb/leveldb.lib \
    leveldb/ShLwApi.Lib \
    libqrencode.a \
    qrencode.lib \
    pic2/Thumbs.db \
    pic2/copyBtn.png \
    HX.ico \
    .gitignore \
    LICENSE \
    wallet_English.ts \
    wallet_simplified_Chinese.ts

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
