#ifndef STRING_QT_TYPE_CONVERTER_HPP
#define STRING_QT_TYPE_CONVERTER_HPP

#include "i_qt_type_converter.hpp"

#include <string>

namespace wgt
{
class RefObjectIdQtTypeConverter : public IQtTypeConverter
{
public:
	bool toVariant(const QVariant& qVariant, Variant& o_variant) const override
	{
		int typeId = qVariant.type();
		if (typeId == QVariant::UserType)
		{
			typeId = qVariant.userType();
		}

		if (typeId != qMetaTypeId<RefObjectId>())
		{
			return false;
		}

		o_variant = qVariant.value<RefObjectId>();
		return true;
	}

	bool toQVariant(const Variant& variant, QVariant& o_qVariant, QObject* parent = nullptr) const override
	{
		if (variant.typeIs<RefObjectId>() == false)
		{
			return false;
		}
		RefObjectId tmp;
		if (variant.tryCast(tmp))
		{
			// TODO: build the real data instead of converting to a string
			o_qVariant = QString(tmp.toString().c_str());
		}
		return true;
	}
};
} // end namespace wgt
#endif
