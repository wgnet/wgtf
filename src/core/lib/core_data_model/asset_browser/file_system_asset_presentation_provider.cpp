#include "file_system_asset_presentation_provider.hpp"
#include "core_data_model/asset_browser/i_asset_object_item.hpp"
#include <fstream>

namespace wgt
{
void FileSystemAssetPresentationProvider::generateData(std::string directory)
{
	testThumbnails_.clear();

	const std::string folder(directory + PROJECT_RESOURCE_FOLDER);
	addThumbnail(CACHED_DEFAULT, (folder + "thumbnail_default.png").c_str());
	addThumbnail(CACHED_SCRIPT, (folder + "thumbnail_script.png").c_str());
	addThumbnail(CACHED_XML, (folder + "thumbnail_xml.png").c_str());
	addThumbnail(CACHED_DECAL, (folder + "thumbnail_decal.png").c_str());

	testStatusIcon_ = getBinaryDataFromFile((folder + "status_readonly.png").c_str());
}

ThumbnailData FileSystemAssetPresentationProvider::getBinaryDataFromFile(const char* filename)
{
	if (filename == nullptr)
	{
		return nullptr;
	}

	size_t length = 0;
	std::ifstream input(filename, std::ifstream::binary | std::ios::in);
	if (input.is_open())
	{
		input.seekg(0, std::ios_base::end);
		length = static_cast<size_t>(input.tellg());
		input.seekg(0, std::ios_base::beg);
	}

	if (!length)
	{
		return nullptr;
	}

	char* buffer = new char[length];
	input.read(buffer, length);
	input.close();

	return std::make_shared<BinaryBlock>(buffer, length, false);
}

void FileSystemAssetPresentationProvider::addThumbnail(int index, const char* filename)
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

	testThumbnails_.insert(std::make_pair(index, binaryData));
}

ThumbnailData FileSystemAssetPresentationProvider::getThumbnail(const IAssetObjectItem* asset)
{
	if (asset == nullptr || asset->getAssetName() == nullptr)
	{
		return nullptr;
	}

	// Assumes the use of the BaseAssetObjectItem, whose asset name includes the extension
	const char* extStart = strchr(asset->getAssetName(), '.');
	if (!extStart)
		return nullptr;

	extStart++;

	int type = CACHED_DEFAULT;

	if (strcmp(extStart, "txt") == 0 || strcmp(extStart, "dll") == 0 || strcmp(extStart, "pdb") == 0)
	{
		type = CACHED_SCRIPT;
	}
	else if (strcmp(extStart, "xml") == 0)
	{
		type = CACHED_XML;
	}
	else if (strcmp(extStart, "png") == 0 || strcmp(extStart, "bmp") == 0 || strcmp(extStart, "jpg") == 0)
	{
		type = CACHED_DECAL;
	}

	return testThumbnails_[type];
}

ThumbnailData FileSystemAssetPresentationProvider::getStatusIconData(const IAssetObjectItem* asset)
{
	if (asset != nullptr && asset->isReadOnly())
	{
		return testStatusIcon_;
	}

	return nullptr;
}

const char* FileSystemAssetPresentationProvider::getTypeIconResourceString(const IAssetObjectItem* asset) const
{
	if (asset == nullptr || asset->getAssetName() == nullptr)
	{
		return nullptr;
	}

	// Assumes the use of the BaseAssetObjectItem, whose asset name includes the extension
	const char* extStart = strchr(asset->getAssetName(), '.');
	if (!extStart)
		return nullptr;

	extStart++;

	// Only testing a handful of file types to make sure the functionality works as intended
	if (strcmp(extStart, "txt") == 0 || strcmp(extStart, "dll") == 0 || strcmp(extStart, "pdb") == 0)
	{
		return "icons/script_16x16.png";
	}
	else if (strcmp(extStart, "xml") == 0)
	{
		return "icons/xml_16x16.png";
	}
	else if (strcmp(extStart, "png") == 0 || strcmp(extStart, "bmp") == 0 || strcmp(extStart, "jpg") == 0)
	{
		return "icons/decal_16x16.png";
	}

	// Display default file icon
	return nullptr;
}
} // end namespace wgt
