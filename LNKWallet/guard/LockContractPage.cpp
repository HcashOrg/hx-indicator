#include "LockContractPage.h"
#include "ui_LockContractPage.h"

#include "wallet.h"

LockContractPage::LockContractPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LockContractPage)
{
    ui->setupUi(this);

    ui->lockBtn->setStyleSheet(TOOLBUTTON_STYLE_1);
    ui->withdrawBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    init();
}

LockContractPage::~LockContractPage()
{
    delete ui;
}

void LockContractPage::init()
{
    ui->accountComboBox->clear();
    QStringList accounts = HXChain::getInstance()->getMyGuards();
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

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->label_back);

}

void LockContractPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}
