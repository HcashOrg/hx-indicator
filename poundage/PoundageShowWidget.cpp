#include "PoundageShowWidget.h"
#include "ui_PoundageShowWidget.h"

#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>

#include "PoundageShowTableModel.h"
#include "GeneralComboBoxDelegate.h"

Q_DECLARE_METATYPE(std::shared_ptr<PoundageUnit>)

class PoundageShowWidget::PoundageShowWidgetPrivate
{
public:
    PoundageShowWidgetPrivate()
        :tableModel(new PoundageShowTableModel())
        ,isContextMenuEnabled(false)
        ,contextMenu(nullptr)
    {

    }
public:
    PoundageShowTableModel *tableModel;
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
    RefreshNumber();
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

void PoundageShowWidget::RefreshNumber()
{
    QIntValidator *validator = new QIntValidator(1,_p->tableModel->GetMaxPage()+1,this);
    ui->pageLineEdit->setValidator( validator );
    ui->pageLabel->setText("/ "+QString::number(_p->tableModel->GetMaxPage()+1));
    ui->numberLabel->setText(QString::number(_p->tableModel->GetTotalNumber()));
    ui->pageLineEdit->setText(QString::number(_p->tableModel->GetCurrentPage()+1));
}

void PoundageShowWidget::GotoFirstPageSlots()
{
    _p->tableModel->SetCurrentPage(0);
    RefreshLineEdit();
}

void PoundageShowWidget::GotoLastPageSlots()
{
    _p->tableModel->SetCurrentPage(_p->tableModel->GetMaxPage());
    RefreshLineEdit();
}

void PoundageShowWidget::GotoNextPageSlots()
{
    _p->tableModel->SetCurrentPage(_p->tableModel->GetCurrentPage()==_p->tableModel->GetMaxPage()?0:_p->tableModel->GetCurrentPage()+1);
    RefreshLineEdit();
}

void PoundageShowWidget::GotoPrePageSlots()
{
    _p->tableModel->SetCurrentPage(_p->tableModel->GetCurrentPage()==0?_p->tableModel->GetMaxPage():_p->tableModel->GetCurrentPage()-1);
    RefreshLineEdit();
}

void PoundageShowWidget::GotoLineEditSlots()
{
    _p->tableModel->SetCurrentPage(ui->pageLineEdit->text().toInt()-1);
    RefreshLineEdit();
}

void PoundageShowWidget::RefreshLineEdit()
{
    ui->pageLineEdit->setText(QString::number(_p->tableModel->GetCurrentPage()+1));
}

void PoundageShowWidget::InitWidget()
{
    InitStyle();
    InitContextMenu();

    ui->tableView->setModel(_p->tableModel);
    ui->tableView->hideColumn(3);
    ui->tableView->hideColumn(2);

    ui->numberLabel->hide();

    ui->pageLineEdit->setText("1");
    QIntValidator *validator = new QIntValidator(1,1,this);
    ui->pageLineEdit->setValidator( validator );
    ui->pageLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    connect(ui->firstPageBtn,&QToolButton::clicked,this,&PoundageShowWidget::GotoFirstPageSlots);
    connect(ui->lastPageBtn,&QToolButton::clicked,this,&PoundageShowWidget::GotoLastPageSlots);
    connect(ui->nextPageBtn,&QToolButton::clicked,this,&PoundageShowWidget::GotoNextPageSlots);
    connect(ui->prePageBtn,&QToolButton::clicked,this,&PoundageShowWidget::GotoPrePageSlots);
    connect(ui->pageLineEdit,&QLineEdit::editingFinished,this,&PoundageShowWidget::GotoLineEditSlots);
}

void PoundageShowWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(40,46,66));
    setPalette(palette);


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


    ui->firstPageBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/firstPage.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                    "QToolButton:hover{background-image:url(:/ui/wallet_ui/firstPage_hover.png);}");
    ui->prePageBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/prePage.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                  "QToolButton:hover{background-image:url(:/ui/wallet_ui/prePage_hover.png);}");
    ui->nextPageBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/nextPage.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                   "QToolButton:hover{background-image:url(:/ui/wallet_ui/nextPage_hover.png);}");
    ui->lastPageBtn->setStyleSheet("QToolButton{background-image:url(:/ui/wallet_ui/lastPage.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"
                                   "QToolButton:hover{background-image:url(:/ui/wallet_ui/lastPage_hover.png);}");

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
