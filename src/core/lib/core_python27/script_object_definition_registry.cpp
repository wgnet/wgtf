#include "pch.hpp"
#include "script_object_definition_registry.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "definition_details.hpp"
#include "definition_helper.hpp"

#include "structmember.h"

namespace wgt
{
const char * g_reflectionDefinition = "__reflectionDefinition";


/**
 *	Type for wrapping C++ definition info into a Python object.
 *	@see https://docs.python.org/2/extending/newtypes.html
 */
typedef struct {
	PyObject_HEAD
	std::weak_ptr< IClassDefinition > definition_;
	RefObjectId id_;
} PyDefinitionObject;


static PyTypeObject Definition_Type = {
	PyObject_HEAD_INIT( nullptr )
	0, /* ob_size */
	"Reflection.DefinitionType", /* tp_name */
	sizeof( PyDefinitionObject ), /* tp_basicsize */
	0, /* tp_itemsize */
	0, /* tp_dealloc */
	0, /* tp_print */
	0, /* tp_getattr */
	0, /* tp_setattr */
	0, /* tp_compare */
	0, /* tp_repr */
	0, /* tp_as_number */
	0, /* tp_as_sequence */
	0, /* tp_as_mapping */
	0, /* tp_hash  */
	0, /* tp_call */
	0, /* tp_str */
	0, /* tp_getattro */
	0, /* tp_setattro */
	0, /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT, /* tp_flags */
	"DefinitionType objects", /* tp_doc */
	0, /* tp_traverse */
	0, /* tp_clear */
	0, /* tp_richcompare */
	0, /* tp_weaklistoffset */
	0, /* tp_iter */
	0, /* tp_iternext */
	0, /* tp_methods */
	0, /* tp_members */
	0, /* tp_getset */
	0, /* tp_base */
	0, /* tp_dict */
	0, /* tp_descr_get */
	0, /* tp_descr_set */
	0, /* tp_dictoffset */
	0, /* tp_init */
	0, /* tp_alloc */
	0, /* tp_new */
};


static PyMethodDef module_methods[] = {
	{ nullptr } /* Sentinel */
};


#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initDefinitionType( void ) 
{
	PyObject* m;

	if (PyType_Ready( &Definition_Type ) < 0)
	{
		return;
	}

	m = Py_InitModule3( "Reflection", module_methods, "Reflection system module." );

	if (m == nullptr)
	{
		return;
	}

	Py_INCREF( &Definition_Type );
	PyModule_AddObject( m,
		"DefinitionType",
		reinterpret_cast< PyObject * >( &Definition_Type ) );
}


namespace ReflectedPython
{


struct ScriptObjectDefinitionDeleter
{
	ScriptObjectDefinitionDeleter( const PyScript::ScriptObject& object, ScriptObjectDefinitionRegistry& registry )
		: object_( object )
		, registry_( registry )
	{}


	void operator()( IClassDefinition* definition )
	{
		registry_.removeDefinition( object_, definition );
	}


	const PyScript::ScriptObject object_;
	ScriptObjectDefinitionRegistry& registry_;
};


template< typename PAIR_T >
class PairMatch
{
public:
	PairMatch( const PyScript::ScriptObject & match );
	bool operator()( const PAIR_T & entry ) const;
private:
	const PyScript::ScriptObject & match_;
};


template< typename PAIR_T >
PairMatch< PAIR_T >::PairMatch( const PyScript::ScriptObject & match )
	: match_( match )
{
}


template< typename PAIR_T >
bool PairMatch< PAIR_T >::operator()( const PAIR_T & entry ) const
{
	const auto & a = entry.first;
	const auto & b = match_;
	const auto result = a.compareTo( b, PyScript::ScriptErrorRetain() ) == 0;
	if (PyScript::Script::hasError())
	{
		// Error comparing objects, just compare pointer addresses
		PyScript::Script::clearError();
		return (a.get() == b.get());
	}
	return result;
}


ScriptObjectDefinitionRegistry::ScriptObjectDefinitionRegistry( IComponentContext& context )
	: context_( context )
	, definitionManager_( context )
{
}


ScriptObjectDefinitionRegistry::~ScriptObjectDefinitionRegistry()
{
}


void ScriptObjectDefinitionRegistry::init()
{
	assert( definitionManager_ != nullptr );

	definitionHelper_.reset( new ReflectedPython::DefinitionHelper );
	definitionManager_->registerDefinitionHelper( *definitionHelper_ );
	initDefinitionType();

}


void ScriptObjectDefinitionRegistry::fini()
{
	assert( definitionManager_ != nullptr );
	definitionManager_->deregisterDefinitionHelper( *definitionHelper_ );
	definitionHelper_.reset();
}


std::shared_ptr< IClassDefinition > ScriptObjectDefinitionRegistry::findOrCreateDefinition(
	const PyScript::ScriptObject & object )
{
	assert( object.exists() );
	assert( definitionManager_ != nullptr );

	// Check if definition is attached to object
	// Rather than searching through list
	{
		PyScript::ScriptType definitionType( &Definition_Type,
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		auto definitionObject = object.getAttribute( g_reflectionDefinition,
			PyScript::ScriptErrorClear() );
		if (definitionObject.exists())
		{
			const auto typeMatches = definitionType.isObjectOfType( definitionObject );
			assert( typeMatches );
			if (typeMatches)
			{
				auto definitionType =
					reinterpret_cast< PyDefinitionObject * >( definitionObject.get() );
				auto pointer = definitionType->definition_.lock();
				if (pointer != nullptr)
				{
					return pointer;
				}
				else
				{
					std::string definitionName =
						ReflectedPython::DefinitionDetails::generateName( object );
					assert( !definitionName.empty() );

					auto definition = definitionManager_->getDefinition( definitionName.c_str() );
					assert( definition != nullptr );
					if (definition != nullptr)
					{
						definitionManager_->deregisterDefinition( definition );
					}
				}
			}
		}

		// Check if object can be modified
		// Also setting the attribute to None helps detect re-entry to
		// findDefinition() by listener hooks
		const auto canSet = definitionObject.exists() ||
			object.setAttribute( g_reflectionDefinition,
				PyScript::ScriptObject::none(),
				PyScript::ScriptErrorClear() );
		// Do not attach to type objects
		// because getAttribute will find the Class.__reflectionDefinition
		// before the instance.__reflectionDefinition
		// TODO find a way to fix this so that lookup can be removed
		const auto isType = PyScript::ScriptType::check( object );
		const auto isClass = PyScript::ScriptClass::check( object );
		if (canSet && !isType && !isClass)
		{
			auto definition = definitionManager_->registerDefinition( std::unique_ptr<IClassDefinitionDetails>(
				new ReflectedPython::DefinitionDetails( context_, object ) ) );
			assert( definition != nullptr );

			std::shared_ptr< IClassDefinition > pointer( definition,
				ScriptObjectDefinitionDeleter( object, *this ) );

			definitionObject = PyScript::ScriptObject(
				definitionType.genericAlloc( PyScript::ScriptErrorPrint() ),
				PyScript::ScriptObject::FROM_NEW_REFERENCE );
			auto definitionType =
				reinterpret_cast< PyDefinitionObject * >( definitionObject.get() );
			definitionType->definition_ = pointer;
			definitionType->id_ = RefObjectId::generate();

			const auto success = object.setAttribute( g_reflectionDefinition,
				definitionObject,
				PyScript::ScriptErrorClear() );
			assert( success );
			return pointer;
		}
		else
		{
			object.delAttribute( g_reflectionDefinition,
				PyScript::ScriptErrorClear() );
			assert( !object.hasAttribute( g_reflectionDefinition ) );
		}
	}

	// Some Python objects cannot be modified, so fall back to storing separately
	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	// Find uses a ScriptObject comparator which may raise script errors
	auto itr = std::find_if( definitions_.cbegin(),
		definitions_.cend(),
		PairMatch< DefinitionPair >( object ) );

	if (itr != definitions_.end())
	{
		std::shared_ptr<IClassDefinition> pointer = itr->second.lock();

		if (pointer != nullptr)
		{
			return pointer;
		}

		std::string definitionName = ReflectedPython::DefinitionDetails::generateName( object );
		assert( !definitionName.empty() );

		auto definition = definitionManager_->getDefinition( definitionName.c_str() );
		assert( definition != nullptr );
		definitionManager_->deregisterDefinition( definition );
	}

	auto definition = definitionManager_->registerDefinition( std::unique_ptr<IClassDefinitionDetails>(
		new ReflectedPython::DefinitionDetails( context_, object ) ) );
	assert( definition != nullptr );

	std::shared_ptr<IClassDefinition> pointer( definition, ScriptObjectDefinitionDeleter( object, *this ) );
	definitions_.emplace_back( DefinitionPair( object, pointer ) );
	ids_.emplace_back( IdPair( object, RefObjectId::generate() ) );

	return pointer;
}


std::shared_ptr< IClassDefinition > ScriptObjectDefinitionRegistry::findDefinition(
	const PyScript::ScriptObject & object )
{
	assert( object.exists() );

	PyScript::ScriptType definitionType( &Definition_Type,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	auto definitionObject = object.getAttribute( g_reflectionDefinition,
		PyScript::ScriptErrorClear() );
	if (definitionObject.exists())
	{
		const auto typeMatches = definitionType.isObjectOfType( definitionObject );
		if (typeMatches)
		{
			auto definitionType = 
				reinterpret_cast< PyDefinitionObject * >( definitionObject.get() );
			return definitionType->definition_.lock();
		}
		else
		{
			// This can happen during findOrCreateDefinition()
			// If listener hooks are enabled
			// In which case definitionType will be None
			assert( definitionObject.isNone() );
			return nullptr;
		}
	}

	std::lock_guard< std::mutex > lock( definitionsMutex_ );
	// Find uses a ScriptObject comparator which may raise script errors
	auto itr = std::find_if( definitions_.cbegin(),
		definitions_.cend(),
		PairMatch< DefinitionPair >( object ) );

	if (itr != definitions_.cend())
	{
		return itr->second.lock();
	}

	return nullptr;
}


void ScriptObjectDefinitionRegistry::removeDefinition(
	const PyScript::ScriptObject& object, const IClassDefinition* definition )
{
	{
		auto definitionObject = object.getAttribute( g_reflectionDefinition,
			PyScript::ScriptErrorClear() );
		if (definitionObject.exists())
		{
			const auto deleted = object.delAttribute( g_reflectionDefinition,
				PyScript::ScriptErrorClear() );
			assert( deleted );

			assert( definitionManager_ != nullptr );
			const bool deregistered = definitionManager_->deregisterDefinition( definition );
			assert( deregistered );
			return;
		}
	}

	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	assert( definition != nullptr );

	auto itr = std::find_if( definitions_.cbegin(),
		definitions_.cend(),
		PairMatch< DefinitionPair >( object ) );

	if (itr == definitions_.cend() || itr->second.use_count() > 0)
	{
		return;
	}

	definitions_.erase( itr );
	auto idItr = std::find_if( ids_.cbegin(),
		ids_.cend(),
		PairMatch< IdPair >( object ) );
	assert( idItr != ids_.cend() );
	ids_.erase( idItr );

	assert( definitionManager_ != nullptr );
	const bool success = definitionManager_->deregisterDefinition( definition );
	assert( success );
}


const RefObjectId & ScriptObjectDefinitionRegistry::getID(
	const PyScript::ScriptObject & object ) /* override */
{
	assert( object.exists() );

	{
		PyScript::ScriptType definitionType( &Definition_Type,
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		auto definitionObject = object.getAttribute( g_reflectionDefinition,
			PyScript::ScriptErrorClear() );
		if (definitionObject.exists())
		{
			const auto typeMatches = definitionType.isObjectOfType( definitionObject );
			assert( typeMatches );
			if (typeMatches)
			{
				auto definitionType = 
					reinterpret_cast< PyDefinitionObject * >( definitionObject.get() );
				return definitionType->id_;
			}
		}
	}

	std::lock_guard< std::mutex > lock( definitionsMutex_ );

	auto itr = std::find_if( ids_.cbegin(),
		ids_.cend(),
		PairMatch< IdPair >( object ) );

	// Object must have been registered with getDefinition()
	assert( itr != ids_.cend() );

	return itr->second;
}


} // namespace ReflectedPython
} // end namespace wgt
