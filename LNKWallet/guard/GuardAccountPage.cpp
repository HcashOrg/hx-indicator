#include "GuardAccountPage.h"
#include "ui_GuardAccountPage.h"

#include "wallet.h"
#include "CreateSenatorDialog.h"

GuardAccountPage::GuardAccountPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GuardAccountPage)
{
    ui->setupUi(this);
    ui->newSenatorBtn->setStyleSheet(TOOLBUTTON_STYLE_1);

    init();
}

GuardAccountPage::~GuardAccountPage()
{
    delete ui;
}

void GuardAccountPage::init()
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
        ui->guardLabel->hide();
        ui->label->hide();
        ui->accountComboBox->hide();

        QLabel* label = new QLabel(this);
        label->setGeometry(QRect(ui->label->pos(), QSize(300,18)));
        label->setText(tr("There are no senator accounts in the wallet."));
    }

    HXChain::getInstance()->mainFrame->installBlurEffect(ui->label_back);
}

void GuardAccountPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    GuardInfo info = HXChain::getInstance()->allGuardMap.value(ui->accountComboBox->currentText());

    ui->guardIdLabel->setText(info.guardId);
    ui->addressLabel->setText(info.address);

    if(info.isFormal)
    {
        ui->guardTypeLabel->setText(tr("formal senator"));
        ui->guardLabel->setPixmap(QPixmap(":/ui/wallet_ui/guard_formal.png"));
    }
    else
    {
        ui->guardTypeLabel->setText(tr("informal senator"));
        ui->guardLabel->setPixmap(QPixmap(":/ui/wallet_ui/guard_normal.png"));
    }
}

void GuardAccountPage::on_newSenatorBtn_clicked()
{
    CreateSenatorDialog dia;
    dia.exec();
}

void GuardAccountPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}
