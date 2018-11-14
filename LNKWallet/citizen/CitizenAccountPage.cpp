#include "CitizenAccountPage.h"
#include "ui_CitizenAccountPage.h"

#include "wallet.h"
#include "control/AssetIconItem.h"
#include "CreateCitizenDialog.h"
#include "commondialog.h"
#include "ChangePayBackDialog.h"
#include "poundage/PageScrollWidget.h"

static const int ROWNUMBER = 7;
CitizenAccountPage::CitizenAccountPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CitizenAccountPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->lockBalanceTableWidget->installEventFilter(this);
    ui->lockBalanceTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->lockBalanceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->lockBalanceTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->lockBalanceTableWidget->setFrameShape(QFrame::NoFrame);
    ui->lockBalanceTableWidget->setMouseTracking(true);
    ui->lockBalanceTableWidget->setShowGrid(false);//隐藏表格线

    ui->lockBalanceTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->lockBalanceTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->lockBalanceTableWidget->horizontalHeader()->setVisible(true);
    ui->lockBalanceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->lockBalanceTableWidget->setColumnWidth(0,140);
    ui->lockBalanceTableWidget->setColumnWidth(1,490);
    ui->lockBalanceTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->newCitizenBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->startMineBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->changeFeeBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->lockBalanceTableWidget);


    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&CitizenAccountPage::pageChangeSlot);

    init();
}

CitizenAccountPage::~CitizenAccountPage()
{
    delete ui;
}

void CitizenAccountPage::init()
{
    inited = false;
    HXChain::getInstance()->fetchCitizenPayBack();

    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyCitizens();
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
        ui->idLabel->hide();
        ui->idLabel2->hide();

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->label->pos(), QSize(300,30)));
        label->setText(tr("There are no citizen accounts in the wallet."));
        label->setStyleSheet("QLabel{color: rgb(137,129,161);font: 11px \"Microsoft YaHei UI Light\";}");
    }

    inited = true;

    on_accountComboBox_currentIndexChanged(ui->accountComboBox->currentText());
}

void CitizenAccountPage::refresh()
{
    init();
}

void CitizenAccountPage::jsonDataUpdated(QString id)
{
    if( id == "CitizenAccountPage+dump_private_key+" + ui->accountComboBox->currentText())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.take("result").toArray();
        if(array.size() > 0)
        {
            QJsonArray array2 = array.at(0).toArray();
            QString privateKey = array2.at(1).toString();
            AccountInfo accountInfo = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
            HXChain::getInstance()->witnessConfig->addPrivateKey( accountInfo.pubKey, privateKey);
            HXChain::getInstance()->witnessConfig->setProductionEnabled(true);

            MinerInfo minerInfo = HXChain::getInstance()->minerMap.value(ui->accountComboBox->currentText());
            HXChain::getInstance()->witnessConfig->addMiner(minerInfo.minerId);
            HXChain::getInstance()->witnessConfig->save();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Mining configuration has been written. This citizen account will start mining when the wallet is launched next time."));
            commonDialog.pop();
        }
    }
}

void CitizenAccountPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void CitizenAccountPage::showLockBalance()
{
    MinerInfo minerInfo = HXChain::getInstance()->minerMap.value(ui->accountComboBox->currentText());
    int size = minerInfo.lockBalances.size();
    ui->lockBalanceTableWidget->setRowCount(0);
    ui->lockBalanceTableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        AssetAmount aa = minerInfo.lockBalances.at(i);
        AssetInfo assetInfo = HXChain::getInstance()->assetInfoMap.value(aa.assetId);
        ui->lockBalanceTableWidget->setItem( i, 0, new QTableWidgetItem( assetInfo.symbol));
        ui->lockBalanceTableWidget->setItem( i, 1, new QTableWidgetItem( getBigNumberString(aa.amount, assetInfo.precision)));

        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->lockBalanceTableWidget->item(i,0)->text());
        ui->lockBalanceTableWidget->setCellWidget(i, 0, assetIconItem);
    }

    int page = (ui->lockBalanceTableWidget->rowCount()%ROWNUMBER==0 && ui->lockBalanceTableWidget->rowCount() != 0) ?
                ui->lockBalanceTableWidget->rowCount()/ROWNUMBER : ui->lockBalanceTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->lockBalanceTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(0);

    pageWidget->setVisible(0 != ui->lockBalanceTableWidget->rowCount());

    tableWidgetSetItemZebraColor(ui->lockBalanceTableWidget);

}

void CitizenAccountPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited || ui->accountComboBox->currentText().isEmpty())  return;
    HXChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    MinerInfo minerInfo = HXChain::getInstance()->minerMap.value(ui->accountComboBox->currentText());
    ui->idLabel->setText(minerInfo.minerId);
    ui->totalProducedLabel->setText(QString::number(minerInfo.totalProduced));
    ui->lastBlockLabel->setText(QString::number(minerInfo.lastBlock));

    if(!minerInfo.minerId.isEmpty())
    {
        if(HXChain::getInstance()->witnessConfig->isProductionEnabled() && HXChain::getInstance()->witnessConfig->getMiners().contains(minerInfo.minerId))
        {
            ui->startMineBtn->hide();
        }
        else
        {
            ui->startMineBtn->show();
        }
    }
    else
    {
        ui->startMineBtn->hide();
    }

    showLockBalance();
}

void CitizenAccountPage::on_newCitizenBtn_clicked()
{
    CreateCitizenDialog createCitizenDialog;
    createCitizenDialog.pop();
}

void CitizenAccountPage::on_startMineBtn_clicked()
{
    CommonDialog commonDialog(CommonDialog::OkAndCancel);
    commonDialog.setText(tr("Sure to open the mining function of this citizen account?"));
    if(commonDialog.pop())
    {
        HXChain::getInstance()->postRPC( "CitizenAccountPage+dump_private_key+" + ui->accountComboBox->currentText(),
                                         toJsonFormat( "dump_private_key", QJsonArray() << ui->accountComboBox->currentText() ));

    }
}

void CitizenAccountPage::on_changeFeeBtn_clicked()
{
    if(!ui->accountComboBox->currentText().isEmpty())
    {
        ChangePayBackDialog dia(ui->accountComboBox->currentText());
        dia.exec();
    }


}

void CitizenAccountPage::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->lockBalanceTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->lockBalanceTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->lockBalanceTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->lockBalanceTableWidget->setRowHidden(i,true);
        }
    }

}
