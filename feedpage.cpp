#include "feedpage.h"
#include "ui_feedpage.h"

#include "wallet.h"
#include "dialog/feedpricedialog.h"
#include "showcontentdialog.h"
#include "control/feedassetcellwidget.h"

#include <QDebug>
#include <QPainter>

FeedPage::FeedPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeedPage)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(244,244,242));
    setPalette(palette);

    ui->tokenPriceTableWidget->installEventFilter(this);
    ui->tokenPriceTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tokenPriceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tokenPriceTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->tokenPriceTableWidget->setFrameShape(QFrame::NoFrame);
    ui->tokenPriceTableWidget->setMouseTracking(true);
    ui->tokenPriceTableWidget->setShowGrid(false);//隐藏表格线

    ui->tokenPriceTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->tokenPriceTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->tokenPriceTableWidget->horizontalHeader()->setVisible(true);
    ui->tokenPriceTableWidget->verticalHeader()->setVisible(false);
    ui->tokenPriceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->tokenPriceTableWidget->setColumnWidth(0,75);
    ui->tokenPriceTableWidget->setColumnWidth(1,75);
    ui->tokenPriceTableWidget->setColumnWidth(2,50);


    ui->feedersPricesTableWidget->installEventFilter(this);
    ui->feedersPricesTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->feedersPricesTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->feedersPricesTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->feedersPricesTableWidget->setFrameShape(QFrame::NoFrame);
    ui->feedersPricesTableWidget->setMouseTracking(true);
    ui->feedersPricesTableWidget->setShowGrid(false);//隐藏表格线

    ui->feedersPricesTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->feedersPricesTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->feedersPricesTableWidget->horizontalHeader()->setVisible(true);
    ui->feedersPricesTableWidget->verticalHeader()->setVisible(false);
    ui->feedersPricesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    ui->feedersPricesTableWidget->setColumnWidth(0,280);
    ui->feedersPricesTableWidget->setColumnWidth(1,120);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    showTokenPrice();

    getAllTokenPrices();
}

FeedPage::~FeedPage()
{
    delete ui;
}


void FeedPage::showTokenPrice()
{
    QStringList keys = UBChain::getInstance()->tokenPriceMap.keys();
    int size = keys.size();
    ui->tokenPriceTableWidget->setRowCount(0);
    ui->tokenPriceTableWidget->setRowCount(size);

    bool showFeedBtnOrNot = UBChain::getInstance()->getMyFeederAccount().size() > 0;

    for( int i = 0; i < size; i++)
    {
        ui->tokenPriceTableWidget->setRowHeight(i,37);

        QString key = keys.at(i);

        FeedAssetCellWidget* cellWidget = new FeedAssetCellWidget;
        cellWidget->setAssetName(key);
        if(key == currentContractName)
        {
            cellWidget->setChecked(true);
        }
        else
        {
            cellWidget->setChecked(false);
        }
        ui->tokenPriceTableWidget->setCellWidget(i,0, cellWidget);


//        ui->tokenPriceTableWidget->setItem( i, 0, new QTableWidgetItem( key) );
//        ui->tokenPriceTableWidget->item(i,0)->setTextColor(QColor(192,196,212));

        ui->tokenPriceTableWidget->setItem( i, 1, new QTableWidgetItem(  getBigNumberString(UBChain::getInstance()->tokenPriceMap.value(key).toULongLong(),ASSET_PRECISION) ) );
        ui->tokenPriceTableWidget->item(i,1)->setTextColor(QColor(24,250,239));

        if( showFeedBtnOrNot)
        {
            ui->tokenPriceTableWidget->setItem( i, 2, new QTableWidgetItem( tr("Feed")) );
            ui->tokenPriceTableWidget->item(i,2)->setTextColor(QColor(192,196,212));
            QFont font = ui->tokenPriceTableWidget->font();
            font.setUnderline(true);
            ui->tokenPriceTableWidget->item(i,2)->setFont(font);
        }
        else
        {
            ui->tokenPriceTableWidget->setItem( i, 2, new QTableWidgetItem( tr("Not feeder")) );
            ui->tokenPriceTableWidget->item(i,2)->setTextColor(QColor(230,53,59));
        }



        for(int j = 1; j < 3; j++)
        {
            ui->tokenPriceTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }

    }
}

void FeedPage::refresh()
{
    showTokenPrice();

    getAllTokenPrices();
}

void FeedPage::jsonDataUpdated(QString id)
{
    if( id == QString("id_contract_call_offline_") + FEEDER_CONTRACT_ADDRESS + "_all_token_prices")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        result.remove("\\");

        result.replace("\"{","{");
        result.replace("}\"","}");

        if( result.startsWith("\"result\":"))
        {

            result.prepend("{");
            result.append("}");

            QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
            QByteArray ba = utfCodec->fromUnicode(result);


            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {

                    QJsonObject jsonObject = parse_doucment.object();
                    if( jsonObject.contains("result"))
                    {
                        QJsonValue resultValue = jsonObject.take("result");
                        if( resultValue.isObject())
                        {
                            QJsonObject resultObject = resultValue.toObject();
                            QStringList tokens = resultObject.keys();
                            tokenFeedPricesMap.clear();
                            foreach (QString token, tokens)
                            {
                                QJsonObject object = resultObject.take(token).toObject();
                                QStringList feeders = object.keys();
                                AddressFeedPriceMap addressFeedPriceMap;
                                foreach (QString feeder, feeders)
                                {
                                    QJsonValue priceValue = object.take(feeder);
                                    unsigned long long  price = 0;
                                    if(priceValue.isString())
                                    {
                                        price = priceValue.toString().toULongLong();
                                    }
                                    else
                                    {
                                        price = QString::number(priceValue.toDouble(),'g',12).toULongLong();
                                    }
                                    addressFeedPriceMap.insert(feeder,price);
                                }

                                tokenFeedPricesMap.insert(token,addressFeedPriceMap);
                            }

                        }
                    }
                }
            }

        }
        return;
    }
}

void FeedPage::getAllTokenPrices()
{
    if(UBChain::getInstance()->addressMap.size() < 1)   return;
    UBChain::getInstance()->postRPC( QString("id_contract_call_offline_") + FEEDER_CONTRACT_ADDRESS + "_all_token_prices",
                                     toJsonFormat( "contract_call_offline",
                                                   QStringList() << FEEDER_CONTRACT_ADDRESS << UBChain::getInstance()->addressMap.keys().at(0)
                                                   << "all_token_prices" << ""));

}

void FeedPage::showFeedPrices(QString token)
{
    if(!tokenFeedPricesMap.contains(token))  return;

    AddressFeedPriceMap addressFeedPriceMap = tokenFeedPricesMap.value(token);
    QStringList addresses = addressFeedPriceMap.keys();
    int size = addressFeedPriceMap.size();
    ui->feedersPricesTableWidget->setRowCount(size);
    for(int i = 0; i < size; i ++)
    {
        ui->feedersPricesTableWidget->setRowHeight(i,37);

        QTableWidgetItem* item0 = new QTableWidgetItem(addresses.at(i));
        ui->feedersPricesTableWidget->setItem(i,0, item0);
        ui->feedersPricesTableWidget->item(i,0)->setTextColor(QColor(192,196,212));


        QString priceStr = QString("1 %1 = %2 %3").arg(token).arg(getBigNumberString(addressFeedPriceMap.value(addresses.at(i)),ASSET_PRECISION)).arg(ASSET_NAME);
        QTableWidgetItem* item1 = new QTableWidgetItem(priceStr);
        ui->feedersPricesTableWidget->setItem(i,1, item1);
        ui->feedersPricesTableWidget->item(i,1)->setTextColor(QColor(24,250,239));

        for(int j = 0; j < 2; j++)
        {
            ui->feedersPricesTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);

            if(i % 2)
            {
                ui->feedersPricesTableWidget->item(i,j)->setBackgroundColor(QColor(43,49,69));
            }
            else
            {
                ui->feedersPricesTableWidget->item(i,j)->setBackgroundColor(QColor(40,46,66));
            }

        }

    }

}

void FeedPage::on_tokenPriceTableWidget_cellPressed(int row, int column)
{
    if( column == 2)
    {
        if(ui->tokenPriceTableWidget->cellWidget(row,0) == NULL)    return;
        if( UBChain::getInstance()->getMyFeederAccount().size() > 0 )
        {
            FeedPriceDialog feedPriceDialog;
            FeedAssetCellWidget* cellWidget = static_cast<FeedAssetCellWidget*>(ui->tokenPriceTableWidget->cellWidget(row,0));
            feedPriceDialog.setCurrentToken( cellWidget->assetName);
            feedPriceDialog.pop();
        }

        return;
    }

    if( column == 0 )
    {
//        if(ui->tokenPriceTableWidget->item(row,0) == NULL)  return;
//        showFeedPrices(ui->tokenPriceTableWidget->item(row,0)->text());

        if(ui->tokenPriceTableWidget->cellWidget(row,0) == NULL)    return;
        FeedAssetCellWidget* cellWidget = static_cast<FeedAssetCellWidget*>(ui->tokenPriceTableWidget->cellWidget(row,0));
        showFeedPrices(cellWidget->assetName);
        cellWidget->setChecked(true);

        currentContractName = cellWidget->assetName;
        showTokenPrice();

        return;
    }

    if( column == 1)
    {
        if(ui->tokenPriceTableWidget->item(row, column) == NULL)   return;
        ShowContentDialog showContentDialog( ui->tokenPriceTableWidget->item(row, column)->text(),this);

        int x = ui->tokenPriceTableWidget->columnViewportPosition(column) + ui->tokenPriceTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->tokenPriceTableWidget->rowViewportPosition(row) - 10 + ui->tokenPriceTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->tokenPriceTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void FeedPage::on_feedersPricesTableWidget_cellPressed(int row, int column)
{
    if( column == 0 )
    {
        if(ui->feedersPricesTableWidget->item(row, column) == NULL)   return;
        ShowContentDialog showContentDialog( ui->feedersPricesTableWidget->item(row, column)->text(),this);

        int x = ui->feedersPricesTableWidget->columnViewportPosition(column) + ui->feedersPricesTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->feedersPricesTableWidget->rowViewportPosition(row) - 10 + ui->feedersPricesTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->feedersPricesTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }

    if( column == 1)
    {
        if(ui->feedersPricesTableWidget->item(row, column) == NULL)   return;
        ShowContentDialog showContentDialog( ui->feedersPricesTableWidget->item(row, column)->text(),this);

        int x = ui->feedersPricesTableWidget->columnViewportPosition(column) + ui->feedersPricesTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->feedersPricesTableWidget->rowViewportPosition(row) - 10 + ui->feedersPricesTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->feedersPricesTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void FeedPage::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(0,0,680,482);
}
