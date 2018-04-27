#ifndef COLUMN_EXTENSION_HPP
#define COLUMN_EXTENSION_HPP

#include "qt_model_extension.hpp"
#include <memory>

namespace wgt
{
class ColumnExtension : public QtModelExtension
{
public:
	ColumnExtension();
	virtual ~ColumnExtension();

	QVariant data(const QModelIndex& index, ItemRole::Id roleId) const override;

	void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                              QAbstractItemModel::LayoutChangeHint hint) override;
	void onLayoutChanged(const QList<QPersistentModelIndex>& parents,
	                     QAbstractItemModel::LayoutChangeHint hint) override;
	void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last) override;
	void onRowsRemoved(const QModelIndex& parent, int first, int last) override;
	void onModelReset() override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // COLUMN_EXTENSION_HPP
