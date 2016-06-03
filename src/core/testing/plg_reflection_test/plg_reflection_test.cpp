#include "core_generic_plugin/generic_plugin.hpp"

#include "core_reflection/base_property.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflection_macros.hpp"

#include "test_class.hpp"

namespace wgt
{
/**
 *	Test reflection system.
 */
class TestPluginReflection
	: public PluginMain
{
private:
	std::unique_ptr< MetaTypeImpl< ObjectHandle > > baseProviderMetaType_;
public:
	TestPluginReflection( IComponentContext & contextManager )
		: baseProviderMetaType_( new MetaTypeImpl< ObjectHandle >() )
	{ 
	}


	//==========================================================================
	void Initialise( IComponentContext & contextManager ) override
	{
		auto metaTypeManager =
			contextManager.queryInterface< IMetaTypeManager >();
		if (metaTypeManager)
		{
			metaTypeManager->registerType( baseProviderMetaType_.get() );
		}
		Variant::setMetaTypeManager( metaTypeManager );

		IDefinitionManager* pDefinitionManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			return;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager);
		REGISTER_DEFINITION( TestBase );

		// Inner definition must be registered before outer definition
		REGISTER_DEFINITION( TestClass::InnerClass );
		REGISTER_DEFINITION( TestClass );

		// Access members on TestBase
		{
			auto testBase = definitionManager.create< TestBase >();
			assert( testBase.get() != nullptr );

			auto definition = definitionManager.getDefinition< TestBase >();
			assert( definition != nullptr );

			// BaseName
			{
				PropertyAccessor baseNamePropertyAccessor =
					definition->bindProperty( "Name", testBase );
				assert( baseNamePropertyAccessor.isValid() );
				Variant value = baseNamePropertyAccessor.getValue();
				std::string testBaseName;
				bool ok = false;
				ok = value.tryCast( testBaseName );
				assert( ok );
				assert( strcmp( testBaseName.c_str(), "TestBaseName" ) == 0 );
			}
		}

		// Access members on TestClass
		{
			auto testClass = definitionManager.create< TestClass >();
			assert( testClass.get() != nullptr );

			auto definition = definitionManager.getDefinition< TestClass >();
			assert( definition != nullptr );

			// Name overridden from base class
			{
				PropertyAccessor namePropertyAccessor =
					definition->bindProperty( "Name", testClass );
				assert( namePropertyAccessor.isValid() );
				std::string testClassName;
				Variant value = namePropertyAccessor.getValue();
				bool ok = value.tryCast( testClassName );
				assert( ok );
				assert( strcmp( testClassName.c_str(), "TestClassName" ) == 0 );
			}

			// String
			{
				PropertyAccessor stringPropertyAccessor =
					definition->bindProperty( "String", testClass );
				assert( stringPropertyAccessor.isValid() );
				std::string testClassString;
				Variant value = stringPropertyAccessor.getValue();
				bool ok = value.tryCast( testClassString );
				assert( ok );
				assert( testClassString == "TestClassString" );
			}

			// String accessors
			{
				PropertyAccessor namePropertyAccessor =
					definition->bindProperty( "StringFunc", testClass );
				assert( namePropertyAccessor.isValid() );
				std::string testClassString;
				Variant value = namePropertyAccessor.getValue();
				bool ok = value.tryCast( testClassString );
				assert( ok );
				assert( testClassString == "TestClassString" );
			}

			// InnerClass
			{
				PropertyAccessor innerPropertyAccessor =
					definition->bindProperty( "InnerClass", testClass );
				assert( innerPropertyAccessor.isValid() );

				// InnerClass properties
				// From testClass property accessor
				{
					PropertyAccessor innerNamePropertyAccessor =
						definition->bindProperty(
							"InnerClass.InnerName",
							testClass );

					assert( innerNamePropertyAccessor.isValid() );
					std::string innerClassName;
					Variant value = innerNamePropertyAccessor.getValue();
					bool ok = value.tryCast( innerClassName );
					assert( ok );
					assert( strcmp( innerClassName.c_str(),
						"TestClassInnerName" ) == 0 );
				}

				// InnerClass properties
				// From innerClass property accessor
				{
					auto value = innerPropertyAccessor.getValue();
					ObjectHandle baseProvider;
					value.tryCast( baseProvider );
					auto pInnerClass
						= baseProvider.getBase< TestClass::InnerClass >();
					assert( pInnerClass != nullptr );

					const auto pStructDefinition =
						innerPropertyAccessor.getStructDefinition();
					assert( pStructDefinition != nullptr );

					const auto & structDefinition =
						(*pStructDefinition);
					PropertyAccessor innerNamePropertyAccessor =
						structDefinition.bindProperty(
							"InnerName",
							baseProvider );
					assert( innerNamePropertyAccessor.isValid() );

					std::string innerClassName;
					value = innerNamePropertyAccessor.getValue();
					bool ok = value.tryCast( innerClassName );
					assert( ok );
					assert( strcmp( innerClassName.c_str(),
						"TestClassInnerName" ) == 0 );
				}
			}
		}
	}
};

PLG_CALLBACK_FUNC( TestPluginReflection )
} // end namespace wgt
