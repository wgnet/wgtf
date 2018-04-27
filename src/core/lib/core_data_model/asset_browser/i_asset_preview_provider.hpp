#ifndef I_ASSET_PREVIEW_PROVIDER_HPP
#define I_ASSET_PREVIEW_PROVIDER_HPP

namespace wgt
{
class ObjectHandle;
class IAssetPreviewProvider
{
public:
	virtual ~IAssetPreviewProvider()
	{
	}

	virtual ObjectHandle getPreview(const char* filePath) = 0;

};
} // end namespace wgt
#endif // I_ASSET_PREVIEW_PROVIDER_HPP
