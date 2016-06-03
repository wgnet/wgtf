#include "pch.hpp"
#include "core_unit_test/unit_test.hpp"

#include "test_reflection_fixture.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/property_accessor.hpp"

namespace wgt
{
TEST_F( TestReflectionFixture, testBinding )
{
	TestStructure & testStructure = getTestStructure();

	ObjectHandle provider(
		&testStructure,
		getDefinitionManager().getDefinition< TestStructure >() );

	auto definition = provider.getDefinition( getDefinitionManager() );
	CHECK( definition );
	auto itRange = definition->allProperties();
	for( auto it = itRange.begin(); it != itRange.end(); ++it )
	{
		auto property = *it;
		definition->bindProperty( property->getName(), provider );
	}

}
} // end namespace wgt
