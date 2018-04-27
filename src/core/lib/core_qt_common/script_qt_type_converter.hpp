#ifndef SCRIPT_QT_TYPE_CONVERTER_HPP
#define SCRIPT_QT_TYPE_CONVERTER_HPP

#include "core_qt_common/i_qt_type_converter.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
class QtScriptingEngineBase;
class IDefinitionManager;

/**
 *	Converts ObjectHandles to QtScriptObjects.
 *	So that properties defined in metadata files (*.mpp) can be accessed
 *	from QML.
 */
class ScriptQtTypeConverter : public IQtTypeConverter, Depends<IDefinitionManager>
{
public:
	ScriptQtTypeConverter(QtScriptingEngineBase& scriptingEngine);

	bool toVariant(const QVariant& qVariant, Variant& o_variant) const override;
	bool toQVariant(const Variant& variant, QVariant& o_qVariant, QObject* parent = 0) const override;
	bool toQVariant(const ObjectHandle& object, QVariant& o_qVariant, QObject* parent = nullptr) const override;

private:
	QtScriptingEngineBase& scriptingEngine_;
};
} // end namespace wgt
#endif // SCRIPT_QT_TYPE_CONVERTER_HPP
