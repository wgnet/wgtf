#include "image_extension.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_image_provider.hpp"

#include <QIcon>
#include <Qimage>

namespace wgt
{
ITEMROLE( image )

ImageExtension::ImageExtension()
{
	roles_.push_back( ItemRole::imageName );
}

ImageExtension::~ImageExtension()
{
}

QVariant ImageExtension::data( const QModelIndex &index, ItemRole::Id roleId ) const
{
	if (roleId != ItemRole::imageId)
	{
		return QVariant(QVariant::Invalid);
	}

	auto qtFramework = Context::queryInterface<IQtFramework>();
	if (qtFramework == nullptr)
	{
		return QVariant(QVariant::Invalid);
	}

	const auto id = QtImageProvider::providerId();
	auto imageProvider = dynamic_cast<QtImageProvider*>(qtFramework->qmlEngine()->imageProvider(id));
	if (imageProvider == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	auto decoration = extensionData_->data(index, ItemRole::decorationId);
	if (decoration.isValid())
	{
		if (decoration.canConvert<QColor>())
		{
			auto color = decoration.value<QColor>();
			return imageProvider->encode(color);
		}

		if (decoration.canConvert<QIcon>())
		{
			auto icon = decoration.value<QIcon>();
			return imageProvider->encode(icon);
		}

		if (decoration.canConvert<QPixmap>())
		{
			auto pixmap = decoration.value<QPixmap>();
			return imageProvider->encode(pixmap);
		}

		if (decoration.canConvert<QImage>())
		{
			auto image = decoration.value<QImage>();
			return imageProvider->encode(image);
		}
	}

	auto thumbnail = extensionData_->data(index, ThumbnailRole::roleId_);
	if (thumbnail.isValid())
	{
		auto image = thumbnail.value<QImage>();
		return imageProvider->encode(image);
	}

	return QVariant( QVariant::Invalid );
}
} // end namespace wgt
