#ifndef TEST_ASSET_PRESENTATION_PROVIDER_HPP
#define TEST_ASSET_PRESENTATION_PROVIDER_HPP

#include "core_data_model/asset_browser/i_asset_presentation_provider.hpp"
#include <map>

namespace wgt
{
/**
 TestAssetPresentationProvider
 This class provides a test platform for the asset presentation provider functionality. It may also serve as an
 example to others as to how it can be used to provide customized presentation information such as thumbnail
 generation, status overlays, and file type icon overrides for their asset browsers.
 */
class TestAssetPresentationProvider : public IAssetPresentationProvider
{
public:
	TestAssetPresentationProvider() {}
	virtual ~TestAssetPresentationProvider() {}

	// Pre-generate the binary block data for the thumbnails and status icons so we aren't creating them 
	// every time an asset is evaluated for its thumbnail needs.
	void generateData();

	// IAssetPresentationProvider Implementation
	virtual ThumbnailData getThumbnail( const IAssetObjectItem * asset ) override;
	virtual ThumbnailData getStatusIconData( const IAssetObjectItem * asset ) override;
	virtual const char* getTypeIconResourceString( const IAssetObjectItem * asset ) const override;

private:
	// Enumeration of thumbnail files we've cached
	enum CachedThumbnails
	{
		CACHED_DEFAULT			= 0,
		CACHED_SCRIPT			= 1,
		CACHED_XML				= 2,
		CACHED_DECAL			= 3,
	};

	// Returns a binary block of data for the designated file
	ThumbnailData getBinaryDataFromFile( const char * filename );

	// Reads the thumbnail file and creates the BinaryBlock data to cache away
	void addThumbnail( int index, const char * filename );

	// Gets the extension value from the asset name
	const char* getExtension( const char* assetName ) const;

private:

	// Cached thumbnail data for easy retrieval
	std::map< unsigned int, ThumbnailData > testThumbnails_;
	ThumbnailData testStatusIcon_;
};
} // end namespace wgt
#endif // TEST_ASSET_PRESENTATION_PROVIDER_HPP
