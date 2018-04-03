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
    transferpage.cpp \
    bottombar.cpp \
    setdialog.cpp \
    lockpage.cpp \
    consoledialog.cpp \
    titlebar.cpp \
    debug_log.cpp \
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
    transferconfirmdialog.cpp \
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
    extra/dynamicmove.cpp \
    control/remarkcellwidget.cpp \
    control/shadowwidget.cpp \
    assetpage.cpp \
    selectwalletpathwidget.cpp \
    dialog/addtokendialog.cpp \
    feedpage.cpp \
    dialog/feedpricedialog.cpp \
    dialog/choosetokenoperationdialog.cpp \
    dialog/depositdialog.cpp \
    dialog/destroydialog.cpp \
    dialog/mintdialog.cpp \
    dialog/withdrawcontractbalancedialog.cpp \
    dialog/createtokendialog.cpp \
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
    extra/transactioindb.cpp \
    AES/aes.cpp \
    AES/aesencryptor.cpp \
    dialog/exportsetpwddialog.cpp \
    dialog/importenterpwddialog.cpp \
    contract/contractdetailwidget.cpp \
    contract/singlecontractwidget.cpp \
    contract/smartcontractpage.cpp \
    control/contacteditdialog.cpp \
    dialog/functionbarchoicedialog.cpp \
    extra/guiutil.cpp \
    extra/contractserverinfomanager.cpp \
    control/feedassetcellwidget.cpp \
    lnk.cpp \
    neworimportwalletwidget.cpp \
    websocketmanager.cpp \
    dialog/backupwalletdialog.cpp \
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
    functionBar/FunctionWidget.cpp

HEADERS  += firstlogin.h \
    normallogin.h \
    frame.h \
    mainpage.h \
    accountpage.h \
    transferpage.h \
    bottombar.h \
    setdialog.h \
    lockpage.h \
    consoledialog.h \
    debug_log.h \
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
    transferconfirmdialog.h \
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
    extra/dynamicmove.h \
    control/remarkcellwidget.h \
    control/shadowwidget.h \
    assetpage.h \
    selectwalletpathwidget.h \
    dialog/addtokendialog.h \
    feedpage.h \
    dialog/feedpricedialog.h \
    dialog/choosetokenoperationdialog.h \
    dialog/depositdialog.h \
    dialog/destroydialog.h \
    dialog/mintdialog.h \
    dialog/withdrawcontractbalancedialog.h \
    dialog/createtokendialog.h \
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
    extra/transactioindb.h \
    AES/aes.h \
    AES/aesencryptor.h \
    dialog/exportsetpwddialog.h \
    dialog/importenterpwddialog.h \
    contract/contractdetailwidget.h \
    contract/singlecontractwidget.h \
    contract/smartcontractpage.h \
    control/contacteditdialog.h \
    dialog/functionbarchoicedialog.h \
    extra/style.h \
    extra/guiutil.h \
    extra/contractserverinfomanager.h \
    control/feedassetcellwidget.h \
    lnk.h \
    neworimportwalletwidget.h \
    websocketmanager.h \
    dialog/backupwalletdialog.h \
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
    functionBar/FunctionWidget.h

FORMS    += firstlogin.ui \
    normallogin.ui \
    mainpage.ui \
    accountpage.ui \
    transferpage.ui \
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
    transferconfirmdialog.ui \
    commontip.ui \
    searchoptionwidget.ui \
    addnodedialog.ui \
    editremarkdialog.ui \
    addcontactdialog.ui \
    exportdialog.ui \
    importdialog.ui \
    ipcellwidget.ui \
    control/accountdetailwidget.ui \
    control/rightclickmenudialog.ui \
    control/chooseaddaccountdialog.ui \
    dialog/renamedialog.ui \
    control/remarkcellwidget.ui \
    control/shadowwidget.ui \
    assetpage.ui \
    selectwalletpathwidget.ui \
    dialog/addtokendialog.ui \
    feedpage.ui \
    dialog/feedpricedialog.ui \
    dialog/choosetokenoperationdialog.ui \
    dialog/depositdialog.ui \
    dialog/destroydialog.ui \
    dialog/mintdialog.ui \
    dialog/withdrawcontractbalancedialog.ui \
    dialog/createtokendialog.ui \
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
    contract/contractdetailwidget.ui \
    contract/singlecontractwidget.ui \
    contract/smartcontractpage.ui \
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
    contact/ContactTreeWidget.ui
    
win32{
    DISTFILES += logo.rc
    RC_FILE = logo.rc
}

RESOURCES += \
    wallet.qrc \
    contact/resources.qrc

TRANSLATIONS +=   wallet_simplified_Chinese.ts  wallet_English.ts

DISTFILES += \
    contact/search.png

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
