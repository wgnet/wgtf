#ifndef MODEL_QT_TYPE_CONVERTER_HPP
#define MODEL_QT_TYPE_CONVERTER_HPP

#include "i_qt_type_converter.hpp"

namespace wgt
{
class IComponentContext;

class ModelQtTypeConverter : public IQtTypeConverter
{
public:
	ModelQtTypeConverter(IComponentContext& context);
	bool toVariant(const QVariant& qVariant, Variant& o_variant) const override;
	bool toQVariant(const Variant& variant, QVariant& o_qVariant, QObject* parent) const override;

private:
	IComponentContext& context_;
};
} // end namespace wgt
#endif
