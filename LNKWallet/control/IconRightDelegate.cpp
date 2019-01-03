#include "IconRightDelegate.h"

void IconRightDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->decorationPosition = QStyleOptionViewItem::Right;
}
