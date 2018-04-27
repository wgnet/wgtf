#ifndef QT_SCRIPTING_ENGINE_HPP
#define QT_SCRIPTING_ENGINE_HPP

/*
The QtScriptingEngine manages QtScriptObjects to allow C++ classes
to be used in QML.
This is used internally by the QtUIFramework to create
context objects and properties.
Details: Search for NGT Reflection System on the Wargaming Confluence
*/

#include "core_qt_common/qt_scripting_engine_base.hpp"

#include "core_dependency_system/i_interface.hpp"
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

class QtScriptingEngine : public QtScriptingEngineBase
{
	Q_OBJECT

public:
	QtScriptingEngine();
	virtual ~QtScriptingEngine();

	void initialise() override;

	void finalise() override;

	QtScriptObject* createScriptObject(const Variant& object, QObject* parent) override;
	void deregisterScriptObject(QtScriptObject& scriptObject) override;
	void swapParent(QtScriptObject& scriptObject, QObject* parent) override;

protected:
	// TODO: These invokables need to be refactored into different modules to
	// reduce the bloat of this class
	Q_INVOKABLE bool queueCommand(QString command) override;
	Q_INVOKABLE void beginUndoFrame() override;
	Q_INVOKABLE void endUndoFrame() override;
	Q_INVOKABLE void abortUndoFrame() override;
	Q_INVOKABLE QObject* iterator(const QVariant& collection) override;
	Q_INVOKABLE QVariant getProperty(const QVariant& object, QString propertyPath) override;
	Q_INVOKABLE void deleteMacro(QString command) override;
	Q_INVOKABLE QColor grabScreenColor(int x, int y, QObject* mouseArea) override;
	Q_INVOKABLE bool isValidColor(QVariant value) override;
	Q_INVOKABLE bool writeStringToFile(const QString& string, const QString& destPath) override;
	Q_INVOKABLE QString readStringFromFile(const QString& srcPath) override;

	// TODO: remove this when we support dynamically add properties in QML for GenericObject
	Q_INVOKABLE void addPreference(const QString& preferenceId, const QString& propertyName, QVariant value) override;
	Q_INVOKABLE QVariant getPreferenceValueByName(const QString& preferenceId, const QString& propertyName) override;

	// this temp function is used by the child controls of a window when they try to close the parent window
	Q_INVOKABLE void closeWindow(const QString& windowId) override;

	Q_INVOKABLE QString getIconUrlFromImageProvider(const QString& iconKey) override;

	Q_INVOKABLE void executeAction(const QString& actionId, const QVariant& contextObject) override;
	Q_INVOKABLE bool canExecuteAction(const QString& actionId, const QVariant& contextObject) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // QT_SCRIPTING_ENGINE_HPP
