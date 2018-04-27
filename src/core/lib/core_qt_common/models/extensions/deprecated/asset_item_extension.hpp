#ifndef ASSET_ITEM_EXTENSION_HPP
#define ASSET_ITEM_EXTENSION_HPP

#include "i_model_extension_old.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_qt_common/i_qt_framework.hpp"

namespace wgt
{
class IQtHelpers;

class AssetItemExtension : public IModelExtensionOld, Depends<IQtFramework, IQtHelpers>
{
public:
	virtual ~AssetItemExtension();

	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	void onDataChanged(const QModelIndex& index, int role, const QVariant& value) override;
};
} // end namespace wgt
#endif // ASSET_ITEM_EXTENSION_HPP
