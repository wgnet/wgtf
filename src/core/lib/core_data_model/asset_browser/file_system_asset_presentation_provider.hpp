#ifndef FILE_SYSTEM_PRESENTATION_PROVIDER_HPP
#define FILE_SYSTEM_PRESENTATION_PROVIDER_HPP

#include "i_asset_presentation_provider.hpp"

#include <memory>
#include <map>


namespace wgt
{
class FileSystemAssetPresentationProvider : public IAssetPresentationProvider
{
public:
	FileSystemAssetPresentationProvider() {}
	virtual ~FileSystemAssetPresentationProvider() {}

	// Pre-generate the binary block data for the thumbnails and status icons so we aren't creating them 
	// every time an asset is evaluated for its thumbnail needs.
	void generateData();

	// IAssetPresentationProvider Implementation
	virtual ThumbnailData getThumbnail(const IAssetObjectItem * asset) override;
	virtual ThumbnailData getStatusIconData(const IAssetObjectItem * asset) override;
	virtual const char* getTypeIconResourceString(const IAssetObjectItem * asset) const override;

private:
	// Enumeration of thumbnail files we've cached
	enum CachedThumbnails
	{
		CACHED_DEFAULT = 0,
		CACHED_SCRIPT = 1,
		CACHED_XML = 2,
		CACHED_DECAL = 3,
	};

	// Returns a binary block of data for the designated file
	ThumbnailData getBinaryDataFromFile(const char * filename);

	// Reads the thumbnail file and creates the BinaryBlock data to cache away
	void addThumbnail(int index, const char * filename);

private:

	// Cached thumbnail data for easy retrieval
	std::map< unsigned int, ThumbnailData > testThumbnails_;
	ThumbnailData testStatusIcon_;
};
} // end namespace wgt
#endif // FILE_SYSTEM_PRESENTATION_PROVIDER_HPP
