#ifndef I_ASSET_BROWSER_MANAGER_HPP
#define I_ASSET_BROWSER_MANAGER_HPP

namespace wgt
{
class IAssetListener;
class IComponentContext;

class IAssetBrowserManager
{
public:
	IAssetBrowserManager()
	{
	}
	virtual ~IAssetBrowserManager()
	{
	}

	virtual void initialise(IComponentContext& context) = 0;
	virtual void registerListener(IAssetListener* listener) = 0;
	virtual void createView(IComponentContext& context) = 0;
};
} // end namespace wgt
#endif // I_ASSET_BROWSER_MANAGER_HPP
