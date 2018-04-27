#include "qt_thumbnail_provider.hpp"
#include "qt_framework.hpp"

namespace wgt
{
QtThumbnailProvider::QtThumbnailProvider(QtFramework& qtFramework)
	: QQuickImageProvider(ImageType::Image)
	, qtFramework_(qtFramework)
{
}

QImage QtThumbnailProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
	if (size != nullptr)
	{
		*size = requestedSize;
	}
	return qtFramework_.requestThumbnail(id, requestedSize);
}

const char* QtThumbnailProvider::providerId()
{
	return "QtThumbnailProvider";
}
} // end namespace wgt
