#ifndef COLLECTION_QT_TYPE_CONVERTER_HPP
#define COLLECTION_QT_TYPE_CONVERTER_HPP


#include "core_qt_common/i_qt_type_converter.hpp"


namespace wgt
{
/**
 *	Converts between QVariant and Collection.
 */
class CollectionQtTypeConverter : public IQtTypeConverter
{
public:
	bool toVariant(const QVariant & qVariant,
		Variant & o_variant) const override;

	bool toQVariant(const Variant& variant,
		QVariant& o_qVariant, QObject* parent = nullptr) const override;
};
} // end namespace wgt
#endif // COLLECTION_QT_TYPE_CONVERTER_HPP
