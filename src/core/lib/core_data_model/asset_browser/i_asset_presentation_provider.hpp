#ifndef I_ASSET_PRESENTATION_PROVIDER_HPP
#define I_ASSET_PRESENTATION_PROVIDER_HPP

#include "core_data_model/i_item.hpp"

namespace wgt
{
class IAssetObjectItem;

/**
 IAssetPresentationProviderOld
 An interface to a class that can provide the specific presentation data for studio assets. Each studio or plugin
 will need to implement a provider that will interpret asset data per their specifications (e.g. file type icons,
 thumbnail generation/rendering routines, etc.).

 NOTE: This class is a temporary measure to provide presentation support. Once the data models have been cleaned
       up to remove file system dependencies, this will all be moved back into custom IAssetObjectItem implementations.
 */
class IAssetPresentationProvider
{
public:
	IAssetPresentationProvider()
	{
	}
	virtual ~IAssetPresentationProvider()
	{
	}

	virtual ThumbnailData getThumbnail(const IAssetObjectItem* asset) = 0;
	virtual ThumbnailData getStatusIconData(const IAssetObjectItem* asset) = 0;
	virtual const char* getTypeIconResourceString(const IAssetObjectItem* asset) const = 0;
};
} // end namespace wgt
#endif // I_ASSET_PRESENTATION_PROVIDER_HPP
