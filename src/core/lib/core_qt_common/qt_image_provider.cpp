#include "qt_image_provider.hpp"

#include <QIcon>

namespace wgt
{
QtImageProvider::QtImageProvider() : QQuickImageProvider(ImageType::Image)
{
}

QString QtImageProvider::encode(const QColor& color)
{
	QImage image(1, 1, QImage::Format_ARGB32);
	image.fill(color);
	return encode(image);
}

QString QtImageProvider::encode(const QIcon& icon)
{
	auto availableSizes = icon.availableSizes();
	auto pixmap = icon.pixmap(availableSizes[0]);
	return encode(pixmap);
}

QString QtImageProvider::encode(const QPixmap& pixmap)
{
	return encode(pixmap.toImage());
}

QString QtImageProvider::encode(const QImage& image)
{
	auto key = image.cacheKey();
	auto it = imageCache_.find(key);
	if (it == imageCache_.end())
	{
		imageCache_[key] = image;
	}
	return "image://" + QString(providerId()) + "/" + QString(std::to_string(key).c_str());
}

QImage QtImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
	if (size != nullptr)
	{
		*size = requestedSize;
	}

	auto key = id.toLongLong();
	auto it = imageCache_.find(key);
	if (it == imageCache_.end())
	{
		return QImage(requestedSize.width(), requestedSize.height(), QImage::Format_ARGB32);
	}

	return requestedSize.isValid() ? it.value().scaled(requestedSize) : it.value();
}

const char* QtImageProvider::providerId()
{
	return "QtImageProvider";
}
} // end namespace wgt
