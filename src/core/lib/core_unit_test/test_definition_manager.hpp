#pragma once

#include "core_unit_test/test_global_context.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"

namespace wgt
{
class TestDefinitionManager : public DefinitionManager
{
public:
	TestDefinitionManager(IObjectManager& objectManager)
		: DefinitionManager(objectManager)
	{
		interface_ = registerInterface<IDefinitionManager>(this);
	}

	virtual ~TestDefinitionManager()
	{
		deregisterInterface(interface_.get());
	}

private:
	InterfacePtr interface_ = nullptr;
};
}