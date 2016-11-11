#ifndef SELECTION_EXTENSION_HPP
#define SELECTION_EXTENSION_HPP

#include "i_model_extension_old.hpp"
#include <memory>

namespace wgt
{
class SelectionExtension : public IModelExtensionOld
{
	Q_OBJECT

	Q_PROPERTY(QVariant selectedIndex READ getSelectedIndex WRITE setSelectedIndex NOTIFY selectionChanged)

	Q_PROPERTY(QVariant selectedItem READ getSelectedItem)

	Q_PROPERTY(bool multiSelect READ getMultiSelect WRITE setMultiSelect NOTIFY multiSelectChanged)

	Q_PROPERTY(QVariant currentIndex READ getCurrentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
	SelectionExtension();
	virtual ~SelectionExtension();

	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	void setSelectedIndex(const QVariant& index);

	void onDataAboutToBeChanged(const QModelIndex& index, int role, const QVariant& value) override;
	void onDataChanged(const QModelIndex& index, int role, const QVariant& value) override;

	void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last) override;
	void onRowsRemoved(const QModelIndex& parent, int first, int last) override;

	virtual void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                                      QAbstractItemModel::LayoutChangeHint hint) override;
	virtual void onLayoutChanged(const QList<QPersistentModelIndex>& parents,
	                             QAbstractItemModel::LayoutChangeHint hint) override;

	Q_INVOKABLE void clearOnNextSelect();
	Q_INVOKABLE void prepareRangeSelect();
	Q_INVOKABLE QList<QVariant> getSelection() const;
	Q_INVOKABLE bool moveUp();
	Q_INVOKABLE bool moveDown();
	Q_INVOKABLE bool indexInSelection(const QVariant& index);

signals:
	void selectionChanged();
	void multiSelectChanged();
	void currentIndexChanged();

private:
	QVariant getSelectedIndex() const;
	QVariant getSelectedItem() const;

	bool getMultiSelect() const;
	void setMultiSelect(bool value);

	QVariant getCurrentIndex() const;
	void setCurrentIndex(const QVariant& index);

	void selectCurrentIndex(bool select);

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // SELECTION_EXTENSION_HPP
