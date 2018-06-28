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
    QStringList accounts = UBChain::getInstance()->getMyFormalGuards();
    ui->accountComboBox->addItems(accounts);

    if(accounts.contains(UBChain::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(UBChain::getInstance()->currentAccount);
    }

    UBChain::getInstance()->mainFrame->installBlurEffect(ui->label_back);
}

void GuardAccountPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    GuardInfo info = UBChain::getInstance()->formalGuardMap.value(ui->accountComboBox->currentText());

    ui->guardIdLabel->setText(info.guardId);
    ui->addressLabel->setText(info.address);

    QStringList formalGuards = UBChain::getInstance()->formalGuardMap.keys();
    QStringList normalGuards = UBChain::getInstance()->allGuardMap.keys();
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
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}
