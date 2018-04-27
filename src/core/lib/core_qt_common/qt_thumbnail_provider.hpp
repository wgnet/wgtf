#ifndef QT_THUMBNAIL_PROVIDER_HPP
#define QT_THUMBNAIL_PROVIDER_HPP

#include <QQuickImageProvider>
#include <QHash>
#include <memory>

namespace wgt
{
class QtFramework;
class QtThumbnailProvider : public QQuickImageProvider
{
public:
	QtThumbnailProvider(QtFramework& qtFramework);
	QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

	static const char* providerId();
private:
	QtFramework& qtFramework_;
};
} // end namespace wgt
#endif
