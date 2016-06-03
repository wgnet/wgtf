#include "context_definition_manager.hpp"
#include "core_serialization/serializer/i_serializer.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/generic/generic_definition.hpp"



namespace wgt
{
//==============================================================================
ContextDefinitionManager::ContextDefinitionManager( const wchar_t * contextName )
	: pBaseManager_ ( NULL )
	, contextName_( contextName )
{
}


//==============================================================================
ContextDefinitionManager::~ContextDefinitionManager()
{
	IObjectManager * pObjManager = getObjectManager();
	assert( pObjManager );
	if (pObjManager)
	{
		pObjManager->deregisterContext( this );
	}

	for (auto it = contextDefinitions_.begin();
		it != contextDefinitions_.end(); )
	{
		auto preIt = it;
		preIt++;
		auto definition = *it;
		deregisterDefinition( definition );
		it = preIt;
	}
}


//==============================================================================
void ContextDefinitionManager::init( IDefinitionManager * pBaseManager )
{
	assert( !pBaseManager_ && pBaseManager );
	pBaseManager_ = pBaseManager;

	IObjectManager * pObjManager = getObjectManager();
	assert( pObjManager );
	if (pObjManager)
	{
		pObjManager->registerContext( this );
	}
}


//==============================================================================
IDefinitionManager * ContextDefinitionManager::getBaseManager() const
{
	return pBaseManager_;
}


//==============================================================================
IClassDefinition * ContextDefinitionManager::getDefinition(
	const char * name ) const
{
	assert( pBaseManager_ );
	return pBaseManager_->getDefinition( name );
}


//==============================================================================
IClassDefinition * ContextDefinitionManager::getObjectDefinition( const ObjectHandle & object ) const
{
	assert( pBaseManager_ );
	return pBaseManager_->getObjectDefinition( object );
}


//==============================================================================
IClassDefinition * ContextDefinitionManager::registerDefinition( std::unique_ptr<IClassDefinitionDetails> defDetails )
{
	assert( defDetails );
	assert( pBaseManager_ );
	IClassDefinitionModifier * modifier = nullptr;
	auto definition = pBaseManager_->registerDefinition( std::move(defDetails) );
	if (definition)
	{
		definition->setDefinitionManager( this );
		contextDefinitions_.insert( definition );
	}
	return definition;
}


//==============================================================================
bool ContextDefinitionManager::deregisterDefinition( const IClassDefinition * definition )
{
	assert( definition );
	assert( pBaseManager_ );
	auto it = contextDefinitions_.find( definition );
	assert( it != contextDefinitions_.end() );
	if ( it == contextDefinitions_.end())
	{
		return false;
	}
	contextDefinitions_.erase( it );
	auto ok = pBaseManager_->deregisterDefinition( definition );
	delete definition;
	return ok;
}


//==============================================================================
void ContextDefinitionManager::getDefinitionsOfType(
	const IClassDefinition * definition,
	std::vector< IClassDefinition * > & o_Definitions ) const
{
	assert( pBaseManager_ );
	pBaseManager_->getDefinitionsOfType( definition, o_Definitions );
}


//==============================================================================
void ContextDefinitionManager::getDefinitionsOfType(
	const std::string & type,
	std::vector< IClassDefinition * > & o_Definitions ) const
{
	assert( pBaseManager_ );
	pBaseManager_->getDefinitionsOfType( type, o_Definitions );
}


//==============================================================================
IObjectManager * ContextDefinitionManager::getObjectManager() const
{
	assert( pBaseManager_ );
	return pBaseManager_->getObjectManager();
}


//==============================================================================
void ContextDefinitionManager::registerDefinitionHelper( const IDefinitionHelper & helper )
{
	assert( pBaseManager_ );
	pBaseManager_->registerDefinitionHelper( helper );
}


//==============================================================================
void ContextDefinitionManager::deregisterDefinitionHelper( const IDefinitionHelper & helper )
{
	assert( pBaseManager_ );
	pBaseManager_->deregisterDefinitionHelper( helper );
}


//==============================================================================
void ContextDefinitionManager::registerPropertyAccessorListener(
	std::shared_ptr< PropertyAccessorListener > & listener )
{
	assert( pBaseManager_ );
	pBaseManager_->registerPropertyAccessorListener( listener );
}


//==============================================================================
void ContextDefinitionManager::deregisterPropertyAccessorListener(
	std::shared_ptr< PropertyAccessorListener > &  listener )
{
	assert( pBaseManager_ );
	pBaseManager_->deregisterPropertyAccessorListener( listener );
}


//==============================================================================
const IDefinitionManager::PropertyAccessorListeners &
	ContextDefinitionManager::getPropertyAccessorListeners() const
{
	assert( pBaseManager_ );
	return pBaseManager_->getPropertyAccessorListeners();
}


//==============================================================================
bool ContextDefinitionManager::serializeDefinitions( ISerializer & serializer )
{
	std::set<const IClassDefinition *> genericDefs;
	for (auto & definition : contextDefinitions_)
	{
		if(definition->isGeneric())
		{
			genericDefs.insert( definition );
		}
	}

	serializer.serialize( genericDefs.size() );
	for (auto & classDef : genericDefs)
	{
		assert( classDef );
		serializer.serialize( classDef->getName() );
		auto parent = classDef->getParent();
		serializer.serialize( parent ? parent->getName() : "" );

		// write all properties
		std::vector<IBasePropertyPtr> baseProps;
		for (PropertyIterator pi = classDef->directProperties().begin(),
			end = classDef->directProperties().end(); (pi != end); ++pi)
		{
			auto metaData = findFirstMetaData<MetaNoSerializationObj>( *(*pi), *this );
			if (metaData != nullptr)
			{
				continue;
			}
			baseProps.push_back( *pi );
		}
		size_t count = baseProps.size();
		serializer.serialize( count );
		for (auto baseProp : baseProps)
		{
			assert( baseProp );
			serializer.serialize( baseProp->getName() );
			auto metaType = Variant::findType( baseProp->getType() );
			if (metaType != nullptr)
			{
				serializer.serialize( metaType->name() );
			}
			else
			{
				serializer.serialize( baseProp->getType().getName() );
			}
		}
	}

	genericDefs.clear();
	return true;
}


//==============================================================================
bool ContextDefinitionManager::deserializeDefinitions( ISerializer & serializer )
{
	// load generic definitions
	size_t count = 0;
	serializer.deserialize( count );
	for (size_t i = 0; i < count; i++)
	{
		std::string defName;
		serializer.deserialize( defName );

		std::string parentDefName;
		serializer.deserialize( parentDefName );
		auto pDef = getDefinition( defName.c_str() );
		IClassDefinitionModifier * modifier = nullptr;
		if ( !pDef )
		{
			auto definition = registerDefinition( createGenericDefinition( defName.c_str() ) );
			modifier = definition->getDetails().getDefinitionModifier();
		}

		size_t size = 0;
		serializer.deserialize( size );
		std::string propName;
		std::string typeName;
		for (size_t j = 0; j < size; j++)
		{
			propName.clear();
			typeName.clear();
			serializer.deserialize( propName );
			serializer.deserialize( typeName );
			IBasePropertyPtr property = nullptr;
			auto metaType = Variant::findType( typeName.c_str() );
			if (modifier)
			{
				auto property = modifier->addProperty( propName.c_str(), metaType != nullptr ? metaType->typeId().getName() : typeName.c_str(), nullptr );
				//assert( property );
			}
		}
	}
	return true;
}


//==============================================================================
std::unique_ptr<IClassDefinitionDetails> ContextDefinitionManager::createGenericDefinition( const char * name ) const
{
	assert( pBaseManager_ );
	return pBaseManager_->createGenericDefinition( name );
}
} // end namespace wgt
