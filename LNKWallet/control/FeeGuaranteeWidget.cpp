#include "FeeGuaranteeWidget.h"
#include "ui_FeeGuaranteeWidget.h"

#include "wallet.h"

FeeGuaranteeWidget::FeeGuaranteeWidget(QString _guaranteeId, double _feeAmount, QString _trxId, QWidget *parent) :
    QWidget(parent),
    guaranteeId(_guaranteeId),
    feeAmount(_feeAmount),
    trxId(_trxId),
    ui(new Ui::FeeGuaranteeWidget)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->label->setText(tr("use guaranteeOrder %1").arg(guaranteeId));

    fetchGuaranteeOrder();
}

FeeGuaranteeWidget::~FeeGuaranteeWidget()
{
    delete ui;
}

void FeeGuaranteeWidget::setBackgroundColor(QString color)
{
    ui->label->setStyleSheet(QString("QLabel{background-color:%1;color:rgb(52,37,90);}").arg(color));
}

void FeeGuaranteeWidget::jsonDataUpdated(QString id)
{
    if(id == "FeeGuaranteeWidget-get_guarantee_order-" + trxId)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;

        if( result.startsWith("\"result\":"))             // 成功
        {
            result.prepend("{");
            result.append("}");

            QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLatin1());
            QJsonObject object = parse_doucment.object().value("result").toObject();

            QJsonObject originObject = object.value("asset_orign").toObject();
            QString originAssetId = originObject.value("asset_id").toString();
            AssetInfo originAssetInfo = HXChain::getInstance()->assetInfoMap.value(originAssetId);
            double originAmount = getBigNumberString( jsonValueToULL( originObject.value("amount")), originAssetInfo.precision).toDouble();

            QJsonObject targetObject = object.value("asset_target").toObject();
            QString targetAssetId = targetObject.value("asset_id").toString();
            AssetInfo targetAssetInfo = HXChain::getInstance()->assetInfoMap.value(targetAssetId);
            double targetAmount = getBigNumberString( jsonValueToULL( targetObject.value("amount")), targetAssetInfo.precision).toDouble();

            ui->label->setText(QString("%1 %2 (%3)").arg(feeAmount / originAmount * targetAmount, 0, 'g', targetAssetInfo.precision)
                               .arg(targetAssetInfo.symbol).arg(guaranteeId));
        }

        return;
    }
}

void FeeGuaranteeWidget::fetchGuaranteeOrder()
{
    HXChain::getInstance()->postRPC( "FeeGuaranteeWidget-get_guarantee_order-" + trxId, toJsonFormat( "get_guarantee_order", QJsonArray() << guaranteeId ));

}
