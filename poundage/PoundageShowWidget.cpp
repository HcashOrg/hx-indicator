#include "PoundageShowWidget.h"
#include "ui_PoundageShowWidget.h"

#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QPainter>

#include "PoundageShowTableModel.h"
#include "GeneralComboBoxDelegate.h"
#include "PageScrollWidget.h"
#include "wallet.h"
#include "showcontentdialog.h"
#include "control/BlankDefaultWidget.h"

Q_DECLARE_METATYPE(std::shared_ptr<PoundageUnit>)

class PoundageShowWidget::PoundageShowWidgetPrivate
{
public:
    PoundageShowWidgetPrivate()
        :tableModel(new PoundageShowTableModel())
        ,isContextMenuEnabled(true)
        ,contextMenu(nullptr)
        ,pageWidget(new PageScrollWidget())
        ,isDeleteActionEnabled(false)
        ,isDefaultActionEnabled(false)
        ,deleteAction(nullptr)
        ,defaultAction(nullptr)
        ,blankWidget(nullptr)
    {

    }
public:
    PoundageShowTableModel *tableModel;
    PageScrollWidget *pageWidget;
    bool isContextMenuEnabled;
    QMenu *contextMenu;
    QAction *defaultAction;
    QAction *deleteAction;
    bool isDeleteActionEnabled;
    bool isDefaultActionEnabled;

    BlankDefaultWidget *blankWidget;
};

PoundageShowWidget::PoundageShowWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PoundageShowWidget),
    _p(new PoundageShowWidgetPrivate())
{
    ui->setupUi(this);

    _p->blankWidget = new BlankDefaultWidget(ui->tableView);
    _p->blankWidget->setTextTip(tr("当前无承兑单!"));
    InitWidget();
}

PoundageShowWidget::~PoundageShowWidget()
{
    delete ui;
}

void PoundageShowWidget::InitData(const std::shared_ptr<PoundageSheet> &data)
{
    _p->tableModel->InitData(data);
    _p->pageWidget->SetTotalPage(_p->tableModel->GetMaxPage()+1);
    if(data)
    {
        _p->pageWidget->setShowTip(data->poundages.size(),static_cast<unsigned int>(_p->tableModel->GetSinglePageRow()));
    }
    _p->blankWidget->setVisible(data->size() == 0);
}

void PoundageShowWidget::DeletePoundageSlots()
{
    emit DeletePoundageSignal(ui->tableView->currentIndex().data(Qt::UserRole).value<std::shared_ptr<PoundageUnit>>()->poundageID
                              ,UBChain::getInstance()->addressToName(ui->tableView->currentIndex().data(Qt::UserRole).value<std::shared_ptr<PoundageUnit>>()->ownerAdress));
}

void PoundageShowWidget::SetDefaultPoundageSlots()
{
    emit SetDefaultPoundageSignal(ui->tableView->currentIndex().data(Qt::UserRole).value<std::shared_ptr<PoundageUnit>>()->poundageID);
}

void PoundageShowWidget::EnableContextMenu(bool enable)
{
    _p->isContextMenuEnabled = enable;
}

void PoundageShowWidget::EnableDeleteAction(bool enable)
{
    _p->isDeleteActionEnabled = enable;
}

void PoundageShowWidget::EnalbeDefaultAction(bool enable)
{
    _p->isDefaultActionEnabled = enable;
}

void PoundageShowWidget::changeCurrentPageSlots(unsigned int currentPage)
{
    _p->tableModel->SetCurrentPage(static_cast<unsigned int>(currentPage));
}

void PoundageShowWidget::InitWidget()
{
    InitStyle();
    InitContextMenu();

    ui->tableView->setModel(_p->tableModel);
    ui->tableView->hideColumn(3);
//    ui->tableView->hideColumn(2);
    ui->tableView->hideColumn(6);

    ui->stackedWidget_pageBar->addWidget(_p->pageWidget);
    ui->stackedWidget_pageBar->setCurrentWidget(_p->pageWidget);
    connect(_p->pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&PoundageShowWidget::changeCurrentPageSlots);
}

void PoundageShowWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

//    ui->tableView->setPalette(palette);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setMinimumSectionSize(40);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setShowGrid(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableView->setStyleSheet(TABLEWIDGET_STYLE_1);
}

void PoundageShowWidget::InitContextMenu()
{
   _p->contextMenu = new QMenu();
   _p->contextMenu->setStyleSheet(MENU_STYLE);
   _p->defaultAction = new QAction(tr("setDefault"),this);
   _p->deleteAction = new QAction(tr("delete"),this);

   _p->contextMenu->setAttribute(Qt::WA_TranslucentBackground, true);
   _p->contextMenu->setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | _p->contextMenu->windowFlags());

   connect(_p->defaultAction,&QAction::triggered,this,&PoundageShowWidget::SetDefaultPoundageSlots);
   connect(_p->deleteAction,&QAction::triggered,this,&PoundageShowWidget::DeletePoundageSlots);
}

void PoundageShowWidget::RefreshMenu()
{
    _p->contextMenu->clear();
    //_p->contextMenu->addAction(_p->defaultAction);

    if(_p->isDeleteActionEnabled)
    {
        _p->contextMenu->addAction(_p->deleteAction);
    }
    if(_p->isDefaultActionEnabled)
    {
        _p->contextMenu->addAction(_p->defaultAction);
    }
}

void PoundageShowWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if(!ui->tableView->currentIndex().isValid() || !_p->isContextMenuEnabled
       || !ui->tableView->selectionModel()->isSelected(ui->tableView->currentIndex())) return;
    std::shared_ptr<PoundageUnit> unit = ui->tableView->currentIndex().data(Qt::UserRole).value<std::shared_ptr<PoundageUnit>>();
    if(!unit) return;

    RefreshMenu();
    _p->contextMenu->exec(QCursor::pos());
}

void PoundageShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.save();
    const qreal radius = 10;
    //QPainterPath path;
    //QRectF rect = QRect(50, 5, 670, 270);
    QRectF rect1 = QRect(40,0,696,310);

    QRadialGradient radial(385, 385, 770, 385,385);
    radial.setColorAt(0, QColor(198,202,212,10));
    radial.setColorAt(1, QColor(218,255,248,10));

    painter.setBrush(radial);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect1,radius,radius);

    painter.restore();
    QWidget::paintEvent(event);
}

void PoundageShowWidget::on_tableView_clicked(const QModelIndex &index)
{
    if( index.column() == 2)
    {
        ShowContentDialog showContentDialog( index.data(Qt::DisplayRole).toString(),this);

        int x = ui->tableView->columnViewportPosition(index.column()) + ui->tableView->columnWidth(index.column()) / 2
                - showContentDialog.width() / 2;
        int y = ui->tableView->rowViewportPosition(index.row()) - 10 + ui->tableView->horizontalHeader()->height();

        showContentDialog.move( ui->tableView->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}
