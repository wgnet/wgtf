#include "qt_image_provider_old.hpp"

// TODO: Remove to platform string header
#if defined( _WIN32 )
#define snprintf sprintf_s
#endif

namespace wgt
{
QtImageProviderOld::QtImageProviderOld()
	: QQuickImageProvider( ImageType::Image )
{
}

QString QtImageProviderOld::encodeImage( const std::shared_ptr< BinaryBlock > & image )
{
	char buffer[ 65535 ];
	snprintf( buffer, sizeof(buffer), "%p", image->data() );
	QString imagePath( buffer );
	auto it = imageCache_.find( imagePath );
	if (it == imageCache_.end())
	{
		QByteArray data;
		data.setRawData( image->cdata(), ( uint )( image->length() ) );
		imageCache_[ imagePath ] = QImage::fromData( data );
	}
	return "image://" + QString( providerId() ) + "/" + imagePath;
}

QImage QtImageProviderOld::requestImage( 
	const QString &id, QSize *size, const QSize& requestedSize )
{
	auto it = imageCache_.find( id );
	if (it != imageCache_.end())
	{
		return it.value();
	}

	return QImage( 
		requestedSize.width(), requestedSize.height(), QImage::Format_ARGB32 );
}

const char * QtImageProviderOld::providerId()
{
	return "QtImageProviderOld";
}
} // end namespace wgt
