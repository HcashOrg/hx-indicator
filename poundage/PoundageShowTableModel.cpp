#include "PoundageShowTableModel.h"

#include <QDebug>
#include <QFont>
#include "PoundageDataUtil.h"

Q_DECLARE_METATYPE(std::shared_ptr<PoundageUnit>)

class PoundageShowTableModel::PoundageShowTableModelPrivate
{
public:
    PoundageShowTableModelPrivate()
        :data(std::make_shared<PoundageSheet>())
        ,currentPage(0)
        ,pageMaxRow(6)
        ,maxPage(0)
    {

    }
public:
    std::shared_ptr<PoundageSheet> data;
    int currentPage;
    int pageMaxRow;
    int maxPage;
};

PoundageShowTableModel::PoundageShowTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    ,_p(new PoundageShowTableModelPrivate())
{

}

PoundageShowTableModel::~PoundageShowTableModel()
{
    delete _p;
}

QVariant PoundageShowTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
        {
            if (role == Qt::DisplayRole)
            {
                switch (section)
                {
                case 0:
                    return tr("承兑单id");
                case 1:
                    return tr("交易对象");
                case 2:
                    return tr("发布者地址");
                case 3:
                    return tr("发布者\ ");
                case 4:
                    return tr("汇率");
                case 5:
                    return tr("余额(LNK)");
                case 6:
                    return tr("总额");
                default:
                    break;
                }
            }
            else if (role == Qt::TextAlignmentRole)
            {
                switch (section)
                {
                case 0:
                    return Qt::AlignCenter;
                default:
                    break;
                }
            }

        }
    return QVariant();
}

Qt::ItemFlags PoundageShowTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
    }
    return Qt::ItemIsEnabled;
}

int PoundageShowTableModel::rowCount(const QModelIndex &parent) const
{

    return std::max<int>(0,std::min<int>(_p->pageMaxRow,_p->data->poundages.size() - _p->currentPage * _p->pageMaxRow));
}

int PoundageShowTableModel::columnCount(const QModelIndex &parent) const
{
   return 7;
}

QVariant PoundageShowTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if(index.column() == 0)
            {
                //return _p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->publishTime.toString("yyyy/MM/dd HH:mm");
                return _p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->poundageID;
            }
            else if(index.column() == 1)
            {
                return _p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->chainType+" : "+"LNK";
            }
            else if(index.column() == 2)
            {
                return _p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->ownerAdress;
            }
            else if(index.column() == 3)
            {
                return _p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->ownerAdress;
            }
            else if(index.column() == 4)
            {
                return "1 : "+ QString::number(_p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->sourceCoinNumber/
                        _p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->targetCoinNumber,'f',5);
            }
            else if(index.column() == 5)
            {
                return QString::number(_p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->calSourceLeftNumber(),'f',5);
            }
            else if(index.column() == 6)
            {
                return _p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]->targetCoinNumber;
            }
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ForegroundRole:
            break;
        case Qt::UserRole:
            return QVariant::fromValue<std::shared_ptr<PoundageUnit>>(_p->data->poundages[index.row()+_p->currentPage*_p->pageMaxRow]);
            break;
        default:
            break;
        }
    return QVariant();
}

void PoundageShowTableModel::InitData(const std::shared_ptr<PoundageSheet> &sheet)
{
    if(!sheet) return;
    beginResetModel();
    _p->data = sheet;
    adjustMaxPage();
    _p->currentPage = 0;
    qDebug()<<_p->maxPage;
    endResetModel();
}

int PoundageShowTableModel::GetTotalNumber() const
{
    return _p->data->size();
}

int PoundageShowTableModel::GetCurrentPage() const
{
    return _p->currentPage;
}

void PoundageShowTableModel::RefreshModel()
{
    beginResetModel();
    endResetModel();
}

void PoundageShowTableModel::SetCurrentPage(int currPage)
{
    if(currPage <= _p->maxPage)
    {
        _p->currentPage = currPage;
        RefreshModel();
    }
}

void PoundageShowTableModel::SetSinglePageRow(int rowNumber)
{
    int pageRow = std::max<int>(0,std::min<int>(rowNumber,static_cast<int>(_p->data->poundages.size())));
    if(_p->pageMaxRow != pageRow)
    {
        _p->pageMaxRow = pageRow;
        adjustMaxPage();
        RefreshModel();
    }
}

int PoundageShowTableModel::GetMaxPage() const
{
    return _p->maxPage;
}

void PoundageShowTableModel::adjustMaxPage()
{
    _p->maxPage = static_cast<int>(_p->data->poundages.size()) / _p->pageMaxRow;

    if(static_cast<int>(_p->data->poundages.size())%_p->pageMaxRow == 0 && _p->maxPage > 0)
    {
        --_p->maxPage;
    }
}
