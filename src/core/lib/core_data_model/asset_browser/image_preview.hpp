#ifndef IMAGE_PREVIEW_H_
#define IMAGE_PREVIEW_H_

#pragma once
#include "i_asset_preview.hpp"
#include <string>
#include "core_common/signal.hpp"

namespace wgt
{
class Variant;
class ImagePreview
	: public IAssetPreview
{
	DECLARE_REFLECTED
public:
	ImagePreview(/*const std::string imagePath*/);
	virtual ~ImagePreview();
	virtual void setAssetPath(const char* assetPath) override;
private:
	const std::string& getImagePath() const;
	virtual const char* assetName() const override;
	std::string imagePath_;
	std::string fileName_;
};
} // end namespace wgt
#endif // IMAGE_PREVIEW_H_
