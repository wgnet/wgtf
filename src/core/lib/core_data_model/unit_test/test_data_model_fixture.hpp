#ifndef TEST_DATA_MODEL_FIXTURE_HPP
#define TEST_DATA_MODEL_FIXTURE_HPP

#include <memory>
#include "core_unit_test/test_framework.hpp"

namespace wgt
{
class TestDataModelFixture
{
public:
	TestDataModelFixture();
	virtual ~TestDataModelFixture();
	IObjectManager* getObjectManager() const;
	IDefinitionManager* getDefinitionManager() const;

private:
    TestFramework framework_;
};
} // end namespace wgt
#endif // TEST_DATA_MODEL_FIXTURE_HPP
