#include "qt_script_object.hpp"

#include "core_qt_common/helpers/qt_helpers.hpp"

#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "qt_scripting_engine.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
#include <QEvent>

namespace
{
	PropertyAccessor bindProperty( ObjectHandle & object, int propertyIndex, IDefinitionManager & definitionManager, bool method = false )
	{
		assert( propertyIndex >= 0 );
		auto definition = object.getDefinition( definitionManager );
		if (definition == nullptr)
		{
			return PropertyAccessor();
		}

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

		return definition->bindProperty( it->getName(), object );
	}


	int findPropertyId( IDefinitionManager & definitionManager, ObjectHandle& object, const IBasePropertyPtr & property )
	{
		assert( property != nullptr );
		auto definition = object.getDefinition( definitionManager );
		
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


QtScriptObject::~QtScriptObject()
{
	data_->scriptEngine_.deregisterScriptObject( *this );
}

const QMetaObject * QtScriptObject::metaObject() const
{
	return &data_->metaObject_;
}


//------------------------------------------------------------------------------
void QtScriptObject::setParent(QObject * parent)
{
	data_->scriptEngine_.swapParent( *this, parent );
	QObject::setParent( parent );
}


//------------------------------------------------------------------------------
int QtScriptObject::qt_metacall( QMetaObject::Call c, int id, void **argv )
{
	id = QObject::qt_metacall( c, id, argv );

	if (id < 0)
	{
		return id;
	}

	switch (c) 
	{
	case QMetaObject::InvokeMetaMethod:
		{
			callMethod( id, argv );
			int methodCount = data_->metaObject_.methodCount() - data_->metaObject_.methodOffset();
			id -= methodCount;
			break;
		}
	case QMetaObject::ReadProperty:
	case QMetaObject::WriteProperty:
		{
			int propertyCount = data_->metaObject_.propertyCount() - data_->metaObject_.propertyOffset();

			if (id == 0)
			{
				if (c == QMetaObject::ReadProperty)
				{
					auto value = reinterpret_cast< QObject ** >( argv[0] );
					*value = this;
				}

				id -= propertyCount;
				return id;
			}

			// The property offset is in our QtScriptObject
			auto property = bindProperty( data_->object_, id, *data_->get< IDefinitionManager >() );

			if (property.isValid())
			{
				auto value = reinterpret_cast< QVariant * >( argv[0] );

				auto controller = data_->get< IReflectionController >();
				if (c == QMetaObject::ReadProperty)
				{
					*value = QtHelpers::toQVariant( controller->getValue( property ), this );				}
				else
				{
					auto oldValue = QtHelpers::toQVariant( controller->getValue( property ), this );
					if (*value != oldValue)
					{
						RefObjectId objectId;
						Variant valueVariant = QtHelpers::toVariant( *value );
						controller->setValue( property, valueVariant );
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


void QtScriptObject::firePropertySignal( const IBasePropertyPtr & property, const Variant& value )
{
	QVariant qvariant = QtHelpers::toQVariant( value, this );
	void *parameters[] = { nullptr, &qvariant };
	int signalId = findPropertyId( *data_->get< IDefinitionManager>(), data_->object_, property );
	callMethod( signalId, parameters );
}


void QtScriptObject::fireMethodSignal( const IBasePropertyPtr & method, bool undo )
{
	QVariant qvariant = undo;
	void *parameters[] = { nullptr, &qvariant };
	int methodId = findPropertyId( *data_->get< IDefinitionManager >(), data_->object_, method );
	int propertyCount = data_->metaObject_.propertyCount() - data_->metaObject_.propertyOffset();
	int firstMethodSignalId = propertyCount - 1;
	int nonReflectedMethodSignals = 2;
	int signalId = firstMethodSignalId + nonReflectedMethodSignals + methodId;
	callMethod( signalId, parameters );
}


void QtScriptObject::callMethod( int id, void **argv )
{
	int methodCount = data_->metaObject_.methodCount() - data_->metaObject_.methodOffset();

	if (id >= methodCount)
	{
		return;
	}

	int propertyCount = data_->metaObject_.propertyCount() - data_->metaObject_.propertyOffset();
	int firstMethodSignalId = propertyCount - 1;
	int methodSignalCount = (methodCount - firstMethodSignalId - 1) / 2;
	int firstMethodId = firstMethodSignalId + methodSignalCount;

	if (id < firstMethodId)
	{
		data_->metaObject_.activate( this, id + data_->metaObject_.methodOffset(), argv );
		return;
	}

	id -= firstMethodId;
	QVariant* result = reinterpret_cast<QVariant*>( argv[0] );

	if (id < 3)
	{
		auto definition = data_->object_.getDefinition( *data_->get< IDefinitionManager >() );

		if (definition == nullptr)
		{
			return;
		}

		QString* property = reinterpret_cast<QString*>( argv[1] );
		QString* metaType = (id == 0) ? nullptr : reinterpret_cast<QString*>( argv[2] );

		switch (id)
		{
		case 0:
			{
				auto meta = getMetaObject( definition, *property );

				if (meta == nullptr)
				{
					*result = QVariant::Invalid;
				}
				else
				{
					ObjectHandle handle = meta;
					*result = QtHelpers::toQVariant( handle, this );
				}

				break;
			}
		case 1:
			{
				auto meta = getMetaObject( definition, *property, *metaType );

				if (meta == nullptr)
				{
					*result = QVariant::Invalid;
				}
				else
				{
					ObjectHandle handle = meta;
					*result = QtHelpers::toQVariant( handle, this );
				}

				break;
			}
		case 2:
			{
				bool found = getMetaObject( definition, *property, *metaType ) != nullptr;
				*result = QtHelpers::toQVariant( Variant( found ), this );
				break;
			}
		}

		{// fire signal
			int signalId = id + firstMethodSignalId - 1;

			// the first two methods have the same name
			if (id == 0)
			{
				++ signalId;
			}

			QVariant undo = false;
			void* parameters[] = {nullptr, &undo};
			callMethod( signalId, parameters );
		}
	}
	else
	{
		int methodId = id - 2;
		auto pa = bindProperty(data_->object_, methodId, *data_->get< IDefinitionManager >(), true );
		ReflectedMethodParameters parameters;

		for (size_t i = 0; i < pa.getProperty()->parameterCount(); ++i)
		{
			QVariant& qvariant = *reinterpret_cast<QVariant*>( argv[1 + i] );
			parameters.push_back( QtHelpers::toVariant( qvariant ) );
		}

		Variant returnValue = data_->get< IReflectionController >()->invoke( pa, parameters );
		*result = QtHelpers::toQVariant( returnValue, this );
	}

	return;
}


MetaHandle QtScriptObject::getMetaObject(
	const IClassDefinition* definition,
	const QString& property ) const
{
	MetaHandle meta = nullptr;
	
	if (property == "")
	{
		meta = definition->getMetaData();
	}
	else
	{
		auto properties = definition->allProperties();

		for (auto itr = properties.begin(); itr != properties.end(); ++itr)
		{
			if (property == itr->getName())
			{
				meta = itr->getMetaData();
				break;
			}
		}
	}

	return meta;
}


MetaHandle QtScriptObject::getMetaObject(
	const IClassDefinition* definition,
	const QString& property,
	const QString& metaType ) const
{
	QString metaClassName = "class wgt::Meta" + metaType + "Obj";
	std::string strMetaClassName = metaClassName.toUtf8().constData();
	TypeId metaTypeId(strMetaClassName.c_str());

	auto metaObject = getMetaObject( definition, property );
	return findFirstMetaData( metaTypeId, metaObject, *data_->get< IDefinitionManager >() );
}


const ObjectHandle & QtScriptObject::object() const
{ 
	return data_->object_;
}
} // end namespace wgt
