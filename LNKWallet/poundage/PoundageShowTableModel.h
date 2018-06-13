#ifndef POUNDAGESHOWTABLEMODEL_H
#define POUNDAGESHOWTABLEMODEL_H

#include <memory>
#include <QAbstractTableModel>

#include "PoundageDataUtil.h"
class PoundageShowTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PoundageShowTableModel(QObject *parent = 0);
    ~PoundageShowTableModel();

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
public:
    void InitData(const std::shared_ptr<PoundageSheet> &sheet);

    int GetTotalNumber()const;
    int GetCurrentPage()const;
    void SetCurrentPage(int currPage);
    void SetSinglePageRow(int rowNumber);
    int GetSinglePageRow()const;
    int GetMaxPage()const;
private:
    void RefreshModel();
    void adjustMaxPage();
private:
    class PoundageShowTableModelPrivate;
    PoundageShowTableModelPrivate *_p;

    std::shared_ptr<PoundageSheet> sheetData;
};

#endif // POUNDAGESHOWTABLEMODEL_H
