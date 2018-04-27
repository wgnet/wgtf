#ifndef TEST_REFLECTION_FIXTURE_HPP
#define TEST_REFLECTION_FIXTURE_HPP

#include "testing/reflection_objects_test/test_objects.hpp"

namespace wgt
{
class IObjectManager;
class IDefinitionManager;
class TestFramework;

class TestReflectionFixture
{
public:
	TestReflectionFixture();
	~TestReflectionFixture();
	IObjectManager& getObjectManager() const;
	IDefinitionManager& getDefinitionManager() const;
	TestStructure& getTestStructure();

private:
	std::unique_ptr<TestFramework> framework_;
    TestObjects testObjects_;
};
} // end namespace wgt
#endif // TEST_REFLECTION_FIXTURE_HPP
