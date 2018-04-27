#ifndef I_ASSET_PREVIEW_HPP
#define I_ASSET_PREVIEW_HPP

#include "core_reflection/reflected_object.hpp"

namespace wgt
{
class ObjectHandle;

class IAssetPreview
{
	DECLARE_REFLECTED
public:
	virtual ~IAssetPreview()
	{
	}
	virtual const char* assetName() const = 0;
	virtual void setAssetPath(const char* assetPath) = 0;
};
} // end namespace wgt
#endif // I_ASSET_PREVIEW_HPP
