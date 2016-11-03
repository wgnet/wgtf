#include "pch.hpp"

#include "definition_details.hpp"
#include "defined_instance.hpp"
#include "property.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_storage.hpp"
#include "core_reflection/base_property_with_metadata.hpp"

namespace wgt
{
namespace
{


/**
 *	Get metadata from class.
 *	@param name of attribute.
 *	@param metaData metadata found on Python class.
 *		Can be null.
 *	@return metadata or null.
 *		Caller is responsible for deleting metadata.
 */
MetaHandle extractMetaData( const char * name,
	const PyScript::ScriptDict & metaData )
{
	assert( name != nullptr );
	assert( strlen( name ) > 0 );
	if (name[0] == '_')
	{
		// Members that start with an underscore are private
		// Mark it as hidden
		return MetaHidden();
	}

	if (!metaData.exists())
	{
		// Class has no metadata
		return nullptr;
	}

	auto metaItem = metaData.getItem( name, PyScript::ScriptErrorClear() );
	if (!metaItem.exists())
	{
		// Class has metadata, but none for this attribute
		return nullptr;
	}

	// Metadata should always be of the format
	// { "attribute" : "string" }
	// TODO NGT-1559 use a class instead of strings
	// TODO NGT-1559 support all MetaBase types
	auto metaTypeString = PyScript::ScriptString::create( metaItem );
	assert( metaTypeString );

	// Convert Python metadata to C++ metadata
	if (strcmp( metaTypeString.c_str(), "MetaNone" ) == 0)
	{
		NGT_WARNING_MSG( "MetaNone not supported, just leave entry blank.\n" );
		return nullptr;
	}
	else if (strcmp( metaTypeString.c_str(), "MetaNoNull" ) == 0)
	{
		return MetaNoNull();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaColor" ) == 0)
	{
		return MetaColor();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaSlider" ) == 0)
	{
		return MetaMinMax( 0.0f, 5.0f ) + MetaStepSize( 1.0f ) + MetaDecimals( 1 ) + MetaSlider();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaHidden" ) == 0)
	{
		return MetaHidden();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaReadOnly" ) == 0)
	{
		return MetaReadOnly();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaNoSerialization" ) == 0)
	{
		return MetaNoSerialization();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaOnStack" ) == 0)
	{
		return MetaOnStack();
	}

	return nullptr;
}

} // namespace


namespace ReflectedPython
{

class PropertyIterator : public PropertyIteratorImplBase
{
public:	
	PropertyIterator( IComponentContext & context,
		const PyScript::ScriptObject& pythonObject,
		const PyScript::ScriptDict & metaDataDict )
		: context_( context )
		, object_( pythonObject )
		, metaDataDict_( metaDataDict )
	{
		if (object_.get() == nullptr)
		{
			return;
		}

		// Get a list of strings appropriate for object arguments
		PyScript::ScriptObject dir = object_.getDir( PyScript::ScriptErrorPrint() );
		if (dir.get() != nullptr)
		{
			iterator_ = dir.getIter( PyScript::ScriptErrorPrint() );
		}
	}

	IBasePropertyPtr current() const
	{
		return current_;
	}

	bool next()
	{

		if (iterator_.get() == nullptr)
		{
            current_.reset();
			return false;
		}

		while (PyScript::ScriptObject key = iterator_.next())
		{
			PyScript::ScriptString str = key.str( PyScript::ScriptErrorPrint() );
			const char * name = str.c_str();

			// Some properties from dir are not accessible as attributes
			// e.g. __abstractmethods__ is a descriptor
			if (!object_.hasAttribute( name ))
			{
				continue;
			}

			auto meta = extractMetaData( name, metaDataDict_ );
            if (current_ == nullptr)
            {
                current_ = std::make_shared< ReflectedPython::Property >(
                    context_, name, object_, meta);
            }
            else
            {
                auto property = static_cast<ReflectedPython::Property*>(current_.get());
                property->updatePropertyData(name, object_, meta);
            }
			return true;
		}
        current_.reset();
		return false;
	}

private:
	IComponentContext &		context_;
	PyScript::ScriptObject	object_;
	PyScript::ScriptDict	metaDataDict_;
	PyScript::ScriptIter	iterator_;
	IBasePropertyPtr		current_;
};


DefinitionDetails::DefinitionDetails( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
	: context_( context )
	, name_( DefinitionDetails::generateName( pythonObject ) )
	, pythonObject_( pythonObject )
	, metaData_( MetaNone() )
{
	assert( !name_.empty() );

	// Assume that _metaData is not modified after creation
	const char * metaDataName = "_metaData";
	const auto metaDataAttribute = pythonObject.getAttribute( metaDataName,
		PyScript::ScriptErrorClear() );
	metaDataDict_ = PyScript::ScriptDict::create( metaDataAttribute );

	attachListenerHooks( pythonObject_ );
}


DefinitionDetails::~DefinitionDetails()
{
	detachListenerHooks( pythonObject_ );
}


bool DefinitionDetails::isAbstract() const
{
	return false;
}

bool DefinitionDetails::isGeneric() const
{
	return true;
}

const char * DefinitionDetails::getName() const
{
	return name_.c_str();
}

const char * DefinitionDetails::getParentName() const
{
	// TODO NGT-1225 inheritance not implemented
	// All new-style Python classes inherit from 'object'
	return nullptr;
}

MetaHandle DefinitionDetails::getMetaData() const
{
	return metaData_;
}

ObjectHandle DefinitionDetails::create( const IClassDefinition & classDefinition ) const
{
	// Python definitions should be created based on a PyScript::PyObject

	// If this Python object is a type; create an instance of that type
	auto scriptType = PyScript::ScriptType::create( pythonObject_ );
	if (!scriptType.exists())
	{
		// If this Python object is an instance; clone the instance
		scriptType = PyScript::ScriptType::getType( pythonObject_ );
	}

	// Clone instance
	auto newPyObject = scriptType.genericAlloc( PyScript::ScriptErrorPrint() );
	if (newPyObject == nullptr)
	{
		return nullptr;
	}

	ObjectHandle parentHandle;
	const char * childPath = "";
	return DefinedInstance::findOrCreate( context_,
		PyScript::ScriptObject( newPyObject,
			PyScript::ScriptObject::FROM_NEW_REFERENCE ),
		parentHandle,
		childPath );
}


void * DefinitionDetails::upCast( void * object ) const
{
	return nullptr;
}


bool DefinitionDetails::canDirectLookupProperty() const /* override */
{
	return true;
}


IBasePropertyPtr DefinitionDetails::directLookupProperty( const char * name ) const /* override */
{
	// Some properties from dir are not accessible as attributes
	// e.g. __abstractmethods__ is a descriptor
	if (!pythonObject_.hasAttribute( name ))
	{
		return nullptr;
	}

	auto meta = extractMetaData( name, metaDataDict_ );
    return std::make_shared< ReflectedPython::Property >(
        context_,
        name,
        pythonObject_, meta );
}


PropertyIteratorImplPtr DefinitionDetails::getPropertyIterator() const
{
	return std::make_shared< PropertyIterator >( context_,
		pythonObject_,
		metaDataDict_ );
}


IClassDefinitionModifier * DefinitionDetails::getDefinitionModifier() const
{
	return const_cast< DefinitionDetails * >( this );
}


IBasePropertyPtr DefinitionDetails::addProperty( const char * name, const TypeId & typeId, MetaHandle metaData )
{
	return std::make_shared< ReflectedPython::Property >( context_,
		name,
		typeId,
		pythonObject_, metaData );
}


std::string DefinitionDetails::generateName( const PyScript::ScriptObject & object )
{
	PyScript::ScriptErrorPrint errorHandler;
	std::string typeName;

	if (PyScript::ScriptType::check( object ))
	{
		// Type type
		// type.__module__ + type.__name__ + id( object )
		PyScript::ScriptType scriptType(
			reinterpret_cast<PyTypeObject*>( object.get() ), PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		scriptType.getAttribute( "__module__", typeName, errorHandler );
		typeName += '.';
		typeName += scriptType.name();
	}
	else
	{
		// Class or None type
		// __module__ + __name__ + id( object )
		typeName = object.str( errorHandler ).c_str();
	}

	// Add an address in case there are multiple classes/types with the same
	// name in the same file.
	// E.g.
	// # module "Test"
	// class A( object ):
	//     class InnerClass( object ):
	//         pass
	//     pass
	// class B( object ):
	//     class InnerClass( object ): # different type with the name "Test.InnerClass"
	//         pass
	//     pass
	typeName += " at ";
	typeName += std::to_string( object.id().asUnsignedLongLong(
		PyScript::ScriptErrorRetain() ) );

	// Check for overflow
	assert( !PyScript::Script::hasError() );
#if defined( _DEBUG )
	PyScript::Script::clearError();
#endif // defined( _DEBUG )

	return typeName;
}


const PyScript::ScriptObject & DefinitionDetails::object() const
{
	return pythonObject_;
}


} // namespace ReflectedPython
} // end namespace wgt
