#include "FunctionDappWidget.h"
#include "ui_FunctionDappWidget.h"

#include "wallet.h"

FunctionDappWidget::FunctionDappWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionDappWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionDappWidget::~FunctionDappWidget()
{
    delete ui;
}

void FunctionDappWidget::retranslator()
{
    ui->retranslateUi(this);
}

void FunctionDappWidget::DefaultShow()
{
    on_contractTokenBtn_clicked();
}

void FunctionDappWidget::InitWidget()
{
    InitStyle();
    ui->contractTokenBtn->setCheckable(true);
}

void FunctionDappWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(84,116,235));
    setPalette(palette);

    setStyleSheet(FUNCTIONBAR_PUSHBUTTON_STYLE);
}

void FunctionDappWidget::on_contractTokenBtn_clicked()
{
    ui->contractTokenBtn->setChecked(true);

    showContractTokenSignal();
}


void FunctionDappWidget::paintEvent(QPaintEvent *)
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

