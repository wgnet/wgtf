#ifndef QT_IMAGE_PROVIDER_HPP
#define QT_IMAGE_PROVIDER_HPP

#include <QQuickImageProvider>
#include <QHash>

namespace wgt
{
class QtImageProvider : public QQuickImageProvider
{
public:
	QtImageProvider();

	QString encode(const QColor& color);
	QString encode(const QIcon& icon);
	QString encode(const QPixmap& pixmap);
	QString encode(const QImage& image);

	QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

	static const char* providerId();

private:
	// TODO cap the size of this cache
	QHash<qint64, QImage> imageCache_;
};
} // end namespace wgt
#endif
