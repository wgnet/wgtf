#pragma once

#include <memory>
#include "test_definition_manager.hpp"
#include "test_object_manager.hpp"
#include "test_meta_type_creator.hpp"
#include "test_global_context.hpp"

namespace wgt
{
class IDefinitionManager;
class IObjectManager;

class TestFramework
{
public:
	TestFramework();
	~TestFramework();

	IObjectManager& getObjectManager() const;
	IDefinitionManager& getDefinitionManager() const;

private:
	struct Impl;
	std::unique_ptr< Impl > impl_;
};

}