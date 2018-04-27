#include "core_generic_plugin/generic_plugin.hpp"

#include "core_logging/logging.hpp"

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include "core_data_model/reflection/reflected_list.hpp"
#include "core_data_model/reflection_proto/reflected_tree_model.hpp"

#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include <vector>

namespace wgt
{
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

private:
	int value_;
};

BEGIN_EXPOSE(Test1, MetaNone())
EXPOSE("Value", value_, MetaNone())
END_EXPOSE()

class Test2
{
	DECLARE_REFLECTED
public:
	Test2() : value_(0)
	{
	}
	Test2(int v) : value_(v), test1_(v)
	{
	}

private:
	int value_;
	Test1 test1_;
};

BEGIN_EXPOSE(Test2, MetaNone())
EXPOSE("Value", value_, MetaNone())
EXPOSE("Test1", test1_, MetaNone())
END_EXPOSE()

class Test3
{
	DECLARE_REFLECTED
public:
	Test3() : value_(0)
	{
	}
	Test3(int v) : value_(v), test2_(v), vector_(v, 1)
	{
	}

private:
	int value_;
	Test2 test2_;
	std::vector<Test1> vector_;
};

BEGIN_EXPOSE(Test3, MetaNone())
EXPOSE("Value", value_, MetaNone())
EXPOSE("Test2", test2_, MetaNone())
EXPOSE("TestVec", vector_, MetaNone())
END_EXPOSE()

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

BEGIN_EXPOSE(Test1Stack, MetaOnStack())
EXPOSE("Value", value_, MetaNone())
END_EXPOSE()

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

BEGIN_EXPOSE(Test2Stack, MetaOnStack())
EXPOSE("Value", value_, MetaNone())
EXPOSE("Test1", test1_, MetaNone())
END_EXPOSE()

class GListTest
{
public:
	GListTest(IDefinitionManager* defManager) : gl_(defManager)
	{
	}
	GListTest(const GListTest&) : gl_(nullptr)
	{
		assert(false);
	}

	template <typename T>
	void addItem(T&& t)
	{
		Collection& collection = gl_.getSource();
		collection.insertValue(collection.size(), t);
	}

	const AbstractListModel* getList() const
	{
		return &gl_;
	}

private:
	ReflectedList gl_;
};

/**
* A plugin which tests the reflection plugin
*
* @ingroup plugins
* @bug currently broken
* @note Requires Plugins:
*       - @ref coreplugins
*/
class TestObjHandlePlugin : public PluginMain, public Depends<IViewCreator>
{
public:
	bool PostLoad(IComponentContext& contextManager) override
	{
		if (IDefinitionManager* dm = contextManager.queryInterface<IDefinitionManager>())
		{
			def1_ = dm->registerDefinition<TypeClassDefinition<Test1>>();
			def2_ = dm->registerDefinition<TypeClassDefinition<Test2>>();
			def3_ = dm->registerDefinition<TypeClassDefinition<Test3>>();

			dm->registerDefinition<TypeClassDefinition<Test1Stack>>();
			dm->registerDefinition<TypeClassDefinition<Test2Stack>>();
		}
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		auto defManager = contextManager.queryInterface<IDefinitionManager>();
		glist_ = std::unique_ptr<GListTest>(new GListTest(defManager));
		glist_->addItem(Test1Stack(5));
		glist_->addItem(Test2Stack(58));
		glist_->addItem(Test1Stack(7));

		auto viewCreator = get<IViewCreator>();
		if (viewCreator == nullptr)
		{
			return;
		}

		viewGL_ = viewCreator->createView("plg_list_model_test/test_list_panel.qml", glist_->getList());

		test_ = ManagedObject<Test3>(std::unique_ptr<Test3>(new Test3(3)));
		auto model = std::shared_ptr<AbstractTreeModel>(new proto::ReflectedTreeModel(test_.getHandle()));

		viewTest_ = viewCreator->createView("plg_tree_model_test/test_tree_panel.qml", model);
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		if (IUIApplication* app = contextManager.queryInterface<IUIApplication>())
		{
			if (viewGL_.valid())
			{
				auto view = viewGL_.get();
				app->removeView(*view);
				view = nullptr;
			}
			if (viewTest_.valid())
			{
				auto view = viewTest_.get();
				app->removeView(*view);
				view = nullptr;
			}
		}
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
	}

private:
	IClassDefinition* def1_;
	IClassDefinition* def2_;
	IClassDefinition* def3_;

	std::unique_ptr<GListTest> glist_;
	wg_future<std::unique_ptr<IView>> viewGL_;
	ManagedObject<Test3> test_;
	wg_future<std::unique_ptr<IView>> viewTest_;
};

PLG_CALLBACK_FUNC(TestObjHandlePlugin)
} // end namespace wgt
