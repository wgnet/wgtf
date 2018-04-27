#include "pch.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_generic_plugin_manager/unit_test/plugin1_test/plugin_objects.hpp"
#include "core_generic_plugin_manager/unit_test/plugin2_test/plugin_objects.hpp"
#include "core_generic_plugin_test/memory_plugin_context_creator.hpp"
#include "core_generic_plugin_test/test_plugin_loader.hpp"

namespace wgt
{
namespace
{
const wchar_t* s_Plugin1Path = L"plugins/plg_plugin1_test";
const wchar_t* s_Plugin2Path = L"plugins/plg_plugin2_test";
std::unique_ptr<TestPluginLoader> s_pluginLoader;
}

//------------------------------------------------------------------------------
TEST(load_plugin)
{
    TF_ASSERT(!s_pluginLoader);
    s_pluginLoader = std::make_unique<TestPluginLoader>();
    auto& pluginManager = *getPluginManager();

    auto pObjectManager = pluginManager.queryInterface<IObjectManager>();
    CHECK(pObjectManager != nullptr);

    auto defManager = pluginManager.queryInterface<IDefinitionManager>();
    TF_ASSERT(defManager != nullptr);

    auto plugin1 = pluginManager.queryInterface<ITestPlugin1>();
    CHECK(plugin1 == nullptr);

    std::vector<std::wstring> plugins;
    plugins.push_back(s_Plugin1Path);

    pluginManager.loadPlugins(plugins);

    plugin1 = pluginManager.queryInterface<ITestPlugin1>();
    CHECK(plugin1 != nullptr);

    TestPlugin1TestObjectPtr testObj = nullptr;
    if (plugin1 != nullptr)
    {
        testObj = plugin1->getObject();
        CHECK(testObj != nullptr);

        if (testObj != nullptr)
        {
            std::string text("Sample");
            testObj->setText(text);
            CHECK_EQUAL(text, testObj->getText());
        }
    }

    pluginManager.unloadPlugins(plugins);
    CHECK(testObj == nullptr);

    plugin1 = pluginManager.queryInterface<ITestPlugin1>();
    CHECK(plugin1 == nullptr);
}

//------------------------------------------------------------------------------
TEST(reload_plugin)
{
	auto& pluginManager = *getPluginManager();

	auto objManager = pluginManager.queryInterface<IObjectManager>();
	TF_ASSERT(objManager != nullptr);

	std::vector<std::wstring> plugins;
	plugins.push_back(s_Plugin1Path);

	pluginManager.loadPlugins(plugins);

	auto rawPointer = pluginManager.queryInterface<ITestPlugin1>();
	CHECK(rawPointer != nullptr);
	if (rawPointer == nullptr)
	{
		return;
	}
	auto plugin1 = safeCast<TestPlugin1Interface>(objManager->getObject(rawPointer));
	CHECK(plugin1 != nullptr);

	plugins.clear();
	plugins.push_back(s_Plugin1Path);
	pluginManager.unloadPlugins(plugins);
	CHECK(plugin1 == nullptr);

	rawPointer = pluginManager.queryInterface<ITestPlugin1>();
	CHECK(rawPointer == nullptr);
	CHECK(plugin1 == nullptr);

	pluginManager.loadPlugins(plugins);
	CHECK(plugin1 == nullptr);

	rawPointer = pluginManager.queryInterface<ITestPlugin1>();
	CHECK(rawPointer != nullptr);
	if (rawPointer == nullptr)
	{
		return;
	}
	plugin1 = safeCast<TestPlugin1Interface>(objManager->getObject(rawPointer));
	CHECK(plugin1 != nullptr);
	CHECK(plugin1.get<TestPlugin1Interface>() != nullptr);

	pluginManager.unloadPlugins(plugins);
	CHECK(plugin1.get<TestPlugin1Interface>() == nullptr);
	CHECK(plugin1 == nullptr);

	rawPointer = pluginManager.queryInterface<ITestPlugin1>();
	CHECK(rawPointer == nullptr);
	CHECK(plugin1 == nullptr);
}

////------------------------------------------------------------------------------
TEST(serialise_plugin)
{
	auto& pluginManager = *getPluginManager();

	auto objManager = pluginManager.queryInterface<IObjectManager>();
	TF_ASSERT(objManager != nullptr);

	auto defManager = pluginManager.queryInterface<IDefinitionManager>();
	TF_ASSERT(defManager != nullptr);

	std::vector<std::wstring> plugins;
	plugins.push_back(s_Plugin1Path);

	pluginManager.loadPlugins(plugins);

	auto rawPointer = pluginManager.queryInterface<ITestPlugin1>();
	CHECK(rawPointer != nullptr);
	if (rawPointer == nullptr)
	{
		return;
	}
	auto plugin1 = safeCast<TestPlugin1Interface>(objManager->getObject(rawPointer));
	CHECK(plugin1 != nullptr);
    if (plugin1 == nullptr)
    {
        return;
    }

	TestPlugin1TestObjectPtr testObj = plugin1->getObject();
	CHECK(testObj != nullptr);

	if (testObj != nullptr)
	{
		std::string text("Sample");
		testObj->setText(text);
		CHECK_EQUAL(text, testObj->getText());
	}

	plugins.clear();
	plugins.push_back(s_Plugin1Path);
	pluginManager.unloadPlugins(plugins);
	CHECK(plugin1 == nullptr);
	CHECK(testObj == nullptr);
}

//------------------------------------------------------------------------------
TEST(more_plugins)
{
	auto& pluginManager = *getPluginManager();

	auto objManager = pluginManager.queryInterface<IObjectManager>();
	TF_ASSERT(objManager != nullptr);

	auto defManager = pluginManager.queryInterface<IDefinitionManager>();
	TF_ASSERT(defManager != nullptr);

	std::vector<std::wstring> plugins;
	plugins.push_back(s_Plugin1Path);
	plugins.push_back(s_Plugin2Path);
	pluginManager.loadPlugins(plugins);

	auto rawPointer1 = pluginManager.queryInterface<ITestPlugin1>();

	CHECK(rawPointer1 != nullptr);
	if (rawPointer1 == nullptr)
	{
		return;
	}
	auto plugin1 = safeCast<TestPlugin1Interface>(objManager->getObject(rawPointer1));
	CHECK(plugin1 != nullptr);

	auto rawPointer2 = pluginManager.queryInterface<ITestPlugin2>();
	CHECK(rawPointer2 != nullptr);
	if (rawPointer2 == nullptr)
	{
		return;
	}
	auto plugin2 = safeCast<TestPlugin2Interface>(objManager->getObject(rawPointer2));
	CHECK(plugin2 != nullptr);

	TestPlugin2TestObjectPtr testObj = nullptr;
	if (plugin2 != nullptr)
	{
		testObj = plugin2->getObject();
		CHECK(testObj != nullptr);

		if (testObj != nullptr)
		{
			std::string text("from plugin2");
			testObj->setText(text);
			CHECK_EQUAL(text, testObj->getText());
		}
	}

	TestPlugin2TestObjectPtr testObj2 = nullptr;
	if (plugin1 != nullptr)
	{
		testObj2 = plugin1->getObjectFromPlugin2();
		CHECK(testObj2 != nullptr);
		CHECK(*testObj == *testObj2);
	}

	plugins.clear();
	plugins.push_back(s_Plugin1Path);
	pluginManager.unloadPlugins(plugins);
	CHECK(plugin1 == nullptr);
	CHECK(testObj != nullptr);
	CHECK(testObj2 != nullptr);
	CHECK(*testObj == *testObj2);

	pluginManager.loadPlugins(plugins);
	rawPointer1 = pluginManager.queryInterface<ITestPlugin1>();
	CHECK(rawPointer1 != nullptr);
	if (rawPointer1 == nullptr)
	{
		return;
	}
	plugin1 = safeCast<TestPlugin1Interface>(objManager->getObject(rawPointer1));
	CHECK(plugin1 != nullptr);

	if (plugin1 != nullptr)
	{
		testObj2 = plugin1->getObjectFromPlugin2();
		CHECK(testObj2 != nullptr);
		CHECK(*testObj == *testObj2);
	}

	plugins.clear();
	plugins.push_back(s_Plugin2Path);
	plugins.push_back(s_Plugin1Path);
	pluginManager.unloadPlugins(plugins);
	CHECK(plugin2 == nullptr);
	CHECK(testObj == nullptr);
	CHECK(testObj2 == nullptr);
}

//------------------------------------------------------------------------------
TEST(unload_plugin)
{
	TF_ASSERT(s_pluginLoader);
	s_pluginLoader.reset();

	auto& pluginManager = *getPluginManager();

	auto pObjectManager = pluginManager.queryInterface<IObjectManager>();
	CHECK(pObjectManager == nullptr);

	auto defManager = pluginManager.queryInterface<IDefinitionManager>();
	TF_ASSERT(defManager == nullptr);
}

} // end namespace wgt
