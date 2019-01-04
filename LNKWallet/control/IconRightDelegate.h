#ifndef ICONRIGHTDELEGATE_H
#define ICONRIGHTDELEGATE_H

#include <QStyledItemDelegate>

class IconRightDelegate : public QStyledItemDelegate{
public:
    using QStyledItemDelegate::QStyledItemDelegate;
protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;
 };

#endif // ICONRIGHTDELEGATE_H
