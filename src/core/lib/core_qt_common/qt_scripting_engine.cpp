#include "qt_scripting_engine.hpp"

#include "qobject_qt_type_converter.hpp"
#include "qt_script_object.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "script_qt_type_converter.hpp"
#include "wg_list_iterator.hpp"
#include "core_qt_common/models/qt_item_model.hpp"
#include "collection_qt_type_converter.hpp"
#include "core_qt_common/image_qt_type_converter.hpp"
#include "core_qt_common/model_qt_type_converter.hpp"
#include "core_qt_common/refobjectid_qt_type_converter.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_copy_paste/i_copy_paste_manager.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/interfaces/i_action_manager.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include "core_serialization/i_file_system.hpp"

#include "core_common/assert.hpp"
#include "core_logging/logging.hpp"

#include <private/qmetaobjectbuilder_p.h>
#include <QVariant>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPointer>
#include <QMouseEvent>
#include <QApplication>

#include <QImage>
#include <QPixmap>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQuickRenderControl>
#include <QDesktopWidget>
#include <QScreen>
#include <QWindow>

Q_DECLARE_METATYPE(wgt::ObjectHandle);

namespace wgt
{
struct QtScriptingEngine::Implementation : Depends<IDefinitionManager, ICommandManager, ICopyPasteManager,
                                                   IUIApplication, IUIFramework, IQtFramework, IQtHelpers, 
                                                   IObjectManager, IFileSystem, IActionManager>

{
	typedef std::unordered_map<QObject*, QPointer<QtScriptObject>> ParentMap;
	typedef std::unordered_map<uint64_t, std::pair<std::weak_ptr<QtScriptObjectData>, ParentMap>> ScriptObjectCollection;

	Implementation(QtScriptingEngine& self) : self_(self)
	{
		propListener_ = std::make_shared<PropertyListener>(scriptObjects_);

		// TODO: All but the scriptTypeConverter need to be moved to the qt app plugin.
		qtTypeConverters_.emplace_back(new GenericQtTypeConverter<ObjectHandle>());
		qtTypeConverters_.emplace_back(new ImageQtTypeConverter());
		qtTypeConverters_.emplace_back(new ModelQtTypeConverter);
		qtTypeConverters_.emplace_back(new QObjectQtTypeConverter());
		qtTypeConverters_.emplace_back(new CollectionQtTypeConverter);
		qtTypeConverters_.emplace_back(new ScriptQtTypeConverter(self_));
		qtTypeConverters_.emplace_back(new RefObjectIdQtTypeConverter());
		QMetaType::registerComparators<ObjectHandle>();
		QMetaType::registerComparators<RefObjectId>();
	}

	~Implementation()
	{
		propListener_ = nullptr;
		TF_ASSERT(scriptObjects_.empty());
		std::lock_guard<std::mutex> guard(metaObjectsMutex_);

		for (auto& metaObjectPair : metaObjects_)
		{
			free(metaObjectPair.second);
		}

		metaObjects_.clear();
	}

	void initialise();

	QtScriptObject* createScriptObject(const Variant& object, QObject* parent);
    QtScriptObject* createScriptObject(QObject* parent, ObjectHandle handle, uint64_t hash, ParentMap* map = nullptr);
	QMetaObject* getMetaObject(const IClassDefinition& classDefinition);

	QtScriptingEngine& self_;
	std::mutex metaObjectsMutex_;
	std::unordered_map<std::string, QMetaObject*> metaObjects_;
	std::vector<std::unique_ptr<IQtTypeConverter>> qtTypeConverters_;

	ScriptObjectCollection scriptObjects_;

	struct PropertyListener : public PropertyAccessorListener, Depends<IDefinitionManager>
	{
		PropertyListener(const ScriptObjectCollection& scriptObjects) : scriptObjects_(scriptObjects)
		{
		}

		void postSetValue(const PropertyAccessor& accessor, const Variant& value) override;
		void postInvoke(const PropertyAccessor& accessor, Variant result, bool undo) override;

		const ScriptObjectCollection& scriptObjects_;
	};

	std::shared_ptr<PropertyAccessorListener> propListener_;
};

void QtScriptingEngine::Implementation::initialise()
{
	auto qtFramework = get<IQtFramework>();
	for (auto& qtTypeConverter : qtTypeConverters_)
	{
		qtFramework->registerTypeConverter(*qtTypeConverter);
	}

	auto defManager = get<IDefinitionManager>();
	defManager->registerPropertyAccessorListener(propListener_);
}

void QtScriptingEngine::Implementation::PropertyListener::postSetValue(const PropertyAccessor& accessor,
                                                                       const Variant& value)
{
	const ObjectHandle& object = accessor.getObject();
	auto itr = scriptObjects_.find(reflectedHash(object, *get<IDefinitionManager>()));
	if (itr == scriptObjects_.end())
	{
		return;
	}

	// Copy collection to accommodate re-entry
	auto scriptObjects = itr->second;
	for (auto& scriptObject : scriptObjects.second)
	{
		TF_ASSERT(!scriptObject.second.isNull());
		if (!scriptObject.second.isNull())
		{
			scriptObject.second->firePropertySignal(accessor.getProperty(), value);
		}
	}
}

void QtScriptingEngine::Implementation::PropertyListener::postInvoke(const PropertyAccessor& accessor, Variant result,
                                                                     bool undo)
{
    const ObjectHandle& object = accessor.getObject();
    auto itr = scriptObjects_.find(reflectedHash(object, *get<IDefinitionManager>()));
    if (itr == scriptObjects_.end())
    {
        return;
    }

	auto& scriptObjects = itr->second;
	for (auto& scriptObject : scriptObjects.second)
	{
		TF_ASSERT(!scriptObject.second.isNull());
		if (!scriptObject.second.isNull())
		{
			scriptObject.second->fireMethodSignal(accessor.getProperty(), undo);
		}
	}
}

QtScriptObject* QtScriptingEngine::Implementation::createScriptObject(QObject* parent, ObjectHandle handle, uint64_t hash, ParentMap* map)
{
    auto classDefinition = get<IDefinitionManager>()->getDefinition(handle);
    if (classDefinition == nullptr)
    {
        return nullptr;
    }

    auto metaObject = getMetaObject(*classDefinition);
    if (metaObject == nullptr)
    {
        return nullptr;
    }

    auto data = std::make_shared<QtScriptObjectData>(self_, metaObject, classDefinition, handle, hash);
    QtScriptObject* scriptObject = new QtScriptObject(data, parent);

    if (map == nullptr)
    {
        ParentMap parentMap;
        parentMap.insert(std::make_pair(parent, scriptObject));
        scriptObjects_.insert(std::make_pair(data->hash_, std::make_pair(data, parentMap)));
    }
    else
    {
        map->insert(std::make_pair(parent, scriptObject));
    }

    auto definitionModifier = classDefinition->getDetails().getDefinitionModifier();
    if (definitionModifier != nullptr)
    {
        // Cannot capture shared_ptr in the lambda, as this will cause a
        // circular reference
        // TODO: We need to further look into this we shouldn't need a unique
        // lambda per instance.
        auto pData = data.get();
        auto postChanged = [&, pData, classDefinition](const char* name) {
            auto definitionName = classDefinition->getName();
            QMetaObject* oldMetaObject = nullptr;
            {
                std::lock_guard<std::mutex> guard(metaObjectsMutex_);
                auto findIt = metaObjects_.find(definitionName);
                if (findIt == metaObjects_.end())
                {
                    return;
                }
                oldMetaObject = findIt->second;
                metaObjects_.erase(findIt);
            }
            pData->metaObject_ = getMetaObject(*classDefinition);
            if (oldMetaObject != nullptr)
            {
                free(oldMetaObject);
            }
        };
        data->connectPostPropertyAdded_ = definitionModifier->postPropertyAdded.connect(postChanged);
        data->connectPostPropertyRemoved_ = definitionModifier->postPropertyRemoved.connect(postChanged);
    }
    return scriptObject;
}

QtScriptObject* QtScriptingEngine::Implementation::createScriptObject(const Variant& variant, QObject* parent)
{
    ObjectHandle handle;
    if (!variant.tryCast(handle))
    {
        auto name = variant.type()->typeId().getName();
        if (auto definition = get<IDefinitionManager>()->getDefinition(name))
        {
            NGT_ERROR_MSG("%s should be passed as ObjectHandle rather than pointer", name);
            TF_ASSERT(false && "Object should be passed as ObjectHandle rather than pointer");
            return nullptr;
        }
    }

    handle = reflectedRoot(handle, *get<IDefinitionManager>());
	if (!handle.isValid())
	{
		return nullptr;
	}

    auto hash = reflectedHash(handle, *get<IDefinitionManager>());

	auto parentItr = scriptObjects_.find(hash);
	if (parentItr == scriptObjects_.end())
	{
        return createScriptObject(parent, handle, hash);
	}
    
	auto& parentMap = parentItr->second.second;
	auto findIt = parentMap.find(parent);
	if (findIt == parentMap.end())
	{
		auto dataPtr = parentItr->second.first.lock();
		TF_ASSERT(dataPtr != nullptr);

        if (dataPtr->rootObject_ == nullptr)
        {
            return createScriptObject(parent, handle, hash, &parentMap);
        }
        else
        {
            auto scriptObject = new QtScriptObject(dataPtr, parent);
            parentMap.insert(std::make_pair(parent, scriptObject));
            return scriptObject;
        }
	}
	return findIt->second;
}

QMetaObject* QtScriptingEngine::Implementation::getMetaObject(const IClassDefinition& classDefinition)
{
	auto definition = classDefinition.getName();

	{
		std::lock_guard<std::mutex> guard(metaObjectsMutex_);
		auto metaObjectIt = metaObjects_.find(definition);
		if (metaObjectIt != metaObjects_.end())
		{
			return metaObjectIt->second;
		}
	}

	QMetaObjectBuilder builder;
	builder.setClassName(definition);
	builder.setSuperClass(&QObject::staticMetaObject);

	auto thisProperty = builder.addProperty("self", "QObject*");
	thisProperty.setWritable(false);
	thisProperty.setConstant(true);

	// Add all the properties from the ClassDefinition to the builder
	auto properties = classDefinition.allProperties();
	auto it = properties.begin();

	for (; it != properties.end(); ++it)
	{
		if (it->isMethod())
		{
			continue;
		}

		auto property = builder.addProperty(it->getName(), "QVariant");
		property.setWritable(!it->readOnly(ObjectHandle()));

		auto notifySignal = std::string(it->getName()) + "Changed(QVariant)";
		property.setNotifySignal(builder.addSignal(notifySignal.c_str()));
	}

	std::vector<std::pair<std::string, std::string>> methodSignatures;
	std::string methodSignature;

	// TODO: Move these three to actual methods on the scripting engine.
	methodSignatures.emplace_back("getMetaObject(QString)", "QVariant");
	methodSignatures.emplace_back("getMetaObject(QString,QString)", "QVariant");
	methodSignatures.emplace_back("containsMetaType(QString,QString)", "QVariant");

	for (it = properties.begin(); it != properties.end(); ++it)
	{
		if (!it->isMethod())
		{
			continue;
		}

		methodSignature = it->getName();
		methodSignature += "(";

		for (size_t i = 0; i < it->parameterCount(); ++i)
		{
			methodSignature += "QVariant";

			if (i < it->parameterCount() - 1)
			{
				methodSignature += ",";
			}
		}

		methodSignature += ")";

		// TODO - determine if the function does not have a return type.
		// currently 'invoke' will always return a Variant regardless
		methodSignatures.emplace_back(std::move(methodSignature), "QVariant");
	}

	// skip index 0 as it has the same name as the one at index 1.
	for (size_t i = 1; i < methodSignatures.size(); ++i)
	{
		methodSignature =
		methodSignatures[i].first.substr(0, methodSignatures[i].first.find('(')) + "Invoked(QVariant)";
		QMetaMethodBuilder method = builder.addSignal(methodSignature.c_str());
		QList<QByteArray> parameterNames;
		parameterNames.append("undo");
		method.setParameterNames(parameterNames);
	}

	for (size_t i = 0; i < methodSignatures.size(); ++i)
	{
		builder.addMethod(methodSignatures[i].first.c_str(), methodSignatures[i].second.c_str());
	}

	auto metaObject = builder.toMetaObject();
	if (metaObject == nullptr)
	{
		return nullptr;
	}

	{
		std::lock_guard<std::mutex> guard(metaObjectsMutex_);
		auto inserted = metaObjects_.insert(std::pair<std::string, QMetaObject*>(definition, metaObject));
		if (!inserted.second)
		{
			free(metaObject);
		}
		return inserted.first->second;
	}
}

QtScriptingEngine::QtScriptingEngine() : impl_(new Implementation(*this))
{
}

QtScriptingEngine::~QtScriptingEngine()
{
	impl_ = nullptr;
}

void QtScriptingEngine::initialise()
{
	impl_->initialise();
}

void QtScriptingEngine::finalise()
{
	// normally, this assert should never fire
	// if it does, we need to figure out why
	// QScriptObject not get destroyed correctly.
	TF_ASSERT(impl_->scriptObjects_.empty());
	impl_->scriptObjects_.clear();
}

void QtScriptingEngine::deregisterScriptObject(QtScriptObject& scriptObject)
{
	auto findIt = impl_->scriptObjects_.find(scriptObject.getData()->hash_);
	TF_ASSERT(findIt != impl_->scriptObjects_.end());
	auto& parentMap = findIt->second.second;
	auto parent = scriptObject.parent();
	auto parentIt = parentMap.find(parent);
	TF_ASSERT(parentIt != parentMap.end());
	parentMap.erase(parentIt);
	if (parentMap.empty())
	{
		impl_->scriptObjects_.erase(findIt->first);
	}
}

QtScriptObject* QtScriptingEngine::createScriptObject(const Variant& object, QObject* parent)
{
	return impl_->createScriptObject(object, parent);
}

bool QtScriptingEngine::queueCommand(QString command)
{
	std::string commandId = command.toUtf8().constData();
	Command* cmd = impl_->get<ICommandManager>()->findCommand(commandId.c_str());
	if (cmd == nullptr)
	{
		qWarning("Could not find Command: %s \n", commandId.c_str());
		return false;
	}
	impl_->get<ICommandManager>()->queueCommand(commandId.c_str());
	return true;
}

void QtScriptingEngine::beginUndoFrame()
{
	impl_->get<ICommandManager>()->beginBatchCommand();
}

void QtScriptingEngine::endUndoFrame()
{
	impl_->get<ICommandManager>()->endBatchCommand();
}

void QtScriptingEngine::abortUndoFrame()
{
	impl_->get<ICommandManager>()->abortBatchCommand();
}

void QtScriptingEngine::deleteMacro(QString command)
{
	std::string commandId = command.toUtf8().constData();
	Command* cmd = impl_->get<ICommandManager>()->findCommand(commandId.c_str());
	if (cmd == nullptr)
	{
		qWarning("Delete macro failed: Could not find Macro: %s \n", commandId.c_str());
		return;
	}
	impl_->get<ICommandManager>()->deleteMacroByName(commandId.c_str());
}

QObject* QtScriptingEngine::iterator(const QVariant& collection)
{
	int typeId = collection.type();
	if (typeId == QVariant::UserType)
	{
		typeId = collection.userType();
	}

	AbstractListModel* listModel = nullptr;

	if (typeId == qMetaTypeId<Variant>())
	{
		auto variant = collection.value<Variant>();
		if (!variant.typeIs<AbstractListModel>())
		{
			return nullptr;
		}

		listModel = const_cast<AbstractListModel*>(variant.cast<const AbstractListModel*>());
	}
	else if (typeId == qMetaTypeId<ObjectHandle>())
	{
		auto handle = collection.value<ObjectHandle>();
		if (!handle.isValid())
		{
			return nullptr;
		}

		listModel = handle.getBase<AbstractListModel>();
	}
	else if (auto model = QtItemModel<QtListModel>::fromQVariant(collection))
	{
		listModel = &model->source();
	}

	if (listModel == nullptr)
	{
		return nullptr;
	}

	// QML will take ownership of this object
	return new WGListIterator(*listModel);
}

QVariant QtScriptingEngine::getProperty(const QVariant& object, QString propertyPath)
{
	ObjectHandle handle;
	auto qtHelpers = impl_->get<IQtHelpers>();
	if (qtHelpers->toVariant(object).tryCast(handle))
	{
		auto definition = impl_->get<IDefinitionManager>()->getDefinition(handle);
		if (definition != nullptr)
		{
			auto accessor = definition->bindProperty(propertyPath.toLocal8Bit().data(), handle);
			if (accessor.isValid())
			{
				return qtHelpers->toQVariant(accessor.getValue(), nullptr);
			}
		}
	}
	return QVariant();
}

void QtScriptingEngine::closeWindow(const QString& windowId)
{
	std::string id = windowId.toUtf8().constData();
	auto windows = impl_->get<IUIApplication>()->windows();
	auto findIt = windows.find(id);
	if (findIt == windows.end())
	{
		qWarning("Failed to close window: Could not find window: %s \n", id.c_str());
		return;
	}
	findIt->second->close();
}

void QtScriptingEngine::addPreference(const QString& preferenceId, const QString& propertyName, QVariant value)
{
	std::string id = preferenceId.toUtf8().constData();
	std::string name = propertyName.toUtf8().constData();
	std::string data = value.toString().toUtf8().constData();
	auto preference = impl_->get<IUIFramework>()->getPreferences()->getPreference(id.c_str());
	preference->set(name.c_str(), data, false);
}

QVariant QtScriptingEngine::getPreferenceValueByName(const QString& preferenceId, const QString& propertyName)
{
	std::string id = preferenceId.toUtf8().constData();
	std::string name = propertyName.toUtf8().constData();
	auto preference = impl_->get<IUIFramework>()->getPreferences()->getPreference(id.c_str());
	auto accessor = preference->findProperty(name.c_str());
	if (!accessor.isValid())
	{
		return QVariant();
	}
	auto qtHelpers = impl_->get<IQtHelpers>();
	TF_ASSERT(qtHelpers != nullptr);
	return qtHelpers->toQVariant(accessor.getValue(), nullptr);
}

//------------------------------------------------------------------------------
void QtScriptingEngine::swapParent(QtScriptObject& scriptObject, QObject* newParent)
{
	auto findIt = impl_->scriptObjects_.find(scriptObject.getData()->hash_);
	TF_ASSERT(findIt != impl_->scriptObjects_.end());
	auto& parentMap = findIt->second.second;
	auto currentParent = scriptObject.parent();
	auto parentIt = parentMap.find(currentParent);
	TF_ASSERT(parentIt != parentMap.end());
	parentMap.erase(parentIt);
	parentMap.insert(std::make_pair(newParent, &scriptObject));
}

//------------------------------------------------------------------------------
QColor QtScriptingEngine::grabScreenColor(int x, int y, QObject* mouseArea)
{
	QColor result;

	QQuickItem* item = dynamic_cast<QQuickItem*>(mouseArea);
	if (item)
	{
		QPoint widgetOffset;
		QQuickWindow* window = item->window();
		QWindow* renderWindow = QQuickRenderControl::renderWindowFor(window, &widgetOffset);
		QPointF pos = (renderWindow ? renderWindow : window)->mapToGlobal(widgetOffset);

		QTransform transform = QTransform::fromTranslate(pos.x(), pos.y());
		QPointF globalPoint = transform.map(item->mapToScene(QPointF((float)x, (float)y)));

		const QDesktopWidget* desktop = QApplication::desktop();
		const QPixmap pixmap = QGuiApplication::screens()
		                       .at(desktop->screenNumber())
		                       ->grabWindow(desktop->winId(), globalPoint.x(), globalPoint.y(), 1, 1);

		QImage i = pixmap.toImage();
		result = i.pixel(0, 0);
	}

	return result;
}

//------------------------------------------------------------------------------
bool QtScriptingEngine::isValidColor(QVariant value)
{
	if (value.type() == QVariant::Vector3D)
	{
		return true;
	}
	if (value.type() == QVariant::Vector4D)
	{
		return true;
	}
	if(value.type() == QVariant::String)
	{
		return QColor::isValidColor(value.toString());
	}
	return false;
}

//------------------------------------------------------------------------------
bool QtScriptingEngine::writeStringToFile(const QString& string, const QString& destPath)
{
	bool result = false;
	if (IFileSystem *fileSys = impl_->get<IFileSystem>()) 
	{
		const char *data = string.toUtf8().data();
		int dataLen      = string.toUtf8().count();
		result = fileSys->writeFile(destPath.toUtf8().data(), data, dataLen, 
			                        std::ios::trunc | std::ios::out);
	}

	return result;
}

//------------------------------------------------------------------------------
QString QtScriptingEngine::readStringFromFile(const QString& srcPath)
{
	if (IFileSystem *fileSys = impl_->get<IFileSystem>()) 
	{
		const char *pathCStr              = srcPath.toUtf8().data();
		IFileSystem::IStreamPtr streamPtr = fileSys->readFile(pathCStr, std::ios::in);
		std::streamoff size               = streamPtr->seek(0, std::ios::end);
		if (size <= 0) return QString();

		streamPtr->seek(0, std::ios::beg);
		QByteArray outputBuf = QByteArray(size, 0);
		streamPtr->read(outputBuf.data(), size);

		return QString(outputBuf);
	}

	return QString();
}

//------------------------------------------------------------------------------
QString QtScriptingEngine::getIconUrlFromImageProvider(const QString& iconKey)
{
	auto uiFramework = impl_->get<IUIFramework>();
	TF_ASSERT(uiFramework != nullptr);
	return uiFramework->getIconUrlFromImageProvider(iconKey.toUtf8().constData());
}

//------------------------------------------------------------------------------
void QtScriptingEngine::executeAction(const QString& actionId, const QVariant& contextObject)
{
	if (auto actionManager = impl_->get<IActionManager>())
	{
		auto action = actionManager->findAction(actionId.toUtf8().data());
		if (action != nullptr)
		{
			Variant oldData = action->getData();
			auto tempContextObject = impl_->get<IQtHelpers>()->toVariant(contextObject);
			action->setData(tempContextObject);
			if (action->visible() && action->enabled())
			{
				action->execute();
			}
			action->setData(oldData);
		}
	}
}

//------------------------------------------------------------------------------
bool QtScriptingEngine::canExecuteAction(const QString& actionId, const QVariant& contextObject)
{
	auto canExecute = false;
	if (auto actionManager = impl_->get<IActionManager>())
	{
		auto action = actionManager->findAction(actionId.toUtf8().data());
		if (action != nullptr)
		{
			Variant oldData = action->getData();
			auto tempContextObject = impl_->get<IQtHelpers>()->toVariant(contextObject);
			action->setData(tempContextObject);
			canExecute = action->visible() && action->enabled();
			action->setData(oldData);
		}
	}
	return canExecute;
}

} // end namespace wgt
