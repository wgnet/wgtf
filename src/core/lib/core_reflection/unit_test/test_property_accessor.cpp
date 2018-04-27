#include "pch.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_unit_test/unit_test.hpp"

#include "test_reflection_fixture.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
TEST_F(TestReflectionFixture, testBinding)
{
	TestStructure& testStructure = getTestStructure();

	{
		ManagedObject<TestStructure> object(&testStructure);
		ObjectHandleT<TestStructure> handle = object.getHandleT();

		auto definition = getDefinitionManager().getDefinition(handle);
		CHECK(definition);
		auto itRange = definition->allProperties();
		for (auto it = itRange.begin(); it != itRange.end(); ++it)
		{
			auto property = *it;
			definition->bindProperty(property->getName(), handle);
		}
	}

	{
		ManagedObject<TestStructure> object(testStructure);
		ObjectHandleT<TestStructure> handle = object.getHandleT();

		auto definition = getDefinitionManager().getDefinition(handle);
		CHECK(definition);
		auto itRange = definition->allProperties();

		for (auto it = itRange.begin(); it != itRange.end(); ++it)
		{
			auto property = *it;
			definition->bindProperty(property->getName(), handle);
		}
	}
}
} // end namespace wgt
