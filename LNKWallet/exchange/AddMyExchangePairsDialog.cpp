#include "AddMyExchangePairsDialog.h"
#include "ui_AddMyExchangePairsDialog.h"

#include "wallet.h"
#include "control/CheckExchangePairWidget.h"

AddMyExchangePairsDialog::AddMyExchangePairsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMyExchangePairsDialog)
{
    ui->setupUi(this);

    setParent(HXChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);

    ui->okBtn->setStyleSheet(OKBTN_STYLE);
    ui->cancelBtn->setStyleSheet(CANCELBTN_STYLE);

    on_marketBtn1_clicked();
}

AddMyExchangePairsDialog::~AddMyExchangePairsDialog()
{
    delete ui;
}

void AddMyExchangePairsDialog::pop()
{
    move(0,0);
    exec();
}

void AddMyExchangePairsDialog::on_marketBtn1_clicked()
{
    showPairsByQuoteAsset("HX");
}

void AddMyExchangePairsDialog::on_marketBtn2_clicked()
{
    showPairsByQuoteAsset("BTC");
}

void AddMyExchangePairsDialog::on_marketBtn3_clicked()
{
    showPairsByQuoteAsset("ERCPAX");
}

void AddMyExchangePairsDialog::showPairsByQuoteAsset(QString quoteAsset)
{
    // 关闭现有的 pairWidget
    foreach (CheckExchangePairWidget* w, pairWidgetList)
    {
        if(w)   w->close();
    }
    pairWidgetList.clear();

    QList<ExchangePair> pairs = HXChain::getInstance()->getExchangePairsByQuoteAsset(quoteAsset);
    int count = 0;
    foreach (const ExchangePair& pair, pairs)
    {
        CheckExchangePairWidget* w = new CheckExchangePairWidget(ui->scrollAreaWidgetContents);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setPair(pair);
        w->setChecked( HXChain::getInstance()->isMyFavoritePair(pair));
        w->move( (count % 3) * 125, count / 3 * 40);
        w->show();

        pairWidgetList.append(w);
        count++;
    }

    if(count < 7)
    {
        ui->scrollAreaWidgetContents->setMinimumHeight(78);
    }
    else
    {
        ui->scrollAreaWidgetContents->setMinimumHeight( (int((count - 0.1) / 3) + 1) * 40 - 2);
    }
}

void AddMyExchangePairsDialog::on_okBtn_clicked()
{

}

void AddMyExchangePairsDialog::on_cancelBtn_clicked()
{
    close();
}

void AddMyExchangePairsDialog::onCheckStateChanged(ExchangePair pair, bool isChecked)
{

}
