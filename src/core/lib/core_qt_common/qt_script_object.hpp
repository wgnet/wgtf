#ifndef QT_SCRIPT_OBJECT_HPP
#define QT_SCRIPT_OBJECT_HPP

/*
QtScriptObject is the bridge between an ObjectHandle and a QML type.
When an ObjectHandle is converted into a QtScriptObject, all
exposed properties will be accessible via QML as if it was a
QML type.
In addition, access is provided to meta attributes via
QML callable functions getMetaObject and containsMetaType.
Details: Search for NGT Reflection System on the Wargaming Confluence
*/

#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include <QObject>

namespace wgt
{
class IComponentContext;
class MetaBase;
class Variant;
class IBaseProperty;
class QtScriptingEngine;

struct QtScriptObjectData : public Depends<IDefinitionManager, IReflectionController>
{
	QtScriptObjectData(IComponentContext& context, QtScriptingEngine& engine, const QMetaObject& metaObject,
	                   const ObjectHandle& object)
	    : Depends(context), scriptEngine_(engine), metaObject_(metaObject), object_(object)
	{
	}

	QtScriptingEngine& scriptEngine_;
	const QMetaObject& metaObject_;
	ObjectHandle object_;
};

class QtScriptObject : public QObject
{
public:
	QtScriptObject(std::shared_ptr<QtScriptObjectData>& data, QObject* parent = nullptr) : QObject(parent), data_(data)
	{
	}

	virtual ~QtScriptObject();

	std::shared_ptr<QtScriptObjectData> getData() const
	{
		return data_;
	}

	const ObjectHandle& object() const;
	const QMetaObject* metaObject() const override;
	int qt_metacall(QMetaObject::Call c, int id, void** argv) override;

	void firePropertySignal(const IBasePropertyPtr& property, const Variant& value);
	void fireMethodSignal(const IBasePropertyPtr& method, bool undo = false);

	// This is shadowed on purpose, as QtScriptObjects need to know what their parents are.
	void setParent(QObject* parent);

private:
	QtScriptObject(const QtScriptObject&);

	void callMethod(int id, void** argv);
	MetaHandle getMetaObject(const IClassDefinition* definition, const QString& property) const;
	MetaHandle getMetaObject(const IClassDefinition* definition, const QString& property,
	                         const QString& metaType) const;

	std::shared_ptr<QtScriptObjectData> data_;
};
} // end namespace wgt
#endif // QT_SCRIPT_OBJECT_HPP
