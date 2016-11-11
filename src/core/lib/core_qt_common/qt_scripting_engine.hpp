#ifndef QT_SCRIPTING_ENGINE_HPP
#define QT_SCRIPTING_ENGINE_HPP

/*
The QtScriptingEngine manages QtScriptObjects to allow C++ classes
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
class WGCopyController;
class ObjectHandle;
class QtScriptObject;

class QtScriptingEngine : public QObject
{
	Q_OBJECT

public:
	QtScriptingEngine();
	virtual ~QtScriptingEngine();

	void initialise(IQtFramework& qtFramework, IComponentContext& contextManager);

	void finalise();

	QtScriptObject* createScriptObject(const Variant& object, QObject* parent);
	void deregisterScriptObject(QtScriptObject& scriptObject);
	void swapParent(QtScriptObject& scriptObject, QObject* parent);
	IDefinitionManager* getDefinitionManager();

protected:
	// TODO: These invokables need to be refactored into different modules to
	// reduce the bloat of this class
	Q_INVOKABLE QObject* createObject(QString definition);
	Q_INVOKABLE bool queueCommand(QString command);
	Q_INVOKABLE void makeFakeMouseRelease();
	Q_INVOKABLE void beginUndoFrame();
	Q_INVOKABLE void endUndoFrame();
	Q_INVOKABLE void abortUndoFrame();
	Q_INVOKABLE QObject* iterator(const QVariant& collection);
	Q_INVOKABLE QVariant getProperty(const QVariant& object, QString propertyPath);
	Q_INVOKABLE void deleteMacro(QString command);
	Q_INVOKABLE void selectControl(wgt::WGCopyController* control, bool append = true);
	Q_INVOKABLE void deselectControl(wgt::WGCopyController* control, bool reset = false);
	Q_INVOKABLE QColor grabScreenColor(int x, int y, QObject* mouseArea);
	// this function is used to resolve breaking binding issue for checkbox and pushbutton, since
	// clicking on checkbox or pushbutton will break the "checked" property binding
	// see: https://bugreports.qt.io/browse/QTBUG-42505 for reference
	Q_INVOKABLE bool setValueHelper(QObject* object, QString property, QVariant value);

	// TODO: remove this when we support dynamically add properties in QML for GenericObject
	Q_INVOKABLE void addPreference(const QString& preferenceId, const QString& propertyName, QVariant value);

	// this temp function is used by the child controls of a window when they try to close the parent window
	Q_INVOKABLE void closeWindow(const QString& windowId);

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // QT_SCRIPTING_ENGINE_HPP
