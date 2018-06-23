#include "WithdrawConfirmPage.h"
#include "ui_WithdrawConfirmPage.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"

WithdrawConfirmPage::WithdrawConfirmPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WithdrawConfirmPage)
{
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->crosschainTransactionTableWidget->installEventFilter(this);
    ui->crosschainTransactionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->crosschainTransactionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->crosschainTransactionTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->crosschainTransactionTableWidget->setMouseTracking(true);
    ui->crosschainTransactionTableWidget->setShowGrid(false);//隐藏表格线

    ui->crosschainTransactionTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->crosschainTransactionTableWidget->horizontalHeader()->setVisible(true);
    ui->crosschainTransactionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->crosschainTransactionTableWidget->setColumnWidth(0,120);
    ui->crosschainTransactionTableWidget->setColumnWidth(1,120);
    ui->crosschainTransactionTableWidget->setColumnWidth(2,100);
    ui->crosschainTransactionTableWidget->setColumnWidth(3,100);
    ui->crosschainTransactionTableWidget->setColumnWidth(4,80);
    ui->crosschainTransactionTableWidget->setColumnWidth(5,80);
    ui->crosschainTransactionTableWidget->setColumnWidth(6,80);
    ui->crosschainTransactionTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);

    init();
}

WithdrawConfirmPage::~WithdrawConfirmPage()
{
    delete ui;
}

void WithdrawConfirmPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = UBChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    fetchCrosschainTransactions();
}

void WithdrawConfirmPage::fetchCrosschainTransactions(int type)
{
    UBChain::getInstance()->postRPC( "id-get_crosschain_transaction-" + QString::number(type), toJsonFormat( "get_crosschain_transaction",
                                                                                    QJsonArray() << type));
}

void WithdrawConfirmPage::jsonDataUpdated(QString id)
{
    if(id.startsWith("id-get_crosschain_transaction-"))
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        result.prepend("{");
        result.append("}");

        QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonArray array = jsonObject.take("result").toArray();

        foreach (QJsonValue v, array)
        {
            QJsonArray array2 = v.toArray();
            QJsonObject object = array2.at(1).toObject();
            QJsonArray operationArray = object.take("operations").toArray();
            QJsonArray array3 = operationArray.at(0).toArray();
            int operationType = array3.at(0).toInt();
            QJsonObject operationObject = array3.at(1).toObject();

            if(operationType == 62)
            {
                GeneratedTransaction gt;
                gt.trxId = array2.at(0).toString();
                QJsonArray ccwArray = operationObject.take("ccw_trx_ids").toArray();
                foreach (QJsonValue v, ccwArray)
                {
                    gt.ccwTrxIds += v.toString();
                }

                generatedTransactionMap.insert(gt.trxId, gt);
            }
            else if(operationType == 61)
            {
                ApplyTransaction at;
                at.trxId            = array2.at(0).toString();
                at.expirationTime   = object.take("expiration").toString();
                at.withdrawAddress  = operationObject.take("withdraw_account").toString();
                at.amount           = operationObject.take("amount").toString();
                at.assetSymbol      = operationObject.take("asset_symbol").toString();
                at.assetId          = operationObject.take("asset_id").toString();
                at.crosschainAddress  = operationObject.take("crosschain_account").toString();
                at.memo             = operationObject.take("memo").toString();

                applyTransactionMap.insert(at.trxId, at);
            }
        }


        QStringList keys = applyTransactionMap.keys();
        int size = keys.size();
        ui->crosschainTransactionTableWidget->setRowCount(0);
        ui->crosschainTransactionTableWidget->setRowCount(size);

        for(int i = 0; i < size; i++)
        {
            ui->crosschainTransactionTableWidget->setRowHeight(i,40);

            ApplyTransaction at = applyTransactionMap.value(keys.at(i));

            QDateTime time = QDateTime::fromString(at.expirationTime, "yyyy-MM-ddThh:mm:ss");
            time = time.addSecs(-600);       // 时间减10分钟
            QString currentDateTime = time.toString("yyyy-MM-dd hh:mm:ss");
            ui->crosschainTransactionTableWidget->setItem(i, 0, new QTableWidgetItem(currentDateTime));

            ui->crosschainTransactionTableWidget->setItem(i, 1, new QTableWidgetItem(at.amount + " " + at.assetSymbol));

            ui->crosschainTransactionTableWidget->setItem(i, 2, new QTableWidgetItem(at.withdrawAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 3, new QTableWidgetItem(at.crosschainAddress));

            ui->crosschainTransactionTableWidget->setItem(i, 6, new QTableWidgetItem(tr("sign")));
            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->crosschainTransactionTableWidget->item(i,6)->text());
//            toolButton->setBackgroundColor(itemColor);
            ui->crosschainTransactionTableWidget->setCellWidget(i,6,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked,this,i,6));


            for (int j : {0,1,2,3})
            {
                if(i%2)
                {
                    ui->crosschainTransactionTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                    ui->crosschainTransactionTableWidget->item(i,j)->setBackgroundColor(QColor(252,253,255));
                }
                else
                {
                    ui->crosschainTransactionTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
                    ui->crosschainTransactionTableWidget->item(i,j)->setBackgroundColor(QColor("white"));
                }
            }
        }

        return;
    }
}

void WithdrawConfirmPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}

void WithdrawConfirmPage::on_crosschainTransactionTableWidget_cellClicked(int row, int column)
{

}
