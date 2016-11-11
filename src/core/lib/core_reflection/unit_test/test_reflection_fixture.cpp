#include "pch.hpp"
#include "test_reflection_fixture.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"

namespace wgt
{
//==============================================================================
TestReflectionFixture::TestReflectionFixture()
    : objectManager_(new ObjectManager()), definitionManager_(new DefinitionManager(*objectManager_))
{
	objectManager_->init(definitionManager_.get());
	Reflection::initReflectedTypes(*definitionManager_);

	testObjects_.initDefs(*definitionManager_);
}

//==============================================================================
TestReflectionFixture::~TestReflectionFixture()
{
	testObjects_.finiDefs(*definitionManager_);
	objectManager_.reset();
	definitionManager_.reset();
}

//==============================================================================
IObjectManager& TestReflectionFixture::getObjectManager() const
{
	return *objectManager_;
}

//==============================================================================
IDefinitionManager& TestReflectionFixture::getDefinitionManager() const
{
	return *definitionManager_;
}

//==============================================================================
TestStructure& TestReflectionFixture::getTestStructure()
{
	return testObjects_.getTestStructure();
}
} // end namespace wgt
