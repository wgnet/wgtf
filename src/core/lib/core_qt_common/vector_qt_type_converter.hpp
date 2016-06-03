#ifndef VECTOR_QT_TYPE_CONVERTERS_HPP
#define VECTOR_QT_TYPE_CONVERTERS_HPP

#include "i_qt_type_converter.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

namespace wgt
{
class WGVectorQtTypeConverter : public IQtTypeConverter
{
public:
	bool toVariant( const QVariant & qVariant, Variant & o_variant ) const override
	{
		if (qVariant.type() == QVariant::Vector2D)
		{
			auto vectorData = qVariant.value<QVector2D>();
			o_variant = Vector2( 
				vectorData.x(), vectorData.y() );
			return true;
		}

		if (qVariant.type() == QVariant::Vector3D)
		{
			auto vectorData = qVariant.value<QVector3D>();
			o_variant = Vector3( 
				vectorData.x(), vectorData.y(), vectorData.z() );
			return true;
		}

		if (qVariant.type() == QVariant::Vector4D)
		{
			auto vectorData = qVariant.value<QVector4D>();
			o_variant = Vector4( 
				vectorData.x(), vectorData.y(), vectorData.z(), vectorData.w() );
			return true;
		}

		if (qVariant.type() == QVariant::Point)
		{
			auto vectorData = qVariant.value<QPoint>();
			o_variant = Vector2( 
				vectorData.x(), vectorData.y() );
			return true;
		}

		if (qVariant.type() == QVariant::PointF)
		{
			auto vectorData = qVariant.value<QPointF>();
			o_variant = Vector2( 
				vectorData.x(), vectorData.y() );
			return true;
		}

		return false;
	}

	bool toQVariant( const Variant & variant, QVariant & o_qVariant, QObject* parent = nullptr ) const override
	{
		if (variant.typeIs< Vector2 >())
		{
			Vector2 value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QVariant::fromValue( 
				QVector2D( value.x, value.y ) );
			return true;
		}

		if (variant.typeIs< Vector3 >())
		{
			Vector3 value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QVariant::fromValue( 
				QVector3D( value.x, value.y, value.z ) );
			return true;
		}

		if (variant.typeIs< Vector4 >())
		{
			Vector4 value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QVariant::fromValue( 
				QVector4D( value.x, value.y, value.z, value.w ) );
			return true;
		}

		return false;
	}
};
} // end namespace wgt
#endif //VECTOR_QT_TYPE_CONVERTERS_HPP
