#include "qt_script_object.hpp"

#include "core_common/assert.hpp"
#include "core_qt_common/interfaces/i_qt_helpers.hpp"

#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "qt_scripting_engine.hpp"
#include "core_logging/logging.hpp"
#include "core_data_model/reflection/reflected_collection.hpp"
#include <QEvent>
#include <memory>

namespace wgt
{

namespace
{
PropertyAccessor bindProperty(IClassDefinition* definition, ObjectHandle& rootObject, const std::string& path, int propertyIndex, IDefinitionManager& definitionManager,
                              bool method = false)
{
	TF_ASSERT(propertyIndex >= 0);

	auto properties = definition->allProperties();
	auto it = properties.begin();
	while (it->isMethod() != method && it != properties.end())
	{
		++it;
	}
	for (; propertyIndex > 0 && it != properties.end(); ++it)
	{
		if (it->isMethod() == method && --propertyIndex == 0)
		{
			break;
		}
	}
	if (it == properties.end())
	{
		return PropertyAccessor();
	}

	auto rootDefinition = definitionManager.getDefinition(rootObject);
	if (rootDefinition == nullptr)
	{
		return PropertyAccessor();
	}

	auto fullPath = path.empty() ? it->getName() : path + "." + it->getName();
	return rootDefinition->bindProperty(fullPath.c_str(), rootObject);
}

int findPropertyId(IClassDefinition* definition, const IBasePropertyPtr& property)
{
	TF_ASSERT(property != nullptr);

	if (definition == nullptr)
	{
		return -1;
	}

	auto properties = definition->allProperties();
	int id = 0;

	for (auto itr = properties.begin(); itr != properties.end(); ++itr)
	{
		if (itr->getNameHash() == property->getNameHash())
		{
			return id;
		}

		if (itr->isMethod() == property->isMethod())
		{
			++id;
		}
	}

	return -1;
}
}

QtScriptObject::QtScriptObject(std::shared_ptr<QtScriptObjectData>& data, QObject* parent)
    : QObject(parent), data_(data)
{
	auto object = this->object();
	auto definition = data_->definition_;
	auto definitionManager = data_->get<IDefinitionManager>();
	TF_ASSERT(definitionManager != nullptr);
	
	auto properties = definition->allProperties();
	for (auto itr = properties.begin(); itr != properties.end(); ++itr)
	{
		const char* name = itr->getName();
		auto signalMeta = findFirstMetaData<MetaSignalObj>(*itr.get().get(), *definitionManager);
		if (signalMeta != nullptr)
		{
			auto self = this;
			auto property = itr.get();

			Signal<void(Variant&)>* signal = signalMeta->getSignal(object, *definitionManager);

			auto connection = signal->connect([self, property](Variant& v) { self->firePropertySignal(property, v); });

			signalConnections_.add(connection);
		}
	}
}

QtScriptObject::~QtScriptObject()
{
	signalConnections_.clear();
	data_->scriptEngine_.deregisterScriptObject(*this);
}

const QMetaObject* QtScriptObject::metaObject() const
{
	return data_->metaObject_;
}

//------------------------------------------------------------------------------
void QtScriptObject::setParent(QObject* parent)
{
	data_->scriptEngine_.swapParent(*this, parent);
	QObject::setParent(parent);
}

//------------------------------------------------------------------------------
int QtScriptObject::qt_metacall(QMetaObject::Call c, int id, void** argv)
{
	TF_ASSERT(data_->metaObject_ != nullptr);
	id = QObject::qt_metacall(c, id, argv);

	if (id < 0)
	{
		return id;
	}

	switch (c)
	{
	case QMetaObject::InvokeMetaMethod:
	{
		callMethod(id, argv);
		int methodCount = data_->metaObject_->methodCount() - data_->metaObject_->methodOffset();
		id -= methodCount;
		break;
	}
	case QMetaObject::ReadProperty:
	case QMetaObject::WriteProperty:
	{
		int propertyCount = data_->metaObject_->propertyCount() - data_->metaObject_->propertyOffset();

		if (id == 0)
		{
			if (c == QMetaObject::ReadProperty)
			{
				auto value = reinterpret_cast<QObject**>(argv[0]);
				*value = this;
			}

			id -= propertyCount;
			return id;
		}

		// The property offset is in our QtScriptObject
		auto property = bindProperty(data_->definition_, data_->rootObject_, data_->path_, id, *data_->get<IDefinitionManager>());

		if (property.isValid())
		{
			auto value = reinterpret_cast<QVariant*>(argv[0]);

			auto controller = data_->get<IReflectionController>();
			auto variant = controller ? controller->getValue(property) : property.getValue();
			if (variant.canCast<Collection>())
			{
				variant = Collection(std::make_shared<ReflectedCollection>(property, controller));
			}
			auto qtHelpers = data_->get<IQtHelpers>();
			if (c == QMetaObject::ReadProperty)
			{
				*value = qtHelpers->toQVariant(variant, this);
			}
			else
			{
				auto oldValue =
				qtHelpers->toQVariant(variant, this);
				if (*value != oldValue)
				{
					RefObjectId objectId;
					Variant valueVariant = qtHelpers->toVariant(*value);
					if (controller)
					{
						controller->setValue(property, valueVariant);
					}
					else
					{
						property.setValue(valueVariant);
					}
				}
			}
		}

		// not a property of this object. adjust the id and fall through
		id -= propertyCount;
	}
	break;
	default:
		break;
	}

	return id;
}

void QtScriptObject::firePropertySignal(const IBasePropertyPtr& property, const Variant& value)
{
	QVariant qvariant = data_->get<IQtHelpers>()->toQVariant(value, this);
	void* parameters[] = { nullptr, &qvariant };
	int signalId = findPropertyId(data_->definition_, property);
	callMethod(signalId, parameters);
}

void QtScriptObject::fireMethodSignal(const IBasePropertyPtr& method, bool undo)
{
	TF_ASSERT(data_->metaObject_ != nullptr);
	QVariant qvariant = undo;
	void* parameters[] = { nullptr, &qvariant };
	int methodId = findPropertyId(data_->definition_, method);
	int propertyCount = data_->metaObject_->propertyCount() - data_->metaObject_->propertyOffset();
	int firstMethodSignalId = propertyCount - 1;
	int nonReflectedMethodSignals = 2;
	int signalId = firstMethodSignalId + nonReflectedMethodSignals + methodId;
	callMethod(signalId, parameters);
}

void QtScriptObject::callMethod(int id, void** argv)
{
	TF_ASSERT(data_->metaObject_ != nullptr);
	int methodCount = data_->metaObject_->methodCount() - data_->metaObject_->methodOffset();

	if (id >= methodCount)
	{
		return;
	}

	int propertyCount = data_->metaObject_->propertyCount() - data_->metaObject_->propertyOffset();
	int firstMethodSignalId = propertyCount - 1;
	int methodSignalCount = (methodCount - firstMethodSignalId - 1) / 2;
	int firstMethodId = firstMethodSignalId + methodSignalCount;

	if (id < firstMethodId)
	{
		data_->metaObject_->activate(this, id + data_->metaObject_->methodOffset(), argv);
		return;
	}

	id -= firstMethodId;
	QVariant* result = reinterpret_cast<QVariant*>(argv[0]);

	auto qtHelpers = data_->get<IQtHelpers>();

	if (id < 3)
	{
		auto definition = data_->definition_;

		if (definition == nullptr)
		{
			return;
		}

		QString* property = reinterpret_cast<QString*>(argv[1]);
		QString* metaType = (id == 0) ? nullptr : reinterpret_cast<QString*>(argv[2]);

		switch (id)
		{
		case 0:
		{
			auto && meta = getMetaObject(definition, *property);

			if (meta == nullptr)
			{
				*result = QVariant::Invalid;
			}
			else
			{
				ObjectHandle handle = meta.getHandle();
				*result = qtHelpers->toQVariant(handle, this);
			}

			break;
		}
		case 1:
		{
			auto meta = getMetaObject(definition, *property, *metaType);

			if (meta == nullptr)
			{
				*result = QVariant::Invalid;
			}
			else
			{
				ObjectHandle handle = meta;
				*result = qtHelpers->toQVariant(handle, this);
			}

			break;
		}
		case 2:
		{
			bool found = getMetaObject(definition, *property, *metaType) != nullptr;
			*result = qtHelpers->toQVariant(Variant(found), this);
			break;
		}
		}

		{ // fire signal
			int signalId = id + firstMethodSignalId - 1;

			// the first two methods have the same name
			if (id == 0)
			{
				++signalId;
			}

			QVariant undo = false;
			void* parameters[] = { nullptr, &undo };
			callMethod(signalId, parameters);
		}
	}
	else
	{
		int methodId = id - 2;
		auto pa = bindProperty(data_->definition_, data_->rootObject_, data_->path_, methodId, *data_->get<IDefinitionManager>(), true);
		ReflectedMethodParameters parameters;

		for (size_t i = 0; i < pa.getProperty()->parameterCount(); ++i)
		{
			QVariant& qvariant = *reinterpret_cast<QVariant*>(argv[1 + i]);
			parameters.push_back(qtHelpers->toVariant(qvariant));
		}

		auto controller = data_->get<IReflectionController>();
		Variant returnValue = controller ? controller->invoke(pa, parameters) : pa.invoke(parameters);
		*result = qtHelpers->toQVariant(returnValue, this);
	}

	return;
}

const MetaData & QtScriptObject::getMetaObject(const IClassDefinition* definition, const QString& property) const
{
	if (property == "")
	{
		return definition->getMetaData();
	}
	else
	{
		auto properties = definition->allProperties();

		for (auto itr = properties.begin(); itr != properties.end(); ++itr)
		{
			if (property == itr->getName())
			{
				return itr->getMetaData();
			}
		}

		const auto index = property.lastIndexOf(Collection::getIndexOpen());
		if(index != -1)
		{
			QString containerProperty = property.left(index);
			for (auto itr = properties.begin(); itr != properties.end(); ++itr)
			{
				if (containerProperty == itr->getName() && itr->isCollection())
				{
					auto metaData = findFirstMetaData<MetaCollectionItemMetaObj>(
						*(*itr), *data_->get<IDefinitionManager>());
					if(metaData != nullptr)
					{
						return metaData->getMetaData();
					}
				}
			}
		}
	}

	static MetaData s_EmptyMeta;
	return s_EmptyMeta;
}

ObjectHandle QtScriptObject::getMetaObject(const IClassDefinition* definition, const QString& property,
                                         const QString& metaType) const
{
	QString metaClassName = "Meta" + metaType + "Obj";
	std::string strMetaClassName = metaClassName.toUtf8().constData();

	auto& definitionManager = *data_->get<IDefinitionManager>();
	auto && metaObject = getMetaObject(definition, property);
	auto targetDefinition = definitionManager.findDefinition(strMetaClassName.c_str());
	TF_ASSERT(targetDefinition);
	return MetaData::getMetaUtils().findFirstMetaData( *targetDefinition, metaObject, definitionManager );
}

ObjectHandle QtScriptObject::object() const
{
	if (data_->path_.empty())
	{
		return data_->rootObject_;
	}

	auto definitionManager = data_->get<IDefinitionManager>();

	auto rootDefinition = definitionManager->getDefinition(data_->rootObject_);
	auto accessor = rootDefinition->bindProperty(data_->path_.c_str(), data_->rootObject_);
	auto variant = accessor.getValue();
	ObjectHandle object;
	if (!variant.tryCast(object))
	{
		return nullptr;
	}

	auto root = reflectedRoot(object, *definitionManager);
	TF_ASSERT(definitionManager->getDefinition(root) == data_->definition_);
	return root;
}
} // end namespace wgt
