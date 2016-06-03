#include "pch.hpp"
#include "test_data_model_fixture.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"

namespace wgt
{
//==============================================================================
TestDataModelFixture::TestDataModelFixture()
	: objectManager_( new ObjectManager() )
	, definitionManager_( new DefinitionManager( *objectManager_ ) )
{
	objectManager_->init( definitionManager_.get() );
	Reflection::initReflectedTypes( *definitionManager_ );
}


//==============================================================================
TestDataModelFixture::~TestDataModelFixture()
{
	objectManager_.reset(); 
	definitionManager_.reset();
}


//==============================================================================
IObjectManager * TestDataModelFixture::getObjectManager() const
{
	return objectManager_.get();
}


//==============================================================================
IDefinitionManager * TestDataModelFixture::getDefinitionManager() const
{
	return definitionManager_.get();
}
} // end namespace wgt
