#pragma once

#include "core_common/assert.hpp"
#include "core_unit_test/test_application.hpp"
#include "core_unit_test/test_object_manager.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_command_system/command_manager.hpp"
#include "core_environment_system/env_system.hpp"
#include "core_serialization/serializer/serialization_manager.hpp"
#include "core_reflection_utils/reflection_controller.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/reflection/reflected_list.hpp"
#include "core_unit_test/test_framework.hpp"
#include "core_dependency_system/depends.hpp"

#include "test_reflection_fixture.hpp"

namespace wgt
{
class TestObjectHandleFixture: public TestReflectionFixture
{
public:
	TestApplication application_;
	EnvManager envManager_;
	CommandManager commandManager;
	ReflectionController reflectionController;
	SetReflectedPropertyCommand setReflectedPropertyCmd;

	TestObjectHandleFixture()
	    : commandManager(envManager_)
        , setReflectedPropertyCmd(getDefinitionManager())
	{
		commandManager.init(application_, getDefinitionManager() );
		commandManager.registerCommand(&setReflectedPropertyCmd);
		reflectionController.init(commandManager);
	}

	~TestObjectHandleFixture()
	{
		commandManager.fini();
	}
};

class ITestCast : virtual Depends<IDefinitionManager>
{
	DECLARE_REFLECTED
public:
	virtual int value() = 0;
};

class TestCast : public ITestCast
			   , virtual Depends<IDefinitionManager>
{
	DECLARE_REFLECTED
public:
	virtual int value() override { return 1; };
};


class Test1
{
	DECLARE_REFLECTED
public:
	Test1() : value_(0)
	{
	}
	Test1(int v) : value_(v)
	{
	}

	int value_;
};

class Test1Stack
{
	DECLARE_REFLECTED
public:
	Test1Stack() : value_(0)
	{
	}
	Test1Stack(int v) : value_(v)
	{
	}

private:
	int value_;
};

class Test2Stack
{
	DECLARE_REFLECTED
public:
	Test2Stack() : value_(0)
	{
	}
	Test2Stack(int v) : value_(v), test1_(v)
	{
	}

private:
	int value_;
	Test1 test1_;
};

class GListTest
{
public:
	GListTest(IDefinitionManager* defManager) : gl_(defManager)
	{
	}
	GListTest(const GListTest&) : gl_(nullptr)
	{
		TF_ASSERT(false);
	}

	void addItem(ManagedObjectPtr t)
	{
		Collection& collection = gl_.getSource();
		storage_.push_back(std::move(t));
		collection.insertValue(collection.size(), storage_.back()->getHandle());
	}

	const AbstractListModel* getList() const
	{
		return &gl_;
	}

	PropertyAccessor bindProperty(size_t index, IClassDefinition* def, const char* name)
	{
		Collection& collection = gl_.getSource();
		return def->bindProperty(name, collection[index].cast<ObjectHandle>());
	}

private:
	ReflectedList gl_;
	std::vector<ManagedObjectPtr> storage_;
};

} // end namespace wgt
