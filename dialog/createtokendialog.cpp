#include "createtokendialog.h"
#include "ui_createtokendialog.h"

#include "lnk.h"
#include "commondialog.h"
#include "checkpwddialog.h"

#ifdef WIN32
#include "Windows.h"
#endif

#include <QDebug>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QListView>

CreateTokenDialog::CreateTokenDialog(QWidget *parent) :
    QDialog(parent),
    contractType(0),
    ui(new Ui::CreateTokenDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->containerWidget->installEventFilter(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->accountComboBox->setView(new QListView());
    ui->accountComboBox2->setView(new QListView());
    ui->anchorTokenComboBox->setView(new QListView());

    QRegExp rx1("[a-zA-Z0-9]{0,20}");
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->contractNameLineEdit->setValidator(pReg1);
    ui->contractNameLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->contractNameLineEdit2->setValidator(pReg1);
    ui->contractNameLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);


    ui->totalSupplyLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->totalSupplyLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);

    setTotalSupplyValidator();


    QRegExp rx3("^([1-9][0-9]{0,8})?$|(^\\t?$)");
    QRegExpValidator *pReg3 = new QRegExpValidator(rx3, this);
    ui->anchorRatioLineEdit->setValidator(pReg3);
    ui->anchorRatioLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->okBtn2->setStyleSheet(OKBTN_STYLE);
    ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);
    ui->issueBtn->setStyleSheet(OKBTN_STYLE);
    ui->issueBtn2->setStyleSheet(OKBTN_STYLE);
    ui->backBtn->setStyleSheet(CANCELBTN_STYLE);
    ui->backBtn2->setStyleSheet(CANCELBTN_STYLE);

    init();
}

CreateTokenDialog::~CreateTokenDialog()
{
    delete ui;
}

void CreateTokenDialog::pop()
{
    move(0,0);
    exec();
}

void CreateTokenDialog::onNoMortgageLabelSelected()
{
    currentChoice = 0;

    noMortgageLabel->setStyleSheet("border-image: url(:/ui/wallet_ui/createContractType_selected.png);");
    mortgageLabel->setStyleSheet("border-image: url(:/ui/wallet_ui/createContractType_unselected.png);");
}

void CreateTokenDialog::onMortgageLabelSelected()
{
    currentChoice = 1;

    noMortgageLabel->setStyleSheet("border-image: url(:/ui/wallet_ui/createContractType_unselected.png);");
    mortgageLabel->setStyleSheet("border-image: url(:/ui/wallet_ui/createContractType_selected.png);");

}

void CreateTokenDialog::jsonDataUpdated(QString id)
{

    if( id ==  "id_check_normal_token_name_contract_get_info" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        ui->issueBtn->setEnabled(true);

        if( result.startsWith( "\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("This contract name has been used! Please use another name."));
            commonDialog.pop();
        }
        else
        {

            CheckPwdDialog checkPwdDialog;
            if(!checkPwdDialog.pop())   return;

            UBChain::getInstance()->postRPC( QString("id_contract_register"),
                                             toJsonFormat( "contract_register",
                                                           QStringList() << ui->accountComboBox->currentText() << "contract/token.gpc"
                                                           << ASSET_NAME << "5" ));

            contractType = 1;

            ui->stackedWidget->setCurrentIndex(3);
            ui->waitingInfoLabel->setText(tr("Contract registering ..."));
        }

        return;
    }

    if( id ==  "id_check_smart_token_name_contract_get_info" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        ui->issueBtn2->setEnabled(true);

        if( result.startsWith( "\"result\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("This contract name has been used! Please use another name."));
            commonDialog.pop();
        }
        else
        {
            CheckPwdDialog checkPwdDialog;
            if(!checkPwdDialog.pop())   return;

            UBChain::getInstance()->postRPC( QString("id_contract_register"),
                                             toJsonFormat( "contract_register",
                                                           QStringList() << ui->accountComboBox2->currentText() << "contract/any_mortgage_token.gpc"
                                                           << ASSET_NAME << "5" ));

            contractType = 2;

            ui->stackedWidget->setCurrentIndex(3);
            ui->waitingInfoLabel->setText(tr("Contract registering ..."));
        }

        return;
    }



    if( id ==  "id_contract_register" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
qDebug() << id  << result;
        if( result.startsWith( "\"result\":"))
        {
            contractAddress = result.mid(9);
            contractAddress.remove("\"");

//            UBChain::getInstance()->ERC20TokenInfoMap[contractAddress].contractAddress = contractAddress;
//            UBChain::getInstance()->configFile->setValue("/AddedContractToken/" + contractAddress,1);

            timerForRegister->start(5000);

        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            ui->waitingInfoLabel->setText(tr("register failed: ") + errorMessage);
            ui->okBtn2->show();
        }

        return;
    }

    if( id ==  "id_contract_upgrade" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id  << result;
        if( result.startsWith( "\"result\":"))
        {
            timerForUpgrade->start(1000);
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            ui->waitingInfoLabel->setText(tr("upgrade failed: ") + errorMessage);
            ui->okBtn2->show();
        }

        return;
    }

    if( id ==  "id_contract_call-init_token-nomal" )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
qDebug() << id  << result;
        if( result.startsWith( "\"result\":"))
        {
            timerForInitToken->start(1000);

        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            ui->waitingInfoLabel->setText(tr("init_token failed: ") + errorMessage);
            ui->okBtn2->show();
        }

        return;
    }


    if( id == "id_createtoken_contract_get_info+" + contractAddress)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
qDebug() << id  << result;
        if( result.startsWith( "\"result\":"))
        {
            timerForRegister->stop();

            if(contractType == 1)
            {
                QString contractName = ui->contractNameLineEdit->text();
                QString description  = ui->contractDescriptionLineEdit->text();

                UBChain::getInstance()->postRPC( QString("id_contract_upgrade"),
                                                 toJsonFormat( "contract_upgrade",
                                                               QStringList() << contractAddress << ui->accountComboBox->currentText()
                                                               << contractName << description
                                                               << ASSET_NAME << "5" ));
                ui->waitingInfoLabel->setText(tr("Contract registering ...\n"
                                                 "Contract already registered.\n"
                                                 "Registered address: %1\n"
                                                 "Upgrading contract...").arg(contractAddress)
                                              );
            }
            else if(contractType == 2)
            {
                QString contractName = ui->contractNameLineEdit2->text();
                QString description  = ui->contractDescriptionLineEdit2->text();

                UBChain::getInstance()->postRPC( QString("id_contract_upgrade"),
                                                 toJsonFormat( "contract_upgrade",
                                                               QStringList() << contractAddress << ui->accountComboBox2->currentText()
                                                               << contractName << description
                                                               << ASSET_NAME << "5" ));
                ui->waitingInfoLabel->setText(tr("Contract registering ...\n"
                                                 "Contract already registered.\n"
                                                 "Registered address: %1\n"
                                                 "Upgrading contract...").arg(contractAddress)
                                              );
            }


        }
        return;
    }

}


void CreateTokenDialog::on_backBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void CreateTokenDialog::on_backBtn2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void CreateTokenDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->balanceLabel->setText( getBigNumberString(UBChain::getInstance()->accountBalanceMap.value(arg1).value(0),ASSET_PRECISION) + " UB");
}

void CreateTokenDialog::on_accountComboBox2_currentIndexChanged(const QString &arg1)
{
    ui->balanceLabel2->setText( getBigNumberString(UBChain::getInstance()->accountBalanceMap.value(arg1).value(0),ASSET_PRECISION) + " UB");
}

void CreateTokenDialog::init()
{
    ui->stackedWidget->setCurrentIndex(0);

    QStringList keys = UBChain::getInstance()->addressMap.keys();
    ui->accountComboBox->addItems( keys);
    ui->accountComboBox2->addItems( keys);

    QStringList anchorTokens = UBChain::getInstance()->tokenPriceMap.keys();
    ui->anchorTokenComboBox->addItems( anchorTokens);

    ui->gasLineEdit->setReadOnly(true);
    ui->okBtn2->hide();

    timerForRegister = new QTimer(this);
    connect(timerForRegister,SIGNAL(timeout()),this,SLOT(onTimerForRegister()));

    timerForUpgrade = new QTimer(this);
    connect(timerForUpgrade,SIGNAL(timeout()),this,SLOT(onTimerForUpgrade()));

    timerForInitToken = new QTimer(this);
    connect(timerForInitToken,SIGNAL(timeout()),this,SLOT(onTimerForInitToken()));


    noMortgageLabel = new GUIUtil::ClickableLabel;
    noMortgageLabel->setParent(ui->chooseTypePage);
    connect(noMortgageLabel,SIGNAL(clicked(QPoint)),this,SLOT(onNoMortgageLabelSelected()));
    noMortgageLabel->setGeometry(50,60,190,120);

    mortgageLabel = new GUIUtil::ClickableLabel;
    mortgageLabel->setParent(ui->chooseTypePage);
    connect(mortgageLabel,SIGNAL(clicked(QPoint)),this,SLOT(onMortgageLabelSelected()));
    mortgageLabel->setGeometry(270,60,190,120);

    onNoMortgageLabelSelected();
}

void CreateTokenDialog::estimateGas()
{
//    if( ui->stackedWidget->currentIndex() == 1)
//    {
//        UBChain::getInstance()->postRPC( toJsonFormat( QString("id_contract_register_testing-normal"), "contract_register_testing",
//                                                       QStringList() << ui->accountComboBox->currentText() << "contract/token.gpc"
//                                                       << ASSET_NAME  ));
    //    }
}

QString CreateTokenDialog::intToPrecisionString(int precision)
{
    QString result = "1";
    while (precision > 0)
    {
        result.append("0");
        precision--;
    }

    return result;
}

QString CreateTokenDialog::addPrecisionString(QString supply, int precision)
{
    QString result = supply;
    while (precision > 0)
    {
        result.append("0");
        precision--;
    }

    return result;
}

QString CreateTokenDialog::getContractParamsString(QStringList params)
{
    QString result;
    foreach (QString param, params)
    {
        result += param;
        result += ",";
    }
    if( result.endsWith(","))
    {
        result.chop(1);
    }

    return result;
}

void CreateTokenDialog::setTotalSupplyValidator()
{
    int precisionNum = ui->precisionSpinBox->text().toInt();

    QRegExp rx(QString("^([1-9][0-9]{0,%1})?$|(^\\t?$)").arg(17 - precisionNum));
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->totalSupplyLineEdit->setValidator(validator);


    int precisionNum2 = ui->precisionSpinBox2->text().toInt();

    QRegExp rx2(QString("^([1-9][0-9]{0,%1})?$|(^\\t?$)").arg(17 - precisionNum2));
    QRegExpValidator *validator2 = new QRegExpValidator(rx2, this);
    ui->totalSupplyLineEdit2->setValidator(validator2);
}


void CreateTokenDialog::on_issueBtn_clicked()
{
    if( ui->contractNameLineEdit->text().simplified().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Contract name can not be empty!"));
        commonDialog.pop();
        return;
    }

    if( ui->contractDescriptionLineEdit->text().simplified().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Contract description can not be empty!"));
        commonDialog.pop();
        return;
    }

    if( ui->totalSupplyLineEdit->text().simplified().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Total supply can not be empty!"));
        commonDialog.pop();
        return;
    }

    if( UBChain::getInstance()->accountBalanceMap.value(ui->accountComboBox->currentText()).value(0) < 20 * ASSET_PRECISION)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("You don't have enough %1s!").arg(ASSET_NAME));
        commonDialog.pop();
        return;
    }

    // 先校验合约名能不能用
    UBChain::getInstance()->postRPC( "id_check_normal_token_name_contract_get_info",
                                     toJsonFormat( "contract_get_info",
                                                   QStringList() << ui->contractNameLineEdit->text()));

    ui->issueBtn->setEnabled(false);

}


void CreateTokenDialog::on_issueBtn2_clicked()
{
    if( ui->contractNameLineEdit2->text().simplified().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Contract name can not be empty!"));
        commonDialog.pop();
        return;
    }

    if( ui->contractDescriptionLineEdit2->text().simplified().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Contract description can not be empty!"));
        commonDialog.pop();
        return;
    }

    if( ui->totalSupplyLineEdit2->text().simplified().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Total supply can not be empty!"));
        commonDialog.pop();
        return;
    }

    if( ui->anchorTokenComboBox->currentText().isEmpty())
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("No token to anchor to now!"));
        commonDialog.pop();
        return;
    }

    if( UBChain::getInstance()->accountBalanceMap.value(ui->accountComboBox2->currentText()).value(0) < 20 * ASSET_PRECISION)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("You don't have enough %1s!").arg(ASSET_NAME));
        commonDialog.pop();
        return;
    }

    // 先校验合约名能不能用
    UBChain::getInstance()->postRPC( "id_check_smart_token_name_contract_get_info",
                                     toJsonFormat( "contract_get_info",
                                                   QStringList() << ui->contractNameLineEdit2->text()));

    ui->issueBtn2->setEnabled(false);


}


void CreateTokenDialog::on_okBtn2_clicked()
{
    close();
}

void CreateTokenDialog::onTimerForRegister()
{
    UBChain::getInstance()->postRPC( "id_createtoken_contract_get_info+" + contractAddress,
                                     toJsonFormat( "contract_get_info",
                                                   QStringList() << contractAddress
                                                   ));


}

void CreateTokenDialog::onTimerForUpgrade()
{
    UBChain::getInstance()->postRPC( "id_contract_get_info+" + contractAddress,
                                     toJsonFormat( "contract_get_info",
                                                   QStringList() << contractAddress
                                                   ));


    if(contractAddress.isEmpty())   return;
    if(!UBChain::getInstance()->ERC20TokenInfoMap.contains(contractAddress))    return;


    if( UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress).level == "forever")
    {
        timerForUpgrade->stop();

        if(contractType == 1)
        {
            QString name = ui->contractNameLineEdit->text();
            QString symbol = ui->contractNameLineEdit->text();
            QString totalSupply = addPrecisionString(ui->totalSupplyLineEdit->text(),ui->precisionSpinBox->text().toInt());
            QString precision  = intToPrecisionString(ui->precisionSpinBox->text().toInt());
            QString paramString = getContractParamsString(QStringList() << name << symbol << totalSupply << precision);

            UBChain::getInstance()->postRPC( QString("id_contract_call-init_token-nomal"),
                                             toJsonFormat( "contract_call",
                                                           QStringList() << contractAddress << ui->accountComboBox->currentText()
                                                           << "init_token" << paramString
                                                           << ASSET_NAME << "5" ));
            ui->waitingInfoLabel->setText(tr("Contract registering ...\n"
                                             "Contract already registered.\n"
                                             "Registered address: %1\n"
                                             "Upgrading contract...\n"
                                             "Contract upgraded.\n"
                                             "Initing contract...").arg(contractAddress)
                                          );
        }
        else if(contractType == 2)
        {
            QString name = ui->contractNameLineEdit2->text();
            QString symbol = ui->contractNameLineEdit2->text();
            QString totalSupply = addPrecisionString(ui->totalSupplyLineEdit2->text(),ui->precisionSpinBox2->text().toInt());
            QString precision  = intToPrecisionString(ui->precisionSpinBox2->text().toInt());
            QString anchorToken = ui->anchorTokenComboBox->currentText();
            QString anchorRatio = addPrecisionString(ui->anchorRatioLineEdit->text(),5);
            QString paramString = getContractParamsString(QStringList() << name << totalSupply << precision << anchorToken << anchorRatio);

            UBChain::getInstance()->postRPC( QString("id_contract_call-init_token-nomal"),
                                             toJsonFormat( "contract_call",
                                                           QStringList() << contractAddress << ui->accountComboBox2->currentText()
                                                           << "init_token" << paramString
                                                           << ASSET_NAME << "5" ));
            ui->waitingInfoLabel->setText(tr("Contract registering ...\n"
                                             "Contract already registered.\n"
                                             "Registered address: %1\n"
                                             "Upgrading contract...\n"
                                             "Contract upgraded.\n"
                                             "Initing contract...").arg(contractAddress)
                                          );
        }


    }
}


void CreateTokenDialog::onTimerForInitToken()
{
    UBChain::getInstance()->postRPC( "id_contract_get_info+" + contractAddress,
                                     toJsonFormat( "contract_get_info",
                                                   QStringList() << contractAddress
                                                   ));


    if(contractAddress.isEmpty())   return;
    if(!UBChain::getInstance()->ERC20TokenInfoMap.contains(contractAddress))    return;


    if( UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress).level == "forever")
    {
        if( UBChain::getInstance()->ERC20TokenInfoMap.value(contractAddress).state != "NOT_INITED")
        {
            timerForInitToken->stop();

            ui->waitingInfoLabel->setText(tr("Contract registering ...\n"
                                             "Contract already registered.\n"
                                             "Registered address: %1\n"
                                             "Upgrading contract...\n"
                                             "Contract upgraded.\n"
                                             "Initing contract...\n"
                                             "Contract inited\n"
                                             "Contract created successfully!").arg(contractAddress)
                                          );
            ui->okBtn2->show();

            UBChain::getInstance()->addConcernedContract(contractAddress);
        }
    }
}




void CreateTokenDialog::on_precisionSpinBox_valueChanged(const QString &arg1)
{
    ui->totalSupplyLineEdit->clear();
    setTotalSupplyValidator();
}

void CreateTokenDialog::on_precisionSpinBox2_valueChanged(const QString &arg1)
{
    ui->totalSupplyLineEdit2->clear();
    setTotalSupplyValidator();
}

void CreateTokenDialog::on_okBtn_clicked()
{
    if(currentChoice == 0)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    else if(currentChoice == 1)
    {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void CreateTokenDialog::on_closeBtn_clicked()
{
    close();
}
