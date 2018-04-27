#include "pch.hpp"
#include "test_data_model_fixture.hpp"

namespace wgt
{
//==============================================================================
TestDataModelFixture::TestDataModelFixture()
{
}

//==============================================================================
TestDataModelFixture::~TestDataModelFixture()
{
}

//==============================================================================
IObjectManager* TestDataModelFixture::getObjectManager() const
{
    return &framework_.getObjectManager();
}

//==============================================================================
IDefinitionManager* TestDataModelFixture::getDefinitionManager() const
{
    return &framework_.getDefinitionManager();
}
} // end namespace wgt
