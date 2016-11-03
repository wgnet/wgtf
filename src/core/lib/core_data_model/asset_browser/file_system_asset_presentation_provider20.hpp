#ifndef FILE_SYSTEM_PRESENTATION_PROVIDER20_HPP
#define FILE_SYSTEM_PRESENTATION_PROVIDER20_HPP

#include "i_asset_presentation_provider20.hpp"

#include <memory>
#include <map>

namespace wgt
{
namespace AssetBrowser20
{
class FileSystemAssetPresentationProvider : public IAssetPresentationProvider
{
public:
	FileSystemAssetPresentationProvider()
	{
	}
	virtual ~FileSystemAssetPresentationProvider()
	{
	}

	// Pre-generate the binary block data for the thumbnails and status icons so we aren't creating them
	// every time an asset is evaluated for its thumbnail needs.
	void generateData();

	// IAssetPresentationProvider Implementation
	virtual std::shared_ptr<BinaryBlock> getThumbnail(const IAssetObjectItem* asset) override;
	virtual std::shared_ptr<BinaryBlock> getStatusIconData(const IAssetObjectItem* asset) override;
	virtual const char* getTypeIconResourceString(const IAssetObjectItem* asset) const override;

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
	std::shared_ptr<BinaryBlock> getBinaryDataFromFile(const char* filename);

	// Reads the thumbnail file and creates the BinaryBlock data to cache away
	void addThumbnail(int index, const char* filename);

private:
	// Cached thumbnail data for easy retrieval
	std::map<unsigned int, std::shared_ptr<BinaryBlock>> testThumbnails_;
	std::shared_ptr<BinaryBlock> testStatusIcon_;
};
} // end namespace AssetBrowser20

} // end namespace wgt
#endif // FILE_SYSTEM_PRESENTATION_PROVIDER20_HPP
