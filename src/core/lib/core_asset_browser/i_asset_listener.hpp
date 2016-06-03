#ifndef I_ASSET_LISTENER_HPP
#define I_ASSET_LISTENER_HPP

namespace wgt
{
class IAssetListener
{
public:

	IAssetListener() {}
	virtual ~IAssetListener() {}

	virtual void useAsset( const char* assetPath ) = 0;
};
} // end namespace wgt
#endif // I_ASSET_LISTENER_HPP
