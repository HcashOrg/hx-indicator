#include "OnchainOrderPage.h"
#include "ui_OnchainOrderPage.h"

#include "wallet.h"
#include "commondialog.h"
#include "BuyOrderWidget.h"
#include "ToolButtonWidget.h"
#include "control/BlankDefaultWidget.h"
#include "poundage/PageScrollWidget.h"
#include "ExchangeModeWidget.h"

#include <QtMath>

static const int ROWNUMBER = 6;
OnchainOrderPage::OnchainOrderPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnchainOrderPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->ordersTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->ordersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ordersTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->ordersTableWidget->setFrameShape(QFrame::NoFrame);
    ui->ordersTableWidget->setMouseTracking(true);
    ui->ordersTableWidget->setShowGrid(false);//隐藏表格线
    ui->ordersTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->ordersTableWidget->horizontalHeader()->setFixedHeight(35);
    ui->ordersTableWidget->horizontalHeader()->setVisible(true);
    ui->ordersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ordersTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->ordersTableWidget->verticalHeader()->setVisible(false);
    ui->ordersTableWidget->setColumnWidth(0,190);
    ui->ordersTableWidget->setColumnWidth(1,180);
    ui->ordersTableWidget->setColumnWidth(2,130);
    ui->ordersTableWidget->setColumnWidth(3,90);

    ui->ordersTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    ui->favoriteBtn->setCheckable(true);

    pageWidget = new PageScrollWidget();
    ui->stackedWidget->addWidget(pageWidget);
    connect(pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&OnchainOrderPage::pageChangeSlot);

    blankWidget = new BlankDefaultWidget(ui->ordersTableWidget);
    blankWidget->setTextTip(tr("There are no orders!"));

    favoritePairsWidget = new FavoritePairsWidget(6,this);
    connect(favoritePairsWidget, SIGNAL(showPair(QString)), this, SLOT(showPair(QString)));
    favoritePairsWidget->move(60,120);
    favoritePairsWidget->init();

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->ordersTableWidget);
    init();
}

OnchainOrderPage::~OnchainOrderPage()
{
    delete ui;
}

void OnchainOrderPage::init()
{
    QStringList accounts = HXChain::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(HXChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(HXChain::getInstance()->currentAccount);
    }

    QStringList assetIds = HXChain::getInstance()->assetInfoMap.keys();
    foreach (QString assetId, assetIds)
    {
        ui->assetComboBox->addItem( revertERCSymbol( HXChain::getInstance()->assetInfoMap.value(assetId).symbol), assetId);
        ui->assetComboBox2->addItem( revertERCSymbol( HXChain::getInstance()->assetInfoMap.value(assetId).symbol), assetId);
    }

    if(HXChain::getInstance()->currentSellAssetId.isEmpty() || HXChain::getInstance()->currentBuyAssetId.isEmpty())
    {
        HXChain::getInstance()->configFile->beginGroup("/FavoriteOrderPairs");
        QStringList keys = HXChain::getInstance()->configFile->childKeys();
        HXChain::getInstance()->configFile->endGroup();
        if(keys.size() > 0)
        {
            QStringList strList = keys.at(0).split("+");
            ui->assetComboBox->setCurrentText( revertERCSymbol( strList.at(0)));
            ui->assetComboBox2->setCurrentText( revertERCSymbol( strList.at(1)));
        }
    }
    else
    {
        if(assetIds.contains(HXChain::getInstance()->currentSellAssetId))
        {
            ui->assetComboBox->setCurrentText( revertERCSymbol( HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->currentSellAssetId).symbol));
        }

        if(assetIds.contains(HXChain::getInstance()->currentBuyAssetId))
        {
            ui->assetComboBox2->setCurrentText( revertERCSymbol( HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->currentBuyAssetId).symbol));
        }
    }



    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    inited = true;

    on_accountComboBox_currentIndexChanged(ui->accountComboBox->currentText());

    ui->exchangeModeBtn->hide();
}

void OnchainOrderPage::onBack()
{
    if(currentWidget)
    {
        currentWidget->close();
        currentWidget = NULL;
    }
}

void OnchainOrderPage::jsonDataUpdated(QString id)
{

}

void OnchainOrderPage::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    QJsonArray  arr   = object.take("result").toObject().take("data").toArray();

    QJsonArray  array = sortArray(arr,false);
    int size = array.size();
    ui->ordersTableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        ui->ordersTableWidget->setRowHeight(i,40);

        QJsonObject dataObject = array.at(i).toObject();
        QString contractAddress = dataObject.take("contract_address").toString();
        unsigned long long sellAmount = jsonValueToULL(dataObject.take("from_supply"));
        QString sellSymbol = dataObject.take("from_asset").toString();
        unsigned long long buyAmount = jsonValueToULL(dataObject.take("to_supply"));
        QString buySymbol = dataObject.take("to_asset").toString();
        int state = dataObject.take("state").toInt();

        AssetInfo sellAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(sellSymbol));
        AssetInfo buyAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(buySymbol));

        ui->ordersTableWidget->setItem(i,0, new QTableWidgetItem(getBigNumberString(sellAmount,sellAssetInfo.precision)));
        ui->ordersTableWidget->item(i,0)->setData(Qt::UserRole,sellAmount);

        ui->ordersTableWidget->setItem(i,1, new QTableWidgetItem(getBigNumberString(buyAmount,buyAssetInfo.precision)));
        ui->ordersTableWidget->item(i,1)->setData(Qt::UserRole,buyAmount);

        double price = (double)sellAmount / qPow(10,sellAssetInfo.precision) / buyAmount * qPow(10,buyAssetInfo.precision);
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(price,'g',8));
        item->setData(Qt::UserRole,contractAddress);
        ui->ordersTableWidget->setItem(i,2,item);

        ui->ordersTableWidget->setItem(i,3, new QTableWidgetItem(tr("buy")));

        for(int j = 3;j < 4;++j)
        {
            ToolButtonWidgetItem *toolButtonItem = new ToolButtonWidgetItem(i,j);

            if(HXChain::getInstance()->getExchangeContractAddress(ui->accountComboBox->currentText()) == contractAddress)
            {
                toolButtonItem->setEnabled(false);
                toolButtonItem->setText(tr("my order"));
                toolButtonItem->setButtonFixSize(80,20);
            }
            else
            {
                toolButtonItem->setText(ui->ordersTableWidget->item(i,j)->text());          
            }

            ui->ordersTableWidget->setCellWidget(i,j,toolButtonItem);
            connect(toolButtonItem,SIGNAL(itemClicked(int,int)),this,SLOT(onItemClicked(int,int)));
        }
    }
    tableWidgetSetItemZebraColor(ui->ordersTableWidget);

    int page = (ui->ordersTableWidget->rowCount()%ROWNUMBER==0 && ui->ordersTableWidget->rowCount() != 0) ?
                ui->ordersTableWidget->rowCount()/ROWNUMBER : ui->ordersTableWidget->rowCount()/ROWNUMBER+1;
    pageWidget->SetTotalPage(page);
    pageWidget->setShowTip(ui->ordersTableWidget->rowCount(),ROWNUMBER);
    pageChangeSlot(0);
    pageWidget->setVisible(0 != size);

    blankWidget->setVisible(0 == size);
}

void OnchainOrderPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void OnchainOrderPage::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;

    HXChain::getInstance()->currentSellAssetId = ui->assetComboBox->currentData().toString();

    ui->ordersTableWidget->setRowCount(0);
    updateTableHeaders();
//    if(ui->assetComboBox->currentText().isEmpty() || ui->assetComboBox2->currentText().isEmpty()
//            || ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())       return;

    queryContractOrders();
    checkFavorite();
}

void OnchainOrderPage::on_assetComboBox2_currentIndexChanged(const QString &arg1)
{
    if(!inited)     return;

    HXChain::getInstance()->currentBuyAssetId = ui->assetComboBox2->currentData().toString();

    ui->ordersTableWidget->setRowCount(0);
    updateTableHeaders();
//    if(ui->assetComboBox->currentText().isEmpty() || ui->assetComboBox2->currentText().isEmpty()
//            || ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())       return;

    queryContractOrders();
    checkFavorite();
}

void OnchainOrderPage::queryContractOrders()
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method","Zchain.Exchange.queryContracts");
    QJsonObject paramObject;
    paramObject.insert("from_asset", getRealAssetSymbol( ui->assetComboBox->currentText()));
    paramObject.insert("to_asset", getRealAssetSymbol( ui->assetComboBox2->currentText()));
    paramObject.insert("limit",10);
    object.insert("params",paramObject);
    httpManager.post(HXChain::getInstance()->middlewarePath,QJsonDocument(object).toJson());
}

void OnchainOrderPage::updateTableHeaders()
{
    if(ui->assetComboBox->currentText() == ui->assetComboBox2->currentText())
    {
        ui->ordersTableWidget->horizontalHeaderItem(0)->setText(tr("SELL"));
        ui->ordersTableWidget->horizontalHeaderItem(1)->setText(tr("BUY"));
        ui->ordersTableWidget->horizontalHeaderItem(2)->setText(tr("PRICE"));
    }
    else
    {
        ui->ordersTableWidget->horizontalHeaderItem(0)->setText(tr("SELL / %1").arg(ui->assetComboBox->currentText()));
        ui->ordersTableWidget->horizontalHeaderItem(1)->setText(tr("BUY / %1").arg(ui->assetComboBox2->currentText()));
        ui->ordersTableWidget->horizontalHeaderItem(2)->setText(tr("PRICE (%1/%2)").arg(ui->assetComboBox->currentText()).arg(ui->assetComboBox2->currentText()));
    }

}

QJsonArray OnchainOrderPage::sortArray(const QJsonArray &data, bool greater)
{
    std::vector<std::pair<double,QJsonValue>> result;
    int size = data.size();
    for(int i = 0; i < size; i++)
    {
        QJsonObject dataObject = data.at(i).toObject();

        unsigned long long sellAmount = jsonValueToULL(dataObject.value("from_supply"));
        QString sellSymbol = dataObject.value("from_asset").toString();
        unsigned long long buyAmount = jsonValueToULL(dataObject.value("to_supply"));
        QString buySymbol = dataObject.value("to_asset").toString();

        AssetInfo sellAssetInfo = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(sellSymbol));
        AssetInfo buyAssetInfo  = HXChain::getInstance()->assetInfoMap.value(HXChain::getInstance()->getAssetId(buySymbol));

        double price = (double)sellAmount / qPow(10,sellAssetInfo.precision) / buyAmount * qPow(10,buyAssetInfo.precision);

        result.emplace_back(std::make_pair(price,data.at(i)));
    }
    std::stable_sort(result.begin(),result.end(),[greater](std::pair<double,QJsonValue> fi,std::pair<double,QJsonValue>se){
        return greater ? (fi.first < se.first):(fi.first > se.first);
    });
    QJsonArray arr;
    std::for_each(result.begin(),result.end(),[&arr](std::pair<double,QJsonValue> info){
        arr.push_back(info.second);
    });
    return arr;
}

void OnchainOrderPage::onItemClicked(int _row, int _column)
{
    if(_column == 3)
    {
        if(HXChain::getInstance()->accountInfoMap.isEmpty())
        {
            CommonDialog dia(CommonDialog::OkOnly);
            dia.setText(tr("Please Import Or Create Account First!"));
            dia.pop();
            HXChain::getInstance()->mainFrame->ShowMainPageSlot();
            return;
        }


        if(!HXChain::getInstance()->ValidateOnChainOperation()) return;

        BuyOrderWidget* buyOrderWidget = new BuyOrderWidget(this);
        buyOrderWidget->setAttribute(Qt::WA_DeleteOnClose);
        buyOrderWidget->move(0,0);
        buyOrderWidget->show();
        buyOrderWidget->raise();

        buyOrderWidget->setAccount(ui->accountComboBox->currentText());
        QString contractAddress = ui->ordersTableWidget->item(_row,2)->data(Qt::UserRole).toString();
        buyOrderWidget->setContractAddress(contractAddress);
        buyOrderWidget->setOrderInfo(ui->ordersTableWidget->item(_row,0)->data(Qt::UserRole).toULongLong(), getRealAssetSymbol( ui->assetComboBox->currentText())
                                     ,ui->ordersTableWidget->item(_row,1)->data(Qt::UserRole).toULongLong(), getRealAssetSymbol( ui->assetComboBox2->currentText()));

        currentWidget = buyOrderWidget;

        emit backBtnVisible(true);

        return;
    }
}

void OnchainOrderPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if(!inited)  return;

    HXChain::getInstance()->currentAccount = ui->accountComboBox->currentText();

    on_assetComboBox_currentIndexChanged(ui->assetComboBox->currentText());
}

void OnchainOrderPage::pageChangeSlot(unsigned int page)
{
    for(int i = 0;i < ui->ordersTableWidget->rowCount();++i)
    {
        if(i < page*ROWNUMBER)
        {
            ui->ordersTableWidget->setRowHidden(i,true);
        }
        else if(page * ROWNUMBER <= i && i < page*ROWNUMBER + ROWNUMBER)
        {
            ui->ordersTableWidget->setRowHidden(i,false);
        }
        else
        {
            ui->ordersTableWidget->setRowHidden(i,true);
        }
    }

}

void OnchainOrderPage::on_swapBtn_clicked()
{
    inited = false;
    QString temp = ui->assetComboBox->currentText();
    ui->assetComboBox->setCurrentText(ui->assetComboBox2->currentText());
    HXChain::getInstance()->currentSellAssetId = ui->assetComboBox->currentData().toString();
    inited = true;
    ui->assetComboBox2->setCurrentText(temp);
}

void OnchainOrderPage::on_favoriteBtn_clicked()
{
    QString str = ui->assetComboBox->currentText() + "+" + ui->assetComboBox2->currentText();
    if(ui->favoriteBtn->isChecked())
    {
        HXChain::getInstance()->configFile->beginGroup("/FavoriteOrderPairs");
        QStringList keys = HXChain::getInstance()->configFile->childKeys();
        HXChain::getInstance()->configFile->endGroup();
        if(keys.size() >= 6)         // 最多6个常用
        {
            CommonDialog dia(CommonDialog::OkOnly);
            dia.setText(tr("You can only set at most 6 favorite order pairs!"));
            dia.pop();
            ui->favoriteBtn->setChecked(false);
            return;
        }

        HXChain::getInstance()->configFile->setValue("/FavoriteOrderPairs/" + str, 1);
        ui->favoriteBtn->setChecked(true);

    }
    else
    {
        HXChain::getInstance()->configFile->remove("/FavoriteOrderPairs/" + str);
        ui->favoriteBtn->setChecked(false);
    }

    favoritePairsWidget->init();
    favoritePairsWidget->setCurrentBtn(str.replace("+","/"));
}

void OnchainOrderPage::showPair(QString pairStr)
{
    if(!pairStr.contains("/"))      return;
    QStringList strList = pairStr.split("/");
    inited = false;
    ui->assetComboBox->setCurrentText(strList.at(0));
    ui->assetComboBox2->setCurrentText(strList.at(1));
    inited = true;
    on_assetComboBox_currentIndexChanged(ui->assetComboBox->currentText());
}

void OnchainOrderPage::checkFavorite()
{
    if(ui->assetComboBox->currentText().isEmpty() || ui->assetComboBox2->currentText().isEmpty()
            || ui->assetComboBox->currentText() == ui->assetComboBox2->currentText() )
    {
        ui->favoriteBtn->hide();
        return;
    }

    ui->favoriteBtn->show();

    HXChain::getInstance()->configFile->beginGroup("/FavoriteOrderPairs");
    QStringList keys = HXChain::getInstance()->configFile->childKeys();
    HXChain::getInstance()->configFile->endGroup();

    QString str = ui->assetComboBox->currentText() + "+" + ui->assetComboBox2->currentText();
    if(keys.contains(str))
    {
        ui->favoriteBtn->setChecked(true);
    }
    else
    {
        ui->favoriteBtn->setChecked(false);
    }

    favoritePairsWidget->setCurrentBtn(str.replace("+","/"));
}

void OnchainOrderPage::on_exchangeModeBtn_clicked()
{
    ExchangeModeWidget* exchangeModeWidget = new ExchangeModeWidget(this);
    exchangeModeWidget->setAttribute(Qt::WA_DeleteOnClose);
    exchangeModeWidget->show();
    exchangeModeWidget->raise();
}
