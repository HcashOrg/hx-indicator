#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T14:17:06
#
#-------------------------------------------------

QT       += core gui xml network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UBWallet
TEMPLATE = app

DEFINES += TEST_WALLET

win32{
    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01

    LIBS += -lDbgHelp
    LIBS += User32.Lib
    LIBS += -limm32
    LIBS += -L$$PWD/leveldb  -lShLwApi
}
macx{
#    ICON = ub.icns
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
    accountpage.cpp \
    bottombar.cpp \
    setdialog.cpp \
    lockpage.cpp \
    consoledialog.cpp \
    titlebar.cpp \
    contactdialog.cpp \
    waitingforsync.cpp \
    remarkdialog.cpp \
    functionbar.cpp \
    contactpage.cpp \
    singlecontactwidget.cpp \
    commondialog.cpp \
    showcontentdialog.cpp \
    namedialog.cpp \
    deleteaccountdialog.cpp \
    commontip.cpp \
    searchoptionwidget.cpp \
    addnodedialog.cpp \
    editremarkdialog.cpp \
    addcontactdialog.cpp \
    exportdialog.cpp \
    importdialog.cpp \
    ipcellwidget.cpp \
    control/myprogressbar.cpp \
    control/showbottombarwidget.cpp \
    control/rightclickmenudialog.cpp \
    control/chooseaddaccountdialog.cpp \
    dialog/renamedialog.cpp \
    control/remarkcellwidget.cpp \
    control/shadowwidget.cpp \
    selectwalletpathwidget.cpp \
    dialog/choosetokenoperationdialog.cpp \
    control/mycheckbtn.cpp \
    dialog/choosecontractoperationdialog.cpp \
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
    control/contacteditdialog.cpp \
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
    exchange/exchangewidget.cpp \
    depositpage/DepositPage.cpp \
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
    FeeChooseWidget.cpp


HEADERS  += firstlogin.h \
    normallogin.h \
    frame.h \
    mainpage.h \
    accountpage.h \
    bottombar.h \
    setdialog.h \
    lockpage.h \
    consoledialog.h \
    titlebar.h \
    contactdialog.h \
    waitingforsync.h \
    remarkdialog.h \
    functionbar.h \
    contactpage.h \
    singlecontactwidget.h \
    commondialog.h \
    showcontentdialog.h \
    namedialog.h \
    deleteaccountdialog.h \
    commontip.h \
    searchoptionwidget.h \
    addnodedialog.h \
    editremarkdialog.h \
    addcontactdialog.h \
    exportdialog.h \
    importdialog.h \
    ipcellwidget.h \
    control/myprogressbar.h \
    control/showbottombarwidget.h \
    control/rightclickmenudialog.h \
    control/chooseaddaccountdialog.h \
    dialog/renamedialog.h \
    control/remarkcellwidget.h \
    control/shadowwidget.h \
    selectwalletpathwidget.h \
    dialog/choosetokenoperationdialog.h \
    control/mycheckbtn.h \
    dialog/choosecontractoperationdialog.h \
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
    control/contacteditdialog.h \
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
    exchange/exchangewidget.h   \
    depositpage/DepositPage.h \
    exchange/myexchangecontractpage.h   \
    depositpage/DepositPage.h \
    depositpage/DepositDataUtil.h \
    depositpage/DepositQrcodeWidget.h \
    depositpage/DepositRecrdWideget.h\
    exchange/exchangewidget.h \
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
    FeeChooseWidget.h

FORMS    += firstlogin.ui \
    normallogin.ui \
    mainpage.ui \
    accountpage.ui \
    bottombar.ui \
    setdialog.ui \
    lockpage.ui \
    consoledialog.ui \
    titlebar.ui \
    contactdialog.ui \
    waitingforsync.ui \
    remarkdialog.ui \
    functionbar.ui \
    contactpage.ui \
    singlecontactwidget.ui \
    commondialog.ui \
    showcontentdialog.ui \
    namedialog.ui \
    deleteaccountdialog.ui \
    commontip.ui \
    searchoptionwidget.ui \
    addnodedialog.ui \
    editremarkdialog.ui \
    addcontactdialog.ui \
    exportdialog.ui \
    importdialog.ui \
    ipcellwidget.ui \
    control/rightclickmenudialog.ui \
    control/chooseaddaccountdialog.ui \
    dialog/renamedialog.ui \
    control/remarkcellwidget.ui \
    control/shadowwidget.ui \
    selectwalletpathwidget.ui \
    dialog/choosetokenoperationdialog.ui \
    dialog/choosecontractoperationdialog.ui \
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
    control/contacteditdialog.ui \
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
    exchange/exchangewidget.ui  \
    depositpage/DepositPage.ui \
    exchange/myexchangecontractpage.ui  \
    depositpage/DepositPage.ui \
    depositpage/DepositQrcodeWidget.ui \
    depositpage/DepositRecrdWideget.ui\
    exchange/exchangewidget.ui \
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
    FeeChooseWidget.ui

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
    LNK.ico \
    .gitignore \
    LICENSE \
    wallet_English.ts \
    wallet_simplified_Chinese.ts

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
