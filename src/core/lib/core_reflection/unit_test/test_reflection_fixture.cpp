#include "pch.hpp"
#include "test_reflection_fixture.hpp"
#include "core_unit_test/test_framework.hpp"

namespace wgt
{
//==============================================================================
TestReflectionFixture::TestReflectionFixture()
    : framework_(new TestFramework())
{
}

//==============================================================================
TestReflectionFixture::~TestReflectionFixture()
{
}

//==============================================================================
IObjectManager& TestReflectionFixture::getObjectManager() const
{
	return framework_->getObjectManager();
}

//==============================================================================
IDefinitionManager& TestReflectionFixture::getDefinitionManager() const
{
	return framework_->getDefinitionManager();
}

//==============================================================================
TestStructure& TestReflectionFixture::getTestStructure()
{
	return testObjects_.getTestStructure();
}
} // end namespace wgt
