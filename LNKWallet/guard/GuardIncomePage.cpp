#include "GuardIncomePage.h"
#include "ui_GuardIncomePage.h"

#include "ToolButtonWidget.h"
#include "depositpage/FeeChargeWidget.h"
#include "dialog/ErrorResultDialog.h"
#include "dialog/TransactionResultDialog.h"
#include "commondialog.h"

static const int ROWNUMBER = 7;
GuardIncomePage::GuardIncomePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GuardIncomePage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->senatorIncomeTableWidget->installEventFilter(this);
    ui->senatorIncomeTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->senatorIncomeTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->senatorIncomeTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->senatorIncomeTableWidget->setFrameShape(QFrame::NoFrame);
    ui->senatorIncomeTableWidget->setMouseTracking(true);
    ui->senatorIncomeTableWidget->setShowGrid(false);//隐藏表格线

    ui->senatorIncomeTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->senatorIncomeTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->senatorIncomeTableWidget->horizontalHeader()->setVisible(true);
    ui->senatorIncomeTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->senatorIncomeTableWidget->setColumnWidth(0,200);
    ui->senatorIncomeTableWidget->setColumnWidth(1,230);
    ui->senatorIncomeTableWidget->setColumnWidth(2,200);
    ui->senatorIncomeTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->obtainAllBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&GuardIncomePage::pageChangeSlot);

    blankWidget = new BlankDefaultWidget(ui->senatorIncomeTableWidget);
    blankWidget->setTextTip(tr("There are no proposals currently!"));
    init();
}

GuardIncomePage::~GuardIncomePage()
{
    delete ui;
}

void GuardIncomePage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyFormalGuards();
    if(accounts.size() > 0)
    {
        ui->accountComboBox->addItems(accounts);

        if(accounts.contains(HXChain::getInstance()->currentAccount))
        {
            ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
        }
    }
    else
    {
        ui->label->hide();
        ui->accountComboBox->hide();

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->label->pos(), QSize(300,18)));
        label->setText(tr("There are no senator accounts in the wallet."));
    }

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->senatorIncomeTableWidget);

}

void GuardIncomePage::jsonDataUpdated(QString id)
{
    if( id == "GuardIncomePage+get_address_pay_back_balance+" + ui->accountComboBox->currentText())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.value("result").toArray();

        incomeFromCitizenMap.clear();
        foreach (QJsonValue v, array)
        {
            QJsonArray array2 = v.toArray();
            QString citizen = array2.at(0).toString();
            QJsonObject object = array2.at(1).toObject();
            unsigned long long amount = jsonValueToULL( object.value("amount"));
            incomeFromCitizenMap.insert(citizen, amount);
        }

        showIncomes();
        return;
    }

    if( id == "GuardIncomePage+obtain_pay_back_balance")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        qDebug() << id << result;

        if(result.startsWith("\"result\":{"))
        {
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("Transaction of get-income has been sent out!"));
            transactionResultDialog.setDetailText(result);
            transactionResultDialog.pop();

            on_accountComboBox_currentIndexChanged(ui->accountComboBox->currentText());
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setDetailText(result);

            if(result.contains("p_back.second.amount >= min_payback_balance"))
            {
                errorResultDialog.setInfoText(tr("This account's mining income is less than %1 %2 ! You can not get it.")
                                              .arg(500).arg(ASSET_NAME));
            }
            else
            {
                errorResultDialog.setInfoText(tr("Fail to get mining income!"));
            }

            errorResultDialog.pop();
        }

        return;
    }
}

void GuardIncomePage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    fetchSenatorIncomes();
}

void GuardIncomePage::fetchSenatorIncomes()
{
    if(ui->accountComboBox->currentText().isEmpty())    return;

    AccountInfo info = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
    HXChain::getInstance()->postRPC( "GuardIncomePage+get_address_pay_back_balance+" + ui->accountComboBox->currentText(),
                                     toJsonFormat( "get_address_pay_back_balance",
                                                   QJsonArray() << info.address << ASSET_NAME));

}

void GuardIncomePage::showIncomes()
{
    QStringList citizens = incomeFromCitizenMap.keys();
    int size = citizens.size();
    ui->senatorIncomeTableWidget->setRowCount(size + 1);

    totalAmount = 0;
    for(int i = 0; i < size; i++)
    {
        ui->senatorIncomeTableWidget->setRowHeight(i,40);

        ui->senatorIncomeTableWidget->setItem(i,0, new QTableWidgetItem(citizens.at(i)));

        unsigned long long amount = incomeFromCitizenMap.value(citizens.at(i));
        QString amountStr = getBigNumberString( amount, ASSET_PRECISION);
        ui->senatorIncomeTableWidget->setItem(i,1, new QTableWidgetItem(amountStr + " " + ASSET_NAME));

        ui->senatorIncomeTableWidget->setItem(i,2, new QTableWidgetItem(tr("obtain")));

        ToolButtonWidget *toolButton = new ToolButtonWidget();
        toolButton->setText(ui->senatorIncomeTableWidget->item(i,2)->text());
        ui->senatorIncomeTableWidget->setCellWidget(i,2,toolButton);
        connect(toolButton,&ToolButtonWidget::clicked,std::bind(&GuardIncomePage::on_senatorIncomeTableWidget_cellClicked,this,i,2));

        totalAmount += amount;
    }
    ui->totalIncomeLabel->setText( getBigNumberString( totalAmount, ASSET_PRECISION) + " " + "HX");

    tableWidgetSetItemZebraColor(ui->senatorIncomeTableWidget);

    int page = (ui->senatorIncomeTableWidget->rowCount()%ROWNUMBER==0 && ui->senatorIncomeTableWidget->rowCount() != 0) ?
                ui->senatorIncomeTableWidget->rowCount()/ROWNUMBER : ui->senatorIncomeTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->senatorIncomeTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(0);

    pageWidget->setVisible(0 != ui->senatorIncomeTableWidget->rowCount());
    blankWidget->setVisible(ui->senatorIncomeTableWidget->rowCount() == 0);

}

void GuardIncomePage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void GuardIncomePage::on_senatorIncomeTableWidget_cellClicked(int row, int column)
{
    if(column == 2)
    {
        if(ui->senatorIncomeTableWidget->item(row,column)->text().isEmpty())       return;
        if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

        QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;

//        FeeChargeWidget *feeCharge = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.minerIncomeFee.toDouble(),
//                                                         HXChain::getInstance()->feeType,ui->accountComboBox->currentText(),
//                                                         HXChain::getInstance()->mainFrame);
//        connect(feeCharge,&FeeChargeWidget::confirmSignal,[this,address,row](){

        CommonDialog commonDialog(CommonDialog::YesOrNo);
        commonDialog.setText(tr("You need to pay %1 %2 for this operation. Continue?").arg(HXChain::getInstance()->feeChargeInfo.minerIncomeFee).arg(ASSET_NAME));
        if(commonDialog.pop())
        {
            QJsonArray array;
            QJsonArray array2;
            QString citizen = ui->senatorIncomeTableWidget->item(row,0)->text();
            array2 << citizen;
            QJsonObject object;
            object.insert("amount", QString::number(incomeFromCitizenMap.value(citizen)));
            object.insert("asset_id", HXChain::getInstance()->getAssetId(ASSET_NAME));
            array2 << object;
            array << array2;
            HXChain::getInstance()->postRPC( "GuardIncomePage+obtain_pay_back_balance",
                                             toJsonFormat( "obtain_pay_back_balance",
                                                           QJsonArray() << address
                                                           << array
                                                           << true ));
        }
//        });
//        feeCharge->show();

        return;
    }
}


void GuardIncomePage::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->senatorIncomeTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->senatorIncomeTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->senatorIncomeTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->senatorIncomeTableWidget->setRowHidden(i,true);
        }
    }
}

void GuardIncomePage::on_obtainAllBtn_clicked()
{
    if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

    QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;

//    FeeChargeWidget *feeCharge = new FeeChargeWidget(HXChain::getInstance()->feeChargeInfo.minerIncomeFee.toDouble(),
//                                                     HXChain::getInstance()->feeType,ui->accountComboBox->currentText(),
//                                                     HXChain::getInstance()->mainFrame);
//    connect(feeCharge,&FeeChargeWidget::confirmSignal,[this,address](){

    CommonDialog commonDialog(CommonDialog::YesOrNo);
    commonDialog.setText(tr("You need to pay %1 %2 for this operation. Continue?").arg(HXChain::getInstance()->feeChargeInfo.minerIncomeFee).arg(ASSET_NAME));
    if(commonDialog.pop())
    {
        QJsonArray array;
        QStringList citizens = incomeFromCitizenMap.keys();
        for(int i = 0; i < citizens.size(); i++)
        {
            QJsonArray array2;
            QString citizen = citizens.at(i);
            array2 << citizen;
            QJsonObject object;
            object.insert("amount", QString::number(incomeFromCitizenMap.value(citizen)));
            object.insert("asset_id", HXChain::getInstance()->getAssetId(ASSET_NAME));
            array2 << object;
            array << array2;
        }

        HXChain::getInstance()->postRPC( "GuardIncomePage+obtain_pay_back_balance",
                                         toJsonFormat( "obtain_pay_back_balance",
                                                       QJsonArray() << address
                                                       << array
                                                       << true ));
    }
//    });
//    feeCharge->show();
}
