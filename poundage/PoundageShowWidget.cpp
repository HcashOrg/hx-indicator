#include "PoundageShowWidget.h"
#include "ui_PoundageShowWidget.h"

#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>

#include "PoundageShowTableModel.h"
#include "GeneralComboBoxDelegate.h"
#include "PageScrollWidget.h"

Q_DECLARE_METATYPE(std::shared_ptr<PoundageUnit>)

class PoundageShowWidget::PoundageShowWidgetPrivate
{
public:
    PoundageShowWidgetPrivate()
        :tableModel(new PoundageShowTableModel())
        ,isContextMenuEnabled(false)
        ,contextMenu(nullptr)
        ,pageWidget(new PageScrollWidget(5))
    {

    }
public:
    PoundageShowTableModel *tableModel;
    PageScrollWidget *pageWidget;
    bool isContextMenuEnabled;
    QMenu *contextMenu;
};

PoundageShowWidget::PoundageShowWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PoundageShowWidget),
    _p(new PoundageShowWidgetPrivate())
{
    ui->setupUi(this);
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
}

void PoundageShowWidget::DeletePoundageSlots()
{
    emit DeletePoundageSignal(ui->tableView->currentIndex().data(Qt::UserRole).value<std::shared_ptr<PoundageUnit>>()->poundageID);
}

void PoundageShowWidget::SetDefaultPoundageSlots()
{
    emit SetDefaultPoundageSignal(ui->tableView->currentIndex().data(Qt::UserRole).value<std::shared_ptr<PoundageUnit>>()->poundageID);
}

void PoundageShowWidget::EnableContextMenu(bool enable)
{
     _p->isContextMenuEnabled = enable;
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
    ui->tableView->hideColumn(2);

    ui->stackedWidget_pageBar->addWidget(_p->pageWidget);
    ui->stackedWidget_pageBar->setCurrentWidget(_p->pageWidget);
    connect(_p->pageWidget,&PageScrollWidget::currentPageChangeSignal,this,&PoundageShowWidget::changeCurrentPageSlots);
}

void PoundageShowWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,249,253));
    setPalette(palette);

    ui->tableView->setPalette(palette);
//    QHeaderView *header = new QHeaderView(Qt::Horizontal,ui->tableView);
//    header->setModel(_p->tableModel);

//    std::vector<std::pair<std::string,int>> data;
//    data.push_back(std::make_pair("111",1));
//    data.push_back(std::make_pair("132",2));
//    data.push_back(std::make_pair("122",3));
//    data.push_back(std::make_pair("3543",4));
//    GeneralComboBoxDelegate *dele = new GeneralComboBoxDelegate(data);
//    header->setItemDelegateForColumn(0,dele);
//    header->setSectionResizeMode(QHeaderView::Stretch);
//    header->setSelectionBehavior(QAbstractItemView::SelectItems);
//    header->setSelectionMode(QAbstractItemView::SingleSelection);
//    ui->tableView->setHorizontalHeader(header);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setShowGrid(false);

    ui->tableView->setStyleSheet("QTableView{background-color:#FFFFFF;border:none;border_radius:20px;\
                                    }\
                                  QHeaderView{border:none;color:#C6CAD4;font-size:12pt;}\
                                  QHeaderView:section{border:none;background-color:#FFFFFF;}\
    ");
}

void PoundageShowWidget::InitContextMenu()
{
   _p->contextMenu = new QMenu(this);
   QAction *defaultAction = new QAction(tr("setDefault"),this);
   QAction *deleteAction = new QAction(tr("delete"),this);

   _p->contextMenu->addAction(defaultAction);
   _p->contextMenu->addAction(deleteAction);

   connect(defaultAction,&QAction::triggered,this,&PoundageShowWidget::SetDefaultPoundageSlots);
   connect(deleteAction,&QAction::triggered,this,&PoundageShowWidget::DeletePoundageSlots);
}

void PoundageShowWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if(!ui->tableView->currentIndex().isValid() || !_p->isContextMenuEnabled
       || !ui->tableView->selectionModel()->isSelected(ui->tableView->currentIndex())) return;
    std::shared_ptr<PoundageUnit> unit = ui->tableView->currentIndex().data(Qt::UserRole).value<std::shared_ptr<PoundageUnit>>();
    if(!unit) return;

    _p->contextMenu->exec(QCursor::pos());
}
