#ifndef ASSET_ITEM_EXTENSION_HPP
#define ASSET_ITEM_EXTENSION_HPP

#include "i_model_extension.hpp"

namespace wgt
{
class IQtFramework;

class AssetItemExtension : public IModelExtension
{
public:
	AssetItemExtension();
	virtual ~AssetItemExtension();

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index, int role ) const override;
	bool setData( const QModelIndex &index, const QVariant &value, int role ) override;

	void onDataChanged( const QModelIndex &index, int role, const QVariant &value ) override;

private:
	IQtFramework * qtFramework_;
};
} // end namespace wgt
#endif // ASSET_ITEM_EXTENSION_HPP
