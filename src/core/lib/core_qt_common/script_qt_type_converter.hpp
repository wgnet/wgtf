#ifndef SCRIPT_QT_TYPE_CONVERTER_HPP
#define SCRIPT_QT_TYPE_CONVERTER_HPP

#include "core_qt_common/i_qt_type_converter.hpp"

namespace wgt
{
class QtScriptingEngine;

/**
 *	Converts ObjectHandles to QtScriptObjects.
 *	So that properties defined in metadata files (*.mpp) can be accessed
 *	from QML.
 */
class ScriptQtTypeConverter : public IQtTypeConverter
{
public:
	ScriptQtTypeConverter(QtScriptingEngine& scriptingEngine);

	bool toVariant(const QVariant& qVariant, Variant& o_variant) const override;
	bool toQVariant(const Variant& variant, QVariant& o_qVariant, QObject* parent = 0) const override;
	bool toQVariant(const ObjectHandle& object, QVariant& o_qVariant, QObject* parent = nullptr) const override;

private:
	QtScriptingEngine& scriptingEngine_;
};
} // end namespace wgt
#endif // SCRIPT_QT_TYPE_CONVERTER_HPP
