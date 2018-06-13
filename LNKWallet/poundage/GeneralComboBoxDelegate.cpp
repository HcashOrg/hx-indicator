#include "GeneralComboBoxDelegate.h"

#include <QtWidgets/QComboBox>
#include <algorithm>

class GeneralComboBoxDelegate::GeneralPrivate
{
public:
	GeneralPrivate()
	{

	}

public:
	std::vector<std::pair<std::string, int>> dataList;//สพิด
};

GeneralComboBoxDelegate::GeneralComboBoxDelegate(std::vector<std::pair<std::string, int>> dataList, QObject *parent)
	: QStyledItemDelegate(parent)
	,_p(new GeneralPrivate())
{
}

GeneralComboBoxDelegate::~GeneralComboBoxDelegate()
{
	delete _p;
}

QWidget * GeneralComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QComboBox *editor = new QComboBox(parent);
	editor->setFrame(false);
	std::for_each(_p->dataList.begin(), _p->dataList.end(), [&editor](const std::pair<std::string,int> data) {
		editor->addItem(data.first.c_str(),QVariant::fromValue<int>(data.second));
	});
	return editor;
}

void GeneralComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	int value = index.model()->data(index, Qt::EditRole).value<int>();
	
	comboBox->setCurrentIndex(comboBox->findData(QVariant::fromValue<int>(value),Qt::UserRole));
}

void GeneralComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	int value = comboBox->itemData(comboBox->currentIndex(),Qt::UserRole).value<int>();
	model->setData(index, QVariant::fromValue<int>(value), Qt::EditRole);
}

void GeneralComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}
