#ifndef I_THUMBNAIL_PROVIDER_HPP
#define I_THUMBNAIL_PROVIDER_HPP

#include <memory>

namespace wgt
{
class BinaryBlock;
class IThumbnailProvider
{
public:
	virtual ~IThumbnailProvider()
	{
	}
	virtual bool getThumbnailData(const char* filePath, int* width = nullptr, int* height = nullptr, int* pitch = nullptr, BinaryBlock* imageData = nullptr) const = 0;

};
} // end namespace wgt
#endif
