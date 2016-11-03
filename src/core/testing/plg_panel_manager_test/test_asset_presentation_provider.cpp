#include "test_asset_presentation_provider.hpp"
#include "core_data_model/asset_browser/i_asset_object_item.hpp"
#include "core_data_model/asset_browser/i_asset_object_item20.hpp"
#include "core_string_utils/file_path.hpp"
#include <fstream>

namespace wgt
{
void TestAssetPresentationProvider::generateData()
{
	testThumbnails_.clear();

	const std::string folder(PROJECT_RESOURCE_FOLDER);
	addThumbnail(CACHED_DEFAULT, (folder + "test_thumbnail_default.png").c_str());
	addThumbnail(CACHED_SCRIPT, (folder + "test_thumbnail_script.png").c_str());
	addThumbnail(CACHED_XML, (folder + "test_thumbnail_xml.png").c_str());
	addThumbnail(CACHED_DECAL, (folder + "test_thumbnail_decal.png").c_str());

	testStatusIcon_ = getBinaryDataFromFile((folder + "test_status_readonly.png").c_str());
}

ThumbnailData TestAssetPresentationProvider::getBinaryDataFromFile(const char* filename)
{
	if (filename == nullptr)
	{
		return nullptr;
	}

	std::streamoff length = 0;
	std::ifstream input( filename, std::ifstream::binary | std::ios::in );
	if (input.is_open())
	{
		input.seekg( 0, std::ios_base::end );
		length = input.tellg();
		input.seekg( 0, std::ios_base::beg );
	}

	if (length < 1)
	{
		return nullptr;
	}
	
	char * buffer = new char[ static_cast< size_t >( length ) ];
	input.read( buffer, length );
	input.close();

	auto bblock = std::make_shared< BinaryBlock >( buffer,
		static_cast< size_t >( length ),
		false /*externallyOwned*/ );

	delete[] buffer;

	return bblock;
}

const char* TestAssetPresentationProvider::getExtension(const char* assetName) const
{
	if (assetName == nullptr)
	{
		return nullptr;
	}

	const char * extension = strchr( assetName, '.' );
	if (extension == nullptr)
	{
		return nullptr;
	}

	if (strlen( extension ) > 1)
	{
		++extension;
	}

	return extension;
}

void TestAssetPresentationProvider::addThumbnail(int index, const char* filename)
{
	if (filename == nullptr)
	{
		return;
	}

	ThumbnailData binaryData = getBinaryDataFromFile( filename );
	if (binaryData == nullptr)
	{
		return;
	}

	testThumbnails_.insert( std::pair< unsigned int, ThumbnailData >( index, binaryData ) );
}

ThumbnailData TestAssetPresentationProvider::getThumbnail(const IAssetObjectItem* asset)
{
	if (asset == nullptr || asset->getAssetName() == nullptr)
	{
		return nullptr;
	}

	// Assumes the use of the BaseAssetObjectItem, whose asset name includes the extension
	const char * extension = getExtension( asset->getAssetName() );
	if (extension == nullptr)
	{
		return nullptr;
	}

	int type = CACHED_DEFAULT;

	if (strcmp( extension, "txt" ) == 0 || strcmp( extension, "dll" ) == 0 || strcmp( extension, "pdb" ) == 0)
	{
		type = CACHED_SCRIPT;
	}
	else if (strcmp( extension, "xml" ) == 0)
	{
		type = CACHED_XML;
	}
	else if (strcmp( extension, "png" ) == 0 || strcmp( extension, "bmp" ) == 0 || strcmp( extension, "jpg" ) == 0)
	{
		type = CACHED_DECAL;
	}

	return testThumbnails_[ type ];
}

ThumbnailData TestAssetPresentationProvider::getStatusIconData(const IAssetObjectItem* asset)
{
	if (asset != nullptr && asset->isReadOnly())
	{
		return testStatusIcon_;
	}

	return nullptr;
}

const char* TestAssetPresentationProvider::getTypeIconResourceString(const IAssetObjectItem* asset) const
{
	if (asset == nullptr || asset->getAssetName() == nullptr)
	{
		return nullptr;
	}

	// Assumes the use of the BaseAssetObjectItem, whose asset name includes the extension
	const char * extension = getExtension( asset->getAssetName() );
	if (extension == nullptr)
	{
		return nullptr;
	}

	// Only testing a handful of file types to make sure the functionality works as intended
	if (strcmp( extension, "txt" ) == 0 || strcmp( extension, "dll" ) == 0 || strcmp( extension, "pdb" ) == 0)
	{
		return "icons/script_16x16.png";
	}
	else if (strcmp( extension, "xml" ) == 0)
	{
		return "icons/xml_16x16.png";
	}
	else if (strcmp( extension, "png" ) == 0 || strcmp( extension, "bmp" ) == 0 || strcmp( extension, "jpg" ) == 0)
	{
		return "icons/decal_16x16.png";
	}

	// Display default file icon
	return nullptr;
}
//////////////////////////////////////////////////////////////////////////
void TestAssetPresentationProvider20::generateData()
{
	testThumbnails_.clear();

	addThumbnail(CACHED_DEFAULT, "plugins/test_resources/test_thumbnail_default.png");
	addThumbnail(CACHED_SCRIPT, "plugins/test_resources/test_thumbnail_script.png");
	addThumbnail(CACHED_XML, "plugins/test_resources/test_thumbnail_xml.png");
	addThumbnail(CACHED_DECAL, "plugins/test_resources/test_thumbnail_decal.png");

	testStatusIcon_ = getBinaryDataFromFile("plugins/test_resources/test_status_readonly.png");
}

ThumbnailData TestAssetPresentationProvider20::getBinaryDataFromFile(const char* filename)
{
	if (filename == nullptr)
	{
		return nullptr;
	}

	std::streamoff length = 0;
	std::ifstream input(filename, std::ifstream::binary | std::ios::in);
	if (input.is_open())
	{
		input.seekg(0, std::ios_base::end);
		length = input.tellg();
		input.seekg(0, std::ios_base::beg);
	}

	if (length < 1)
	{
		return nullptr;
	}

	char* buffer = new char[static_cast<size_t>(length)];
	input.read(buffer, length);
	input.close();

	auto bblock = std::make_shared<BinaryBlock>(buffer,
	                                            static_cast<size_t>(length),
	                                            false /*externallyOwned*/);

	delete[] buffer;

	return bblock;
}

const char* TestAssetPresentationProvider20::getExtension(const char* assetName) const
{
	if (assetName == nullptr)
	{
		return nullptr;
	}

	const char* extension = strchr(assetName, '.');
	if (extension == nullptr)
	{
		return nullptr;
	}

	if (strlen(extension) > 1)
	{
		++extension;
	}

	return extension;
}

void TestAssetPresentationProvider20::addThumbnail(int index, const char* filename)
{
	if (filename == nullptr)
	{
		return;
	}

	ThumbnailData binaryData = getBinaryDataFromFile(filename);
	if (binaryData == nullptr)
	{
		return;
	}

	testThumbnails_.insert(std::pair<unsigned int, ThumbnailData>(index, binaryData));
}

ThumbnailData TestAssetPresentationProvider20::getThumbnail(const AssetBrowser20::IAssetObjectItem* asset)
{
	if (asset == nullptr || asset->getAssetName() == nullptr)
	{
		return nullptr;
	}

	// Assumes the use of the BaseAssetObjectItem, whose asset name includes the extension
	const char* extension = getExtension(asset->getAssetName());
	if (extension == nullptr)
	{
		return nullptr;
	}

	int type = CACHED_DEFAULT;

	if (strcmp(extension, "txt") == 0 || strcmp(extension, "dll") == 0 || strcmp(extension, "pdb") == 0)
	{
		type = CACHED_SCRIPT;
	}
	else if (strcmp(extension, "xml") == 0)
	{
		type = CACHED_XML;
	}
	else if (strcmp(extension, "png") == 0 || strcmp(extension, "bmp") == 0 || strcmp(extension, "jpg") == 0)
	{
		type = CACHED_DECAL;
	}

	return testThumbnails_[type];
}

ThumbnailData TestAssetPresentationProvider20::getStatusIconData(const AssetBrowser20::IAssetObjectItem* asset)
{
	if (asset != nullptr && asset->isReadOnly())
	{
		return testStatusIcon_;
	}

	return nullptr;
}

const char* TestAssetPresentationProvider20::getTypeIconResourceString(const AssetBrowser20::IAssetObjectItem* asset) const
{
	if (asset == nullptr || asset->getAssetName() == nullptr)
	{
		return nullptr;
	}

	// Assumes the use of the BaseAssetObjectItem, whose asset name includes the extension
	const char* extension = getExtension(asset->getAssetName());
	if (extension == nullptr)
	{
		return nullptr;
	}

	// Only testing a handful of file types to make sure the functionality works as intended
	if (strcmp(extension, "txt") == 0 || strcmp(extension, "dll") == 0 || strcmp(extension, "pdb") == 0)
	{
		return "icons/script_16x16.png";
	}
	else if (strcmp(extension, "xml") == 0)
	{
		return "icons/xml_16x16.png";
	}
	else if (strcmp(extension, "png") == 0 || strcmp(extension, "bmp") == 0 || strcmp(extension, "jpg") == 0)
	{
		return "icons/decal_16x16.png";
	}

	// Display default file icon
	return nullptr;
}
} // end namespace wgt
