#ifndef I_ASSET_MANAGER_HPP
#define I_ASSET_MANAGER_HPP

namespace wgt
{
class AbstractItemModel;

class IAssetManager
{
public:
	IAssetManager()
	{
	}
	virtual ~IAssetManager()
	{
	}

	virtual AbstractItemModel* assetModel() const = 0;
};
} // end namespace wgt
#endif // I_ASSET_MANAGER_HPP
