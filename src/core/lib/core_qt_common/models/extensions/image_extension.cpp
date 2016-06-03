#include "image_extension.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_image_provider.hpp"

#include <QIcon>

namespace wgt
{
ITEMROLE( image )

ImageExtension::ImageExtension()
{
	qtFramework_ = Context::queryInterface< IQtFramework >();
}

ImageExtension::~ImageExtension()
{
}

QHash< int, QByteArray > ImageExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	registerRole( ItemRole::imageName, roleNames );
	return roleNames;
}

QVariant ImageExtension::data( const QModelIndex &index, int role ) const
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	if (roleId != ItemRole::imageId )
	{
		return QVariant( QVariant::Invalid );
	}

	auto qtImageProvider = dynamic_cast< QtImageProvider * >(
		qtFramework_->qmlEngine()->imageProvider( QtImageProvider::providerId() ) );
	if (qtImageProvider == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}
		
	auto decoration = index.model()->data( index, Qt::DecorationRole );
	if (!decoration.isValid())
	{
		return QVariant( QVariant::Invalid );
	}

	if (decoration.canConvert<QColor>())
	{
		auto color = decoration.value<QColor>();
		return qtImageProvider->encode( color );
	}
	
	if (decoration.canConvert<QIcon>())
	{
		auto icon = decoration.value<QIcon>();
		return qtImageProvider->encode( icon );
	}

	if (decoration.canConvert<QPixmap>())
	{
		auto pixmap = decoration.value<QPixmap>();
		return qtImageProvider->encode( pixmap );
	}

	if (decoration.canConvert<QImage>())
	{
		auto image = decoration.value<QImage>();
		return qtImageProvider->encode( image );
	}

	return QVariant( QVariant::Invalid );
}

bool ImageExtension::setData( const QModelIndex &index, const QVariant &value, int role )
{
	return false;
}
} // end namespace wgt
