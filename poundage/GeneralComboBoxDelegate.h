#ifndef GeneralComboBoxDelegate_h__
#define GeneralComboBoxDelegate_h__

#include <QStyledItemDelegate>

//////////////////////////////////////////////////////////////////////////
///<summary> 通用comboBox代理 </summary>
///
///<remarks> 朱正天,2017/11/9. </remarks>
///////////////////////////////////////////////////////////////////////////*/

class  GeneralComboBoxDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	//<summary> 数据源为 显示的string及对应的枚举值组成的vector 2017/11/9--朱正天 </summary> 
	explicit GeneralComboBoxDelegate(std::vector<std::pair<std::string,int>> dataList = std::vector<std::pair<std::string, int>>(), 
									 QObject *parent = nullptr);
	~GeneralComboBoxDelegate();
public:
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	class GeneralPrivate;
	GeneralPrivate *_p;
};
#endif // GeneralComboBoxDelegate_h__
