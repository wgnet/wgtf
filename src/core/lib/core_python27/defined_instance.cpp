#include "pch.hpp"

#include "defined_instance.hpp"
#include "definition_details.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "i_script_object_definition_registry.hpp"


namespace wgt
{
namespace ReflectedPython
{


DefinedInstance::DefinedInstance()
	: BaseGenericObject()
	, pythonObject_( nullptr )
	, pDefinition_( nullptr )
	, context_( nullptr )
	, parentHandle_( nullptr )
	, pRoot_( this )
{
	assert( false && "Always construct with a Python object" );
}


DefinedInstance::DefinedInstance(
	IComponentContext & context,
	const PyScript::ScriptObject & pythonObject,
	std::shared_ptr< IClassDefinition > & definition,
	const ObjectHandle & parentHandle,
	const std::string & childPath )
	: BaseGenericObject()
	, pythonObject_( pythonObject )
	, pDefinition_( definition )
	, context_( &context )
	, parentHandle_( parentHandle )
	, pRoot_( this )
{
	auto pParentInstance = parentHandle_.getBase< DefinedInstance >();

	// parentHandle_ must be null or contain a DefinedInstance
	assert( !parentHandle_.isValid() || (pParentInstance != nullptr) );
	if (pParentInstance != nullptr)
	{
		// Detect cycles
		assert( pParentInstance->pythonObject() != pythonObject_ );

		// Update root object
		pRoot_ = &pParentInstance->root();

		// Add parent path
		fullPath_ = pParentInstance->fullPath();
		if (!fullPath_.empty() && !childPath.empty())
		{
			if (childPath[0] != IClassDefinition::INDEX_OPEN)
			{
				fullPath_ += IClassDefinition::DOT_OPERATOR;
			}
		}
	}

	// Add child path after parent path
	fullPath_ += childPath;

	setDefinition( pDefinition_.get() );
}


DefinedInstance::~DefinedInstance()
{
}


/* static */ ObjectHandle DefinedInstance::findOrCreate( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject,
	const ObjectHandle & parentHandle,
	const std::string & childPath )
{
	assert( pythonObject.exists() );

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = context.queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pDefinition = registry.findOrCreateDefinition( pythonObject );
	assert( pDefinition != nullptr );
	auto & definition = (*pDefinition);

	const auto & id = registry.getID( pythonObject );

	// Search for an existing object handle that's using that definition
	auto pObjectManager = context.queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);
	auto handle = objectManager.getObject( id );
	if (handle.isValid())
	{
		return handle;
	}

	// Existing object handle not found, construct a new instance
	auto pInstance = std::unique_ptr< ReflectedPython::DefinedInstance >(
		new DefinedInstance( context, pythonObject, pDefinition, parentHandle, childPath ) );
	ObjectHandleT< ReflectedPython::DefinedInstance > handleT(
		std::move( pInstance ),
		&definition );
	handle = handleT;

	// Register with IObjectManager to generate an ID
	// IObjectManager should take a weak reference
	handle = objectManager.registerObject( handle, id );
	assert( handle.isValid() );

	// Registered reference
	return handle;
}


/* static */ ObjectHandle DefinedInstance::find( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
{
	assert( pythonObject.exists() );

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = context.queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pDefinition = registry.findDefinition( pythonObject );
	if (pDefinition == nullptr)
	{
		return nullptr;
	}
	auto & definition = (*pDefinition);

	const auto & id = registry.getID( pythonObject );

	// Search for an existing object handle that's using that definition
	auto pObjectManager = context.queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);
	return objectManager.getObject( id );
}


const PyScript::ScriptObject & DefinedInstance::pythonObject() const
{
	return pythonObject_;
}


const DefinedInstance & DefinedInstance::root() const
{
	assert( pRoot_ != nullptr );
	return (*pRoot_);
}


const std::string & DefinedInstance::fullPath() const
{
	return fullPath_;
}


ObjectHandle DefinedInstance::getDerivedType() const
{
	auto pRegistry = context_->queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pObjectManager = context_->queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);

	const auto & id = registry.getID( pythonObject_ );
	return objectManager.getObject( id );
}


ObjectHandle DefinedInstance::getDerivedType()
{
	auto pRegistry = context_->queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pObjectManager = context_->queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);

	const auto & id = registry.getID( pythonObject_ );
	return objectManager.getObject( id );
}


TextStream& operator<<( TextStream& stream, const DefinedInstance& value )
{
	if(auto definition = value.getDefinition())
	{
		stream << definition->getName();
	}
	else
	{
		stream << "<empty DefinedInstance>";
	}

	return stream;
}


} // namespace ReflectedPython
} // end namespace wgt
