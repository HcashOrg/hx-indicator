#include "ExchangePairSelectDialog.h"
#include "ui_ExchangePairSelectDialog.h"

#include <QPaintEvent>
#include <QHeaderView>

#include "ExchangeSinglePairCellWidget.h"
#include "ExchangeModeWidget.h"

ExchangePairSelectDialog::ExchangePairSelectDialog(bool _showAddBtn, QWidget *parent) :
    QDialog(parent),
    showAddBtn(_showAddBtn),
    ui(new Ui::ExchangePairSelectDialog)
{
    ui->setupUi(this);


    setWindowFlags( Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground, true);

    tableWidget = new QTableWidget(this);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setFocusPolicy(Qt::NoFocus);
//    tableWidget->setFrameShape(QFrame::NoFrame);
    tableWidget->setMouseTracking(true);
    tableWidget->setShowGrid(false);//隐藏表格线
    tableWidget->horizontalHeader()->setVisible(false);

    tableWidget->setStyleSheet("QTableView{background-color:white;border:none;border-radius:0px;font: 11px \"微软雅黑\";color:rgb(52,37,90);}"
                               "QHeaderView{}"
                               "QScrollBar:vertical{border:none; width:2px;background:transparent;padding:0px 0px 0px 0px;}"
                               "QScrollBar::handle:vertical{width:2px;background:rgb(202,196,223);border-radius:0px;min-height:14px;}"
                              // "QScrollBar::handle:vertical:hover{background:rgb(103,81,158);}"
                               "QScrollBar::add-line:vertical{height:0px;}"
                               "QScrollBar::sub-line:vertical{height:0px;}"
                               "QScrollBar::sub-page:vertical {background: transparent;}"
                               "QScrollBar::add-page:vertical {background: transparent;}");

    tableWidget->setColumnCount(1);
    tableWidget->setColumnWidth(0,110);
    connect(tableWidget, &QTableWidget::cellClicked, this, &ExchangePairSelectDialog::onCellWidgetClicked);

    if(showAddBtn)
    {
        ui->addBtn->show();
        tableWidget->setGeometry(7,40,110,90);
    }
    else
    {
        ui->addBtn->hide();
        tableWidget->setGeometry(7,10,110,120);
    }

    showPairs();
}

ExchangePairSelectDialog::~ExchangePairSelectDialog()
{
    delete ui;
}

void ExchangePairSelectDialog::on_addBtn_clicked()
{

}

void ExchangePairSelectDialog::showPairs()
{
    HXChain::getInstance()->configFile->beginGroup("/FavoriteOrderPairs");
    QStringList keys = HXChain::getInstance()->configFile->childKeys();
    HXChain::getInstance()->configFile->endGroup();

    tableWidget->setRowCount(0);
    tableWidget->setRowCount(keys.size());

    for(int i = 0; i < keys.size(); i++)
    {

        ExchangeSinglePairCellWidget* cellWidget = new ExchangeSinglePairCellWidget;
        tableWidget->setCellWidget( i, 0, cellWidget);

        QStringList strList = keys.at(i).split("+");
        cellWidget->setPair( qMakePair(strList.at(0), strList.at(1)));
    }

}

void ExchangePairSelectDialog::onCellWidgetClicked(int _row, int _column)
{
    ExchangeSinglePairCellWidget* cellWidget = static_cast<ExchangeSinglePairCellWidget*>(tableWidget->cellWidget(_row,_column));
    if(cellWidget)
    {
        Q_EMIT pairSelected(cellWidget->pair);
    }

    close();
}

bool ExchangePairSelectDialog::event(QEvent *event)
{
    static bool class_amended = false;
    if (event->type() == QEvent::WinIdChange)
    {
        HWND hwnd = reinterpret_cast<HWND>(winId());
        if (class_amended == false)
        {
            class_amended = true;
            DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
            class_style &= ~CS_DROPSHADOW;
            ::SetClassLong(hwnd, GCL_STYLE, class_style); // windows系统函数
        }
    }
    return QWidget::event(event);
}

void ExchangePairSelectDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,124,139,QPixmap(":/ui/wallet_ui/dialogBorder.png").scaled(124,139));

    QWidget::paintEvent(event);
}

