#include "image_preview.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{

ImagePreview::ImagePreview()
{
}

ImagePreview::~ImagePreview()
{
}

void ImagePreview::setAssetPath(const char* assetPath)
{
	imagePath_ = assetPath;
	fileName_ = FilePath::getFileWithExtension(imagePath_);
}

const std::string& ImagePreview::getImagePath() const
{
	return imagePath_;
}

const char* ImagePreview::assetName() const
{
	return fileName_.c_str();
}
} // end namespace wgt
