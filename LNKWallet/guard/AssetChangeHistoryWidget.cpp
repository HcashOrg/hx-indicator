#include "AssetChangeHistoryWidget.h"
#include "ui_AssetChangeHistoryWidget.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "control/AssetIconItem.h"
#include "showcontentdialog.h"

AssetChangeHistoryWidget::AssetChangeHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetChangeHistoryWidget)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->changeHistoryTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->changeHistoryTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->changeHistoryTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->changeHistoryTableWidget->setMouseTracking(true);
    ui->changeHistoryTableWidget->setShowGrid(false);//隐藏表格线
    ui->changeHistoryTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->changeHistoryTableWidget->horizontalHeader()->setVisible(true);
    ui->changeHistoryTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->changeHistoryTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->changeHistoryTableWidget->verticalHeader()->setVisible(false);
    ui->changeHistoryTableWidget->setColumnWidth(0,100);
    ui->changeHistoryTableWidget->setColumnWidth(1,150);
    ui->changeHistoryTableWidget->setColumnWidth(2,150);
    ui->changeHistoryTableWidget->setColumnWidth(3,100);
    ui->changeHistoryTableWidget->setColumnWidth(4,100);
    ui->changeHistoryTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

}

AssetChangeHistoryWidget::~AssetChangeHistoryWidget()
{
    delete ui;
}

void AssetChangeHistoryWidget::setAsset(QString _assetSymbol)
{
    assetSymbol = _assetSymbol;
    UBChain::getInstance()->postRPC( "id-get_multisig_account_pair-" + assetSymbol, toJsonFormat( "get_multisig_account_pair",
                                                                           QJsonArray() << assetSymbol));

}

void AssetChangeHistoryWidget::jsonDataUpdated(QString id)
{
    if( id == "id-get_multisig_account_pair-" + assetSymbol)
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if(result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");
            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject jsonObject = parse_doucment.object();
            QJsonArray array = jsonObject.take("result").toArray();

            int size = array.size();
            ui->changeHistoryTableWidget->setRowCount(0);
            ui->changeHistoryTableWidget->setRowCount(size);

            for(int i = 0; i < size; i++)
            {
                int row = size - i - 1;
                ui->changeHistoryTableWidget->setRowHeight(row,40);

                QJsonObject object = array.at(i).toObject();
                QString hotAddress = object.take("bind_account_hot").toString();
                QString coldAddress = object.take("bind_account_cold").toString();
                int effectiveBlock = object.take("effective_block_num").toInt();

                ui->changeHistoryTableWidget->setItem(row,0,new QTableWidgetItem(assetSymbol));
                ui->changeHistoryTableWidget->setItem(row,1,new QTableWidgetItem(hotAddress));
                ui->changeHistoryTableWidget->setItem(row,2,new QTableWidgetItem(coldAddress));
                ui->changeHistoryTableWidget->setItem(row,3,new QTableWidgetItem(QString::number(effectiveBlock)));
                ui->changeHistoryTableWidget->setItem(row,4,new QTableWidgetItem(tr("export")));

                QString itemColor = (row % 2) ?"rgb(252,253,255)":"white";

                AssetIconItem* assetIconItem = new AssetIconItem();
                assetIconItem->setAsset(ui->changeHistoryTableWidget->item(row,0)->text());
                assetIconItem->setBackgroundColor(itemColor);
                ui->changeHistoryTableWidget->setCellWidget(row, 0, assetIconItem);

                ToolButtonWidget *toolButton = new ToolButtonWidget();
                toolButton->setText(ui->changeHistoryTableWidget->item(row,4)->text());
                toolButton->setBackgroundColor(itemColor);
                ui->changeHistoryTableWidget->setCellWidget(row,4,toolButton);
                connect(toolButton,&ToolButtonWidget::clicked,std::bind(&AssetChangeHistoryWidget::on_changeHistoryTableWidget_cellClicked,this,i,4));

                for(int j = 1; j < 4; j++)
                {
                    if(row%2)
                    {
                        ui->changeHistoryTableWidget->item(row,j)->setTextAlignment(Qt::AlignCenter);
                        ui->changeHistoryTableWidget->item(row,j)->setBackgroundColor(QColor(252,253,255));
                    }
                    else
                    {
                        ui->changeHistoryTableWidget->item(row,j)->setTextAlignment(Qt::AlignCenter);
                        ui->changeHistoryTableWidget->item(row,j)->setBackgroundColor(QColor("white"));
                    }
                }
            }
        }

        return;
    }
}

void AssetChangeHistoryWidget::on_changeHistoryTableWidget_cellPressed(int row, int column)
{
    if( column == 1 || column == 2 )
    {
        ShowContentDialog showContentDialog( ui->changeHistoryTableWidget->item(row, column)->text(),this);

        int x = ui->changeHistoryTableWidget->columnViewportPosition(column) + ui->changeHistoryTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->changeHistoryTableWidget->rowViewportPosition(row) - 10 + ui->changeHistoryTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->changeHistoryTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void AssetChangeHistoryWidget::on_changeHistoryTableWidget_cellClicked(int row, int column)
{

}
