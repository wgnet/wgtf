#include "image_qt_type_converter.hpp"
#include "core_reflection/object_handle.hpp"
#include "wg_types/binary_block.hpp"

#include <QPixmap>

namespace wgt
{
bool ImageQtTypeConverter::toVariant( const QVariant & qVariant, Variant & o_variant ) const
{
	if (!qVariant.canConvert<QImage>())
	{
		return false;
	}

	auto image = qVariant.value<QImage>();
	std::shared_ptr< BinaryBlock > data( new BinaryBlock( image.bits(), image.byteCount(), false ) );
	o_variant = ObjectHandle( data );
	return true;
}

bool ImageQtTypeConverter::toQVariant( const Variant & variant, QVariant & o_qVariant, QObject* parent ) const
{
	BinaryBlock* data = nullptr;

	if (variant.typeIs<ObjectHandle>())
	{
		ObjectHandle provider;
		if (!variant.tryCast(provider))
		{
			return false;
		}
		data = provider.getBase<BinaryBlock>();
	}
	else if (variant.canCast<std::shared_ptr<BinaryBlock>>())
	{
		data = variant.cast<std::shared_ptr<BinaryBlock>>().get();
	}

	if (data == nullptr)
	{
		return false;
	}

	QImage image;
	image.loadFromData( reinterpret_cast<uchar*>( data->cdata() ), static_cast<uint>( data->length() ) );
	o_qVariant = image;
	return true;
}
} // end namespace wgt
