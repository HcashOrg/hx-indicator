#include "CitizenPolicyPage.h"
#include "ui_CitizenPolicyPage.h"

#include "wallet.h"
#include "ToolButtonWidget.h"
#include "commondialog.h"
#include "CitizenResolutionDialog.h"
#include "nameTransfer/ShowTrxCodeDialog.h"

CitizenPolicyPage::CitizenPolicyPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CitizenPolicyPage)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->resolutionTableWidget->installEventFilter(this);
    ui->resolutionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->resolutionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->resolutionTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->resolutionTableWidget->setFrameShape(QFrame::NoFrame);
    ui->resolutionTableWidget->setMouseTracking(true);
    ui->resolutionTableWidget->setShowGrid(false);//隐藏表格线

    ui->resolutionTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->resolutionTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->resolutionTableWidget->horizontalHeader()->setVisible(true);
    ui->resolutionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->resolutionTableWidget->setColumnWidth(0,140);
    ui->resolutionTableWidget->setColumnWidth(1,170);
    ui->resolutionTableWidget->setColumnWidth(2,130);
    ui->resolutionTableWidget->setColumnWidth(3,90);
    ui->resolutionTableWidget->setColumnWidth(4,120);
    ui->resolutionTableWidget->setStyleSheet(TABLEWIDGET_STYLE_1);


    HXChain::getInstance()->mainFrame->installBlurEffect(ui->resolutionTableWidget);

    init();
}

CitizenPolicyPage::~CitizenPolicyPage()
{
    delete ui;
}

void CitizenPolicyPage::init()
{
    inited = false;

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

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->label->pos(), QSize(300,30)));
        label->setText(tr("There are no citizen accounts in the wallet."));
        label->setStyleSheet("QLabel{color: rgb(137,129,161);font: 11px \"Microsoft YaHei UI Light\";}");
    }

    inited = true;

    on_accountComboBox_currentIndexChanged(ui->accountComboBox->currentText());

}

void CitizenPolicyPage::refresh()
{
    showResolutions();
}

void CitizenPolicyPage::jsonDataUpdated(QString id)
{
    if( id == "CitizenPolicyPage-get_votes")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        result.prepend("{");
        result.append("}");
        QJsonArray resultArray = QJsonDocument::fromJson(result.toLatin1()).object().value("result").toArray();
        int size = resultArray.size();
        ui->resolutionTableWidget->setRowCount(0);
        ui->resolutionTableWidget->setRowCount(size);
        for(int i = 0; i < size; i++)
        {
            ui->resolutionTableWidget->setRowHeight(i,40);

            ResolutionInfo info;

            QJsonObject object = resultArray.at(i).toObject();
            info.id = object.value("id").toString();
            info.expirationTime = object.value("expiration").toString();
            info.title = object.value("title").toString();

            QJsonArray optionsArray = object.value("options").toArray();
            for(int j = 0; j < optionsArray.size(); j++)
            {
                QJsonArray array = optionsArray.at(j).toArray();
                info.optionsMap.insert(array.at(0).toInt(), array.at(1).toString());
            }

            QJsonArray resultArray = object.value("result").toArray();
            for(int j = 0; j < resultArray.size(); j++)
            {
                QJsonArray array = resultArray.at(j).toArray();
                int index = array.at(0).toInt();
                QJsonArray array2 = array.at(1).toArray();
                QStringList strlist;
                foreach (const QJsonValue& v, array2) {
                    strlist << v.toString();
                }
                info.resultMap.insert(index, strlist);
            }

            QJsonArray plegeArray = object.value("plege").toArray();
            for(int j = 0; j < plegeArray.size(); j++)
            {
                QJsonArray array = plegeArray.at(j).toArray();
                info.pledgeMap.insert(array.at(0).toInt(), array.at(1).toString());
            }

            ui->resolutionTableWidget->setItem(i,0, new QTableWidgetItem(toLocalTime(info.expirationTime)));
            ui->resolutionTableWidget->item(i,0)->setData(Qt::UserRole,QVariant::fromValue<ResolutionInfo>(info));

            ui->resolutionTableWidget->setItem(i,1, new QTableWidgetItem(info.title));

            ui->resolutionTableWidget->setItem(i,2, new QTableWidgetItem(tr("view options")));
            ToolButtonWidget *toolButton = new ToolButtonWidget();
            toolButton->setText(ui->resolutionTableWidget->item(i,2)->text());
            toolButton->setButtonFixSize(130,40);
            ui->resolutionTableWidget->setCellWidget(i,2,toolButton);
            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&CitizenPolicyPage::on_resolutionTableWidget_cellClicked,this,i,2));


            ui->resolutionTableWidget->setItem(i,3, new QTableWidgetItem(calProposalWeight(info)));

            QString address = HXChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address;
            if(address.isEmpty())
            {
                ui->resolutionTableWidget->setItem(i,4,new QTableWidgetItem(tr("no citizen")));
            }
            else
            {
                int option = searchOptionByAddress(info,address);
                if(option == -1)
                {
                    ui->resolutionTableWidget->setItem(i,4,new QTableWidgetItem(tr("not voted")));
                }
                else
                {
                    ui->resolutionTableWidget->setItem(i,4,new QTableWidgetItem(QString::number(option)));
                    ui->resolutionTableWidget->item(i,4)->setTextColor(QColor(0,170,0));
                }
            }

        }

        tableWidgetSetItemZebraColor(ui->resolutionTableWidget);

        return;
    }
}

void CitizenPolicyPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void CitizenPolicyPage::showResolutions()
{
    HXChain::getInstance()->postRPC("CitizenPolicyPage-get_votes",toJsonFormat("get_votes",QJsonArray() << "volans"));

}

QString CitizenPolicyPage::calProposalWeight(const ResolutionInfo &info) const
{
    unsigned long long allWeight = 0;
    unsigned long long alreadyWeight = 0;
    QMap<QString,MinerInfo> allMiner(HXChain::getInstance()->minerMap);
    QMapIterator<QString, MinerInfo> i(allMiner);
    while (i.hasNext()) {
        i.next();
        allWeight += i.value().pledgeWeight;
    }

    foreach (const QString& str , info.pledgeMap) {
        alreadyWeight += str.toULongLong();
    }

    if(0 != allWeight)
    {
        return QString::number(alreadyWeight * 100.0 / allWeight,'f',2)+"%";
    }
    else
    {
        return "0.00%";
    }
}

int CitizenPolicyPage::searchOptionByAddress(const ResolutionInfo &info, QString address)
{
    int result = -1;
    foreach (int index, info.resultMap.keys())
    {
        QStringList strList = info.resultMap.value(index);
        if(strList.contains(address))
        {
            result = index;
            break;
        }
    }

    return result;
}

void CitizenPolicyPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    showResolutions();
}

void CitizenPolicyPage::on_resolutionTableWidget_cellClicked(int row, int column)
{
    if(column == 1)
    {
        if( !ui->resolutionTableWidget->item(row, column))    return;

        ShowTrxCodeDialog showTrxCodeDialog( ui->resolutionTableWidget->item(row, column)->text(),this);

        int x = ui->resolutionTableWidget->columnViewportPosition(column) + ui->resolutionTableWidget->columnWidth(column) / 2
                - showTrxCodeDialog.width() / 2;
        int y = ui->resolutionTableWidget->rowViewportPosition(row) - 10 + ui->resolutionTableWidget->horizontalHeader()->height();

        showTrxCodeDialog.move( ui->resolutionTableWidget->mapToGlobal( QPoint(x, y)));
        showTrxCodeDialog.exec();

        return;
    }

    if(column == 2)
    {
        if(ui->resolutionTableWidget->item(row,0) == nullptr)  return;
        const ResolutionInfo& info = ui->resolutionTableWidget->item(row,0)->data(Qt::UserRole).value<ResolutionInfo>();
        CitizenResolutionDialog citizenResolutionDialog(info,ui->accountComboBox->currentText());
        citizenResolutionDialog.pop();

        showResolutions();

        return;
    }
}
