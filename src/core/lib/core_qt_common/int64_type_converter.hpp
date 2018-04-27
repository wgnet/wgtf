#pragma once

#include "i_qt_type_converter.hpp"

namespace wgt
{
template <class T>
class Int64TypeConverter : public IQtTypeConverter
{
public:
	bool toVariant(const QVariant& qVariant, Variant& o_variant) const override
	{
		if (qVariant.type() != QVariant::UserType)
		{
			return false;
		}

		if (qVariant.userType() != qMetaTypeId<std::shared_ptr<T>>())
		{
			return false;
		}

		o_variant = qVariant.value<std::shared_ptr<T>>()->value();
		return true;
	}

	bool toQVariant(const Variant& variant, QVariant& o_qVariant, QObject* parent = nullptr) const override
	{
		if (!variant.typeIs<typename T::value_type>())
		{
			return false;
		}
		typename T::value_type value;
		if (variant.tryCast(value))
		{
			o_qVariant = QVariant::fromValue(std::shared_ptr<T>(new T(value)));
			return true;
		}
		return false;
	}
};

} // end namespace wgt
