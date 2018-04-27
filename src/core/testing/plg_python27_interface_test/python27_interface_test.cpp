#include "pch.hpp"
#include "python27_interface_test.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
IComponentContext* g_contextManager = nullptr;

class TestListener : public PropertyAccessorListener
{
public:
	TestListener();
	virtual void preSetValue(const PropertyAccessor& accessor, const Variant& value) override;
	virtual void postSetValue(const PropertyAccessor& accessor, const Variant& value) override;

	bool preSetValueCalled_;
	PropertyAccessor preSetAccessor_;
	Variant preSetValue_;

	bool postSetValueCalled_;
	PropertyAccessor postSetAccessor_;
	Variant postSetValue_;
};

TestListener::TestListener()
    : preSetValueCalled_(false), preSetAccessor_(), preSetValue_(), postSetValueCalled_(false), postSetAccessor_(),
      postSetValue_()
{
}

void TestListener::preSetValue(const PropertyAccessor& accessor, const Variant& value) /* override */
{
	preSetValueCalled_ = true;
	preSetAccessor_ = accessor;
	preSetValue_ = value;
}

void TestListener::postSetValue(const PropertyAccessor& accessor, const Variant& value) /* override */
{
	postSetValueCalled_ = true;
	postSetAccessor_ = accessor;
	postSetValue_ = value;
}

TEST(Python27Interface)
{
	CHECK(g_contextManager != nullptr);
	if (g_contextManager == nullptr)
	{
		return;
	}
	IComponentContext& contextManager(*g_contextManager);

	auto scriptingEngine = contextManager.queryInterface<IPythonScriptingEngine>();
	CHECK(scriptingEngine != nullptr);
	if (scriptingEngine == nullptr)
	{
		return;
	}

	// Import a builtin module
	{
		ObjectHandle module = scriptingEngine->import("sys");
		CHECK(module.isValid());
	}

	// Import a builtin module that uses py files
	{
		ObjectHandle module = scriptingEngine->import("os");
		CHECK(module.isValid());
	}

	// Import a builtin module which will then import a submodule
	// encodings.aliases
	{
		ObjectHandle module = scriptingEngine->import("encodings");
		CHECK(module.isValid());
	}

	// Import a builtin module that uses pyd files
	{
		ObjectHandle module = scriptingEngine->import("md5");
		CHECK(module.isValid());
	}

	{
		// Import a test module
		const wchar_t* sourcePath = L"../../../src/core/testing/plg_python27_interface_test/resources/Scripts";
		const wchar_t* deployPath = L":/Scripts";
		const char* moduleName = "python27_test";
		const bool sourcePathSet = scriptingEngine->appendSourcePath(sourcePath);
		CHECK(sourcePathSet);
		const bool deployPathSet = scriptingEngine->appendBinPath(deployPath);
		CHECK(deployPathSet);
		auto module = scriptingEngine->import(moduleName);
		CHECK(module.isValid());
		if (!module.isValid())
		{
			return;
		}

		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		CHECK(definitionManager != nullptr);
		if (definitionManager == nullptr)
		{
			return;
		}

		// Test property listeners
		auto testListener = std::make_shared<TestListener>();
		auto paListener = std::static_pointer_cast<PropertyAccessorListener>(testListener);
		definitionManager->registerPropertyAccessorListener(paListener);

		// Listen to object
		auto moduleDefinition = definitionManager->getDefinition(module);
		auto testDataAccessor = moduleDefinition->bindProperty("testData", module);

		// Set value, notify listeners
		const auto getResult = testDataAccessor.getValue();
		int expectedOldValue = 0;
		const auto getSuccess = getResult.tryCast<int>(expectedOldValue);
		CHECK(getSuccess);

		const int expectedNewValue = 2;
		const auto setResult = testDataAccessor.setValue(expectedNewValue);
		const auto setSuccess = setResult && !scriptingEngine->checkErrors();
		CHECK(setSuccess);

		// Check listeners were notified
		{
			CHECK(testListener->preSetValueCalled_);
			CHECK(testDataAccessor.getType() == testListener->preSetAccessor_.getType());
			CHECK(strcmp(testDataAccessor.getName(), testListener->preSetAccessor_.getName()) == 0);
			const char* expectedName = "testData";
			CHECK(strcmp(expectedName, testListener->preSetAccessor_.getName()) == 0);
			CHECK(testDataAccessor.getObject() == testListener->preSetAccessor_.getObject());
			int preSetValue = 0;
			const auto preSetValueSuccess = testListener->preSetValue_.tryCast<int>(preSetValue);
			CHECK(preSetValueSuccess);
			CHECK_EQUAL(expectedNewValue, preSetValue);
		}

		{
			CHECK(testListener->postSetValueCalled_);
			CHECK(testDataAccessor.getType() == testListener->postSetAccessor_.getType());
			CHECK(strcmp(testDataAccessor.getName(), testListener->postSetAccessor_.getName()) == 0);
			const char* expectedName = "testData";
			CHECK(strcmp(expectedName, testListener->postSetAccessor_.getName()) == 0);
			CHECK(testDataAccessor.getObject() == testListener->postSetAccessor_.getObject());
			int postSetValue = 0;
			const auto postSetValueSuccess = testListener->postSetValue_.tryCast<int>(postSetValue);
			CHECK(postSetValueSuccess);
			CHECK_EQUAL(expectedNewValue, postSetValue);
		}

		definitionManager->deregisterPropertyAccessorListener(paListener);
	}

	return;
}
} // end namespace wgt
