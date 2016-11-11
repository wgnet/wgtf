#ifndef COLUMN_EXTENSION_OLD_HPP
#define COLUMN_EXTENSION_OLD_HPP

#include "i_model_extension_old.hpp"
#include <memory>

namespace wgt
{
class ColumnExtensionOld : public IModelExtensionOld
{
public:
	ColumnExtensionOld();
	virtual ~ColumnExtensionOld();

	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                              QAbstractItemModel::LayoutChangeHint hint) override;
	void onLayoutChanged(const QList<QPersistentModelIndex>& parents,
	                     QAbstractItemModel::LayoutChangeHint hint) override;
	void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last) override;
	void onRowsRemoved(const QModelIndex& parent, int first, int last) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // COLUMN_EXTENSION_OLD_HPP
