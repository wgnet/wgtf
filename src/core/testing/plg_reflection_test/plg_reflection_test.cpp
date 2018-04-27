#include "core_generic_plugin/generic_plugin.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_object/managed_object.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/i_file_system.hpp"

#include "test_class.hpp"
#include "metadata/test_class.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

namespace wgt
{
/**
* A plugin which tests the reflection plugin
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class TestPluginReflection : public PluginMain
{
public:
	TestPluginReflection(IComponentContext& contextManager)
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager) override
	{
		IDefinitionManager* pDefinitionManager = contextManager.queryInterface<IDefinitionManager>();
		if (pDefinitionManager == nullptr)
		{
			return;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager);
		// Access members on TestBase
		{
			auto testBase = ManagedObject<TestBase>::make();
			assert(testBase != nullptr);

			auto definition = definitionManager.getDefinition<TestBase>();
			assert(definition != nullptr);

			// BaseName
			{
				PropertyAccessor baseNamePropertyAccessor = definition->bindProperty("Name", testBase.getHandleT());
				assert(baseNamePropertyAccessor.isValid());
				Variant value = baseNamePropertyAccessor.getValue();
				std::string testBaseName;
				bool ok = value.tryCast(testBaseName);
				assert(ok);
				assert(strcmp(testBaseName.c_str(), "TestBaseName") == 0);
			}
		}

		// Access members on TestClass
		{
			auto testClass = ManagedObject<TestClass>::make();
            auto testClassHandle = testClass.getHandleT();
			assert(testClass != nullptr);

			auto definition = definitionManager.getDefinition<TestClass>();
			assert(definition != nullptr);

			// Name overridden from base class
			{
				PropertyAccessor namePropertyAccessor = definition->bindProperty("Name", testClassHandle);
				assert(namePropertyAccessor.isValid());
				std::string testClassName;
				Variant value = namePropertyAccessor.getValue();
				bool ok = value.tryCast(testClassName);
				assert(ok);
				assert(strcmp(testClassName.c_str(), "TestClassName") == 0);
			}

			// String
			{
				PropertyAccessor stringPropertyAccessor = definition->bindProperty("String", testClassHandle);
				assert(stringPropertyAccessor.isValid());
				assert(!stringPropertyAccessor.getProperty()->isCollection());
				std::string testClassString;
				Variant value = stringPropertyAccessor.getValue();
				bool ok = value.tryCast(testClassString);
				assert(ok);
				assert(testClassString == "TestClassString");

				stringPropertyAccessor.setValue("Test1");
				value = stringPropertyAccessor.getValue();
				ok = value.tryCast(testClassString);
				assert(ok);
				assert(testClassString == "Test1");
			}

			// Strings
			{
				PropertyAccessor stringPropertyAccessor = definition->bindProperty("Strings", testClassHandle);
				assert(stringPropertyAccessor.isValid());
				assert(stringPropertyAccessor.getProperty()->isCollection());
				std::vector<std::string> testClassStrings;
				Variant value = stringPropertyAccessor.getValue();
				bool ok = value.tryCast(testClassStrings);
				assert(ok);
				std::vector<std::string> test1;
				test1.push_back("TestClassString1");
				test1.push_back("TestClassString2");
				assert(testClassStrings == test1);

				std::vector<std::string> test2;
				test2.push_back("TestClassString3");
				test2.push_back("TestClassString4");
				stringPropertyAccessor.setValue(test2);
				value = stringPropertyAccessor.getValue();
				ok = value.tryCast(testClassStrings);
				assert(ok);
				assert(testClassStrings == test2);
			}

			// String accessors
			{
				PropertyAccessor stringPropertyAccessor = definition->bindProperty("StringFunc", testClassHandle);
				assert(stringPropertyAccessor.isValid());
				std::string testClassString;
				Variant value = stringPropertyAccessor.getValue();
				bool ok = value.tryCast(testClassString);
				assert(ok);
				assert(testClassString == "Test1");

				stringPropertyAccessor.setValue("Test2");
				value = stringPropertyAccessor.getValue();
				ok = value.tryCast(testClassString);
				assert(ok);
				assert(testClassString == "Test2");
			}

			// String lambda accessors
			{
				PropertyAccessor stringPropertyAccessor = definition->bindProperty("StringLambda", testClassHandle);
				assert(stringPropertyAccessor.isValid());
				assert(!stringPropertyAccessor.getProperty()->isCollection());
				Variant value = stringPropertyAccessor.getValue();
				std::string testClassString;
				bool ok = value.tryCast(testClassString);
				assert(ok);
				assert(testClassString == "Test2");

				stringPropertyAccessor.setValue("TestClassString");
				value = stringPropertyAccessor.getValue();
				ok = value.tryCast(testClassString);
				assert(ok);
				assert(testClassString == "TestClassString");
			}

			// Strings lambda accessors
			{
				PropertyAccessor stringPropertyAccessor = definition->bindProperty("StringsLambda", testClassHandle);
				assert(stringPropertyAccessor.isValid());
				assert(stringPropertyAccessor.getProperty()->isCollection());
				std::vector<std::string> testClassStrings;
				Variant value = stringPropertyAccessor.getValue();
				bool ok = value.tryCast(testClassStrings);
				assert(ok);
				std::vector<std::string> test1;
				test1.push_back("TestClassString3");
				test1.push_back("TestClassString4");
				assert(testClassStrings == test1);

				std::vector<std::string> test2;
				test1.push_back("TestClassString5");
				test1.push_back("TestClassString6");
				stringPropertyAccessor.setValue(test2);
				value = stringPropertyAccessor.getValue();
				ok = value.tryCast(testClassStrings);
				assert(ok);
				assert(testClassStrings == test2);
			}

			// InnerClass
			{
				PropertyAccessor innerPropertyAccessor = definition->bindProperty("InnerClass", testClassHandle);
				assert(innerPropertyAccessor.isValid());

				// InnerClass properties
				// From testClass property accessor
				{
					PropertyAccessor innerNamePropertyAccessor =
					definition->bindProperty("InnerClass.InnerName", testClassHandle);

					assert(innerNamePropertyAccessor.isValid());
					std::string innerClassName;
					Variant value = innerNamePropertyAccessor.getValue();
					bool ok = value.tryCast(innerClassName);
					assert(ok);
					assert(strcmp(innerClassName.c_str(), "TestClassInnerName") == 0);
				}

				// InnerClass properties
				// From innerClass property accessor
				{
					auto value = innerPropertyAccessor.getValue();
					ObjectHandle handle = value.cast<ObjectHandle>();
					auto pInnerClass = handle.getBase<TestClass::InnerClass>();
					assert(pInnerClass != nullptr);

					const auto pStructDefinition = innerPropertyAccessor.getStructDefinition();
					assert(pStructDefinition != nullptr);
                    const auto& structDefinition = (*pStructDefinition);
                    PropertyAccessor innerNamePropertyAccessor =
                        structDefinition.bindProperty("InnerName", handle);
                    assert(innerNamePropertyAccessor.isValid());

                    std::string innerClassName;
                    value = innerNamePropertyAccessor.getValue();
                    bool ok = value.tryCast(innerClassName);
                    assert(ok);
                    assert(strcmp(innerClassName.c_str(), "TestClassInnerName") == 0);
				}

				// Serialization
				{
					const char* testFileName = "test_definition_serialization_plg.xml";
					const char* genericDefinitionName = "GenericDefinitionTest";

					{
						auto definition = definitionManager.registerDefinition(
						definitionManager.createGenericDefinition(genericDefinitionName));
						auto modifier = definition->getDetails().getDefinitionModifier();

						modifier->addProperty("GenericStringTest", "string", nullptr, false);
						modifier->addProperty("GenericStringsTest", "vectorStrings", nullptr, true);

						ResizingMemoryStream stream;
						XMLSerializer serializer(stream, definitionManager);
						definitionManager.serializeDefinitions(serializer);

						auto fileSystem = contextManager.queryInterface<IFileSystem>();
						fileSystem->writeFile(testFileName, stream.buffer().c_str(), stream.buffer().size(),
						                      std::ios::out | std::ios::binary);

						definitionManager.deregisterDefinition(definition);
						auto klass = definitionManager.getDefinition(genericDefinitionName);
						assert(!klass);
					}

					{
						auto fileSystem = contextManager.queryInterface<IFileSystem>();
						IFileSystem::IStreamPtr fileStream =
						fileSystem->readFile(testFileName, std::ios::in | std::ios::binary);
						XMLSerializer serializer(*fileStream, definitionManager);
						definitionManager.deserializeDefinitions(serializer);

						auto klass = definitionManager.getDefinition(genericDefinitionName);
						IBasePropertyPtr property = klass->findProperty("GenericStringTest");
						assert(property);
						assert(!property->isCollection());

						property = klass->findProperty("GenericStringsTest");
						assert(property);
						assert(property->isCollection());

						property = klass->findProperty("NoSuchProperty");
						assert(!property);
					}
				}
			}
		}
	}
};

PLG_CALLBACK_FUNC(TestPluginReflection)
} // end namespace wgt
