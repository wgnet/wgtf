#include "script_qt_type_converter.hpp"

#include "qt_scripting_engine.hpp"
#include "qt_script_object.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
ScriptQtTypeConverter::ScriptQtTypeConverter(QtScriptingEngine& scriptingEngine)
    : IQtTypeConverter(), scriptingEngine_(scriptingEngine)
{
}

bool ScriptQtTypeConverter::toVariant(const QVariant& qVariant, Variant& o_variant) const
{
	if (!qVariant.canConvert<QObject*>())
	{
		return false;
	}

	auto object = qVariant.value<QObject*>();
	auto scriptObject = dynamic_cast<QtScriptObject*>(object);
	if (scriptObject == nullptr)
	{
		return false;
	}

	o_variant = scriptObject->object();
	return true;
}

bool ScriptQtTypeConverter::toQVariant(const Variant& variant, QVariant& o_qVariant, QObject* parent) const
{
	auto typeId = variant.type()->typeId();
	auto defManager = scriptingEngine_.getDefinitionManager();
	assert(defManager != nullptr);
	auto definition = defManager->getDefinition(typeId.getName());
	if (definition != nullptr)
	{
		ObjectHandle obj(variant, definition);
		return this->toQVariant(obj, o_qVariant, parent);
	}
	if (variant.typeIs<ObjectHandle>())
	{
		ObjectHandle provider;
		if (!variant.tryCast(provider))
		{
			return false;
		}
		return this->toQVariant(provider, o_qVariant, parent);
	}

	return false;
}

bool ScriptQtTypeConverter::toQVariant(const ObjectHandle& object, QVariant& o_qVariant, QObject* parent) const
{
	if (!object.isValid())
	{
		o_qVariant = QVariant::Invalid;
		return true;
	}

	auto scriptObject = scriptingEngine_.createScriptObject(object, parent);
	if (scriptObject == nullptr)
	{
		return false;
	}

	o_qVariant = QVariant::fromValue<QtScriptObject*>(scriptObject);
	return true;
}
} // end namespace wgt
