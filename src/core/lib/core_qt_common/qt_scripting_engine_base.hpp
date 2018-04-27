#pragma once

/*
The QtScriptingEngineLite manages QtScriptObjects to allow C++ classes
to be used in QML.
This is used internally by the QtUIFramework to create
context objects and properties.
Details: Search for NGT Reflection System on the Wargaming Confluence
*/

#include "core_reflection/utilities/reflection_utilities.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <QObject>
#include <QColor>
#include <QVariant>

namespace wgt
{
class IClassDefinition;
class IDefinitionManager;
class IUIApplication;
class IQtFramework;
class IQtTypeConverter;
class IComponentContext;
class ICommandManager;
class ICopyPasteManager;
class ObjectHandle;
class QtScriptObject;

class QtScriptingEngineBase : public QObject
{
	Q_OBJECT

public:
	QtScriptingEngineBase();
	virtual ~QtScriptingEngineBase();

	virtual void initialise();

	virtual void finalise();

	virtual QtScriptObject* createScriptObject(const Variant& object, QObject* parent);
	virtual void deregisterScriptObject(QtScriptObject& scriptObject);
	virtual void swapParent(QtScriptObject& scriptObject, QObject* parent);
	Q_INVOKABLE virtual void makeFakeMouseRelease();
protected:
	// TODO: These invokables need to be refactored into different modules to
	// reduce the bloat of this class
	Q_INVOKABLE virtual bool queueCommand(QString command);
	Q_INVOKABLE virtual void beginUndoFrame();
	Q_INVOKABLE virtual void endUndoFrame();
	Q_INVOKABLE virtual void abortUndoFrame();
	Q_INVOKABLE virtual QObject* iterator(const QVariant& collection);
	Q_INVOKABLE virtual QVariant getProperty(const QVariant& object, QString propertyPath);
	Q_INVOKABLE virtual void deleteMacro(QString command);
	Q_INVOKABLE virtual QColor grabScreenColor(int x, int y, QObject* mouseArea);
	Q_INVOKABLE virtual bool isValidColor(QVariant value);
	Q_INVOKABLE virtual bool writeStringToFile(const QString& string, const QString& destPath);
	Q_INVOKABLE virtual QString readStringFromFile(const QString& srcPath);
	// this function is used to resolve breaking binding issue for checkbox and pushbutton, since
	// clicking on checkbox or pushbutton will break the "checked" property binding
	// see: https://bugreports.qt.io/browse/QTBUG-42505 for reference
	Q_INVOKABLE virtual bool setValueHelper(QObject* object, QString property, QVariant value);

	// TODO: remove this when we support dynamically add properties in QML for GenericObject
	Q_INVOKABLE virtual void addPreference(const QString& preferenceId, const QString& propertyName, QVariant value);
	Q_INVOKABLE virtual QVariant getPreferenceValueByName(const QString& preferenceId, const QString& propertyName);

	// this temp function is used by the child controls of a window when they try to close the parent window
	Q_INVOKABLE virtual void closeWindow(const QString& windowId);

	Q_INVOKABLE virtual QString getIconUrlFromImageProvider(const QString& iconKey);

	Q_INVOKABLE virtual void executeAction(const QString& actionId, const QVariant& contextObject);
	Q_INVOKABLE virtual bool canExecuteAction(const QString& actionId, const QVariant& contextObject);
};
} // end namespace wgt
