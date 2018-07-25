#include "GuardAccountPage.h"
#include "ui_GuardAccountPage.h"

#include "wallet.h"

GuardAccountPage::GuardAccountPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GuardAccountPage)
{
    ui->setupUi(this);

    init();
}

GuardAccountPage::~GuardAccountPage()
{
    delete ui;
}

void GuardAccountPage::init()
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
        ui->guardLabel->hide();
        ui->label->hide();
        ui->accountComboBox->hide();

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->accountComboBox->pos(), QSize(300,30)));
        label->setText(tr("There are no guard accounts in the wallet."));
    }

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->label_back);
}

void GuardAccountPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    GuardInfo info = HXChain::getInstance()->formalGuardMap.value(ui->accountComboBox->currentText());

    ui->guardIdLabel->setText(info.guardId);
    ui->addressLabel->setText(info.address);

    QStringList formalGuards = HXChain::getInstance()->formalGuardMap.keys();
    QStringList normalGuards = HXChain::getInstance()->allGuardMap.keys();
    if(formalGuards.contains(ui->accountComboBox->currentText()))
    {
        ui->guardTypeLabel->setText(tr("formal guard"));
        ui->guardLabel->setPixmap(QPixmap(":/ui/wallet_ui/guard_formal.png"));
    }
    else if(normalGuards.contains(ui->accountComboBox->currentText()))
    {
        ui->guardTypeLabel->setText(tr("informal guard"));
        ui->guardLabel->setPixmap(QPixmap(":/ui/wallet_ui/guard_normal.png"));
    }
}

void GuardAccountPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}
