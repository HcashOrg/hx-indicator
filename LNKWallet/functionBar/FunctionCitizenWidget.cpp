#include "FunctionCitizenWidget.h"
#include "ui_FunctionCitizenWidget.h"

#include "wallet.h"

FunctionCitizenWidget::FunctionCitizenWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionCitizenWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionCitizenWidget::~FunctionCitizenWidget()
{
    delete ui;
}

void FunctionCitizenWidget::retranslator()
{
    ui->retranslateUi(this);
}

void FunctionCitizenWidget::DefaultShow()
{
    on_accountInfoBtn_clicked();
}

void FunctionCitizenWidget::InitWidget()
{
    InitStyle();
    ui->accountInfoBtn->setCheckable(true);
    ui->proposalBtn->setCheckable(true);
}

void FunctionCitizenWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(84,116,235));
    setPalette(palette);

    setStyleSheet(FUNCTIONBAR_PUSHBUTTON_STYLE);
}

void FunctionCitizenWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QLinearGradient linear(QPointF(0, 480), QPointF(130, 0));
    linear.setColorAt(0, QColor(71,33,84));
    linear.setColorAt(0.5, QColor(55,36,88));
    linear.setColorAt(1, QColor(51,37,90));
    linear.setSpread(QGradient::PadSpread);
    painter.setBrush(linear);
    painter.drawRect(QRect(-1,-1,131,481));
}


void FunctionCitizenWidget::on_accountInfoBtn_clicked()
{
    ui->accountInfoBtn->setChecked(true);
    ui->proposalBtn->setChecked(false);
    showCitizenAccountSignal();
}

void FunctionCitizenWidget::on_proposalBtn_clicked()
{
    ui->accountInfoBtn->setChecked(false);
    ui->proposalBtn->setChecked(true);
    showCitizenProposalSignal();
}
