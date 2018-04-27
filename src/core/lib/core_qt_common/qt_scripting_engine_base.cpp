#include "qt_scripting_engine_base.hpp"

#include "wg_list_iterator.hpp"
#include "core_qt_common/models/qt_item_model.hpp"
#include "collection_qt_type_converter.hpp"
#include "core_logging/logging.hpp"

#include <QVariant>
#include <QQmlEngine>
#include <QMouseEvent>
#include <QApplication>

Q_DECLARE_METATYPE(wgt::ObjectHandle);

namespace wgt
{
QtScriptingEngineBase::QtScriptingEngineBase()
{
}

QtScriptingEngineBase::~QtScriptingEngineBase()
{
}

void QtScriptingEngineBase::initialise()
{
}

void QtScriptingEngineBase::finalise()
{
}

void QtScriptingEngineBase::deregisterScriptObject(QtScriptObject& scriptObject)
{
	NGT_WARNING_MSG("Function is not implemented.");
}

QtScriptObject* QtScriptingEngineBase::createScriptObject(const Variant& object, QObject* parent)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return nullptr;
}

bool QtScriptingEngineBase::queueCommand(QString command)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return false;
}

// NOTE(aidan): Qt doesn't send mouse release events correctly when a drag completes. To workaround,
//			   a fake event is posted when the drag is completed
// TODO(aidan): This bug might be specific to Qt's win32 message handling implementation. Test on Mac, Maya
void QtScriptingEngineBase::makeFakeMouseRelease()
{
	QMouseEvent* releaseEvent =
	new QMouseEvent(QEvent::MouseButtonRelease, QPointF(0, 0), Qt::MouseButton::LeftButton, 0, 0);

	// NOTE(aidan): postEvent takes ownership of the release event
	QApplication::postEvent((QObject*)QApplication::focusWidget(), releaseEvent);
}

void QtScriptingEngineBase::beginUndoFrame()
{
	NGT_WARNING_MSG("Function is not implemented.");
}

void QtScriptingEngineBase::endUndoFrame()
{
	NGT_WARNING_MSG("Function is not implemented.");
}

void QtScriptingEngineBase::abortUndoFrame()
{
	NGT_WARNING_MSG("Function is not implemented.");
}

void QtScriptingEngineBase::deleteMacro(QString command)
{
	NGT_WARNING_MSG("Function is not implemented.");
}

QObject* QtScriptingEngineBase::iterator(const QVariant& collection)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return nullptr;
}

QVariant QtScriptingEngineBase::getProperty(const QVariant& object, QString propertyPath)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return QVariant();
}

bool QtScriptingEngineBase::setValueHelper(QObject* object, QString property, QVariant value)
{
	if (object == nullptr)
	{
		return false;
	}

	return object->setProperty(property.toUtf8(), value);
}

void QtScriptingEngineBase::closeWindow(const QString& windowId)
{
	NGT_WARNING_MSG("Function is not implemented.");
}

void QtScriptingEngineBase::addPreference(const QString& preferenceId, const QString& propertyName, QVariant value)
{
	NGT_WARNING_MSG("Function is not implemented.");
}

QVariant QtScriptingEngineBase::getPreferenceValueByName(const QString& preferenceId, const QString& propertyName)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return QVariant();
}

//------------------------------------------------------------------------------
void QtScriptingEngineBase::swapParent(QtScriptObject& scriptObject, QObject* newParent)
{
	NGT_WARNING_MSG("Function is not implemented.");
}

//------------------------------------------------------------------------------
QColor QtScriptingEngineBase::grabScreenColor(int x, int y, QObject* mouseArea)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return QColor();
}

//------------------------------------------------------------------------------
bool QtScriptingEngineBase::isValidColor(QVariant value)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return false;
}

//------------------------------------------------------------------------------
bool QtScriptingEngineBase::writeStringToFile(const QString& string, const QString& destPath)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return false;
}

//------------------------------------------------------------------------------
QString QtScriptingEngineBase::readStringFromFile(const QString& srcPath)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return QString();
}

//------------------------------------------------------------------------------
QString QtScriptingEngineBase::getIconUrlFromImageProvider(const QString& iconKey)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return "";
}

//------------------------------------------------------------------------------
void QtScriptingEngineBase::executeAction(const QString& actionId, const QVariant& contextObject)
{
	NGT_WARNING_MSG("Function is not implemented.");
}

//------------------------------------------------------------------------------
bool QtScriptingEngineBase::canExecuteAction(const QString& actionId, const QVariant& contextObject)
{
	NGT_WARNING_MSG("Function is not implemented.");
	return false;
}

} // end namespace wgt
