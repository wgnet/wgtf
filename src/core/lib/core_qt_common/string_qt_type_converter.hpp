#ifndef STRING_QT_TYPE_CONVERTER_HPP
#define STRING_QT_TYPE_CONVERTER_HPP

#include "i_qt_type_converter.hpp"

#include <string>

namespace wgt
{
class StringQtTypeConverter : public IQtTypeConverter
{
public:
	bool toVariant( const QVariant & qVariant, Variant & o_variant ) const override
	{
		QVariant::Type type = qVariant.type();
		if ((type != QVariant::String) && (type != QVariant::Url))
		{
			return false;
		}

		o_variant = qVariant.toString().toUtf8().constData();
		return true;
	}

	bool toQVariant( const Variant & variant, QVariant & o_qVariant, QObject* parent = nullptr ) const override
	{
		if (variant.typeIs< const char * >() ||
			variant.typeIs< std::string >())
		{
			std::string value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QString( value.c_str() );
			return true;
		}

		if (variant.typeIs< const wchar_t * >() ||
			variant.typeIs< std::wstring >())
		{
			std::wstring value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QString::fromWCharArray( value.c_str() );
			return true;
		}

		return false;
	}
};
} // end namespace wgt
#endif
