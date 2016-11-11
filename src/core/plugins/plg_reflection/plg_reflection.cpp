#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"

#include "reflection_component_provider.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "context_definition_manager.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_reflection_utils/reflection_controller.hpp"
#include "core_reflection_utils/serializer/reflection_serializer.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_string_utils/string_utils.hpp"

namespace wgt
{
//==============================================================================
class ReflectionSystemContextManager : public Implements<IComponentContextCreator>
{
public:
	//==========================================================================
	const char* getType() const
	{
		return typeid(ContextDefinitionManager).name();
	}

	IInterface* createContext(const wchar_t* contextId);
};

//==============================================================================
class ReflectionSystemHolder
{
public:
	//==========================================================================
	ReflectionSystemHolder()
	    : objectManager_(new ObjectManager()), definitionManager_(new DefinitionManager(*objectManager_)),
	      contextManager_(new ReflectionSystemContextManager), controller_(new ReflectionController)
	{
		objectManager_->init(definitionManager_.get());
		s_definitionManager_ = definitionManager_.get();
	}

	//==========================================================================
	~ReflectionSystemHolder()
	{
		objectManager_.reset();
		s_definitionManager_ = nullptr;
		definitionManager_.reset();
		contextManager_.reset();
		controller_.reset();
	}

	//==========================================================================
	static IDefinitionManager* getGlobalDefinitionManager()
	{
		return s_definitionManager_;
	}

	//==========================================================================
	DefinitionManager* getDefinitionManager()
	{
		return definitionManager_.get();
	}

	//==========================================================================
	ObjectManager* getObjectManager()
	{
		return objectManager_.get();
	}

	//==========================================================================
	ReflectionSystemContextManager* getContextManager()
	{
		return contextManager_.get();
	}

	//==========================================================================
	ReflectionController* getController()
	{
		return controller_.get();
	}

private:
	static IDefinitionManager* s_definitionManager_;
	std::unique_ptr<ObjectManager> objectManager_;
	std::unique_ptr<DefinitionManager> definitionManager_;
	std::unique_ptr<ReflectionSystemContextManager> contextManager_;
	std::unique_ptr<ReflectionController> controller_;
};

IDefinitionManager* ReflectionSystemHolder::s_definitionManager_ = nullptr;

/**
 * Usage: {,,plg_reflection.dll} Reflection::inspect( <Address_to ObjectHandle > )
 *		  {,,plg_reflection_d.dll} Reflection::inspectVariant( <Address_to Variant > )
 */
namespace Reflection
{
static std::pair<std::string, std::string> inspectVariant(const Variant* variant = nullptr);

static std::map<std::string, std::pair<std::string, std::string>> inspect(const ObjectHandle* handle = nullptr)
{
	std::map<std::string, std::pair<std::string, std::string>> debugData;
	if (handle == nullptr)
	{
		debugData.insert(std::make_pair("Empty handle", std::make_pair("empty", "empty")));
		return debugData;
	}
	auto defManager = Context::queryInterface<IDefinitionManager>();
	if (defManager == nullptr)
	{
		debugData.insert(std::make_pair("Empty handle", std::make_pair("empty", "empty")));
		return debugData;
	}
	auto definition = handle->getDefinition(*defManager);
	if (definition == nullptr)
	{
		debugData.insert(std::make_pair("ObjectHandleT", std::make_pair(handle->type().getName(), "")));
		return debugData;
	}
	auto range = definition->allProperties();
	for (auto const& prop : range)
	{
		Variant variant = prop->get(*handle, *defManager);
		debugData.insert(std::make_pair(prop->getName(), inspectVariant(&variant)));
	}
	return debugData;
}

template <typename T>
bool outputTypeData(const Variant& variant, std::pair<std::string, std::string>& o_Data)
{
	if (variant.typeIs<T>() == false)
	{
		return false;
	}

	T value;
	variant.tryCast(value);
	o_Data = std::make_pair(typeid(T).name(), std::to_string(value));
	return true;
}

template <>
bool outputTypeData<std::string>(const Variant& variant, std::pair<std::string, std::string>& o_Data)
{
	if (variant.typeIs<std::string>())
	{
		std::string value;
		variant.tryCast<std::string>(value);
		o_Data = std::make_pair(typeid(std::string).name(), value);
		return true;
	}
	return false;
}

template <>
bool outputTypeData<std::wstring>(const Variant& variant, std::pair<std::string, std::string>& o_Data)
{
	if (variant.typeIs<std::wstring>())
	{
		std::wstring_convert<Utf16to8Facet> conversion(Utf16to8Facet::create());

		std::wstring wString;
		variant.tryCast<std::wstring>(wString);
		auto output = conversion.to_bytes(wString.c_str());
		o_Data = std::make_pair(typeid(std::wstring).name(), output);
		return true;
	}
	return false;
}

std::pair<std::string, std::string> inspectVariant(const Variant* variant)
{
	std::pair<std::string, std::string> debugData;
	if (variant == nullptr)
	{
		return std::make_pair("Empty variant", "Empty variant");
	}
	ObjectHandle childObj;
	if (variant->tryCast<ObjectHandle>(childObj))
	{
		debugData = std::make_pair("ObjectHandleT", std::to_string((size_t)childObj.data()));
		return debugData;
	}
#define OUTPUT_TYPE_DATA(type)                     \
	if (outputTypeData<type>(*variant, debugData)) \
	{                                              \
		return debugData;                          \
	}

	OUTPUT_TYPE_DATA(float)
	OUTPUT_TYPE_DATA(double)
	OUTPUT_TYPE_DATA(uint64_t)
	OUTPUT_TYPE_DATA(int64_t)
	OUTPUT_TYPE_DATA(std::string)
	OUTPUT_TYPE_DATA(std::wstring)
	return debugData;
}
};

//==========================================================================
IInterface* ReflectionSystemContextManager::createContext(const wchar_t* contextId)
{
	auto contextManager = new ContextDefinitionManager(contextId);
	contextManager->init(ReflectionSystemHolder::getGlobalDefinitionManager());
	return new InterfaceHolder<ContextDefinitionManager>(contextManager, true);
}

/**
* A plugin which registers an IDefinitionManager interface to allow reflection
* on class members for easy and unified serialization of data and exposing to UI code
*
* @ingroup plugins
* @ingroup coreplugins
*/
class ReflectionPlugin : public PluginMain
{
private:
	std::vector<IInterface*> types_;
	std::unique_ptr<ReflectionComponentProvider> reflectionComponentProvider_;
	std::unique_ptr<ReflectionSystemHolder> reflectionSystemHolder_;

public:
	//==========================================================================
	ReflectionPlugin(IComponentContext& contextManager) : reflectionSystemHolder_(new ReflectionSystemHolder)
	{
		// Force linkage
		Reflection::inspect();
		Reflection::inspectVariant();

		types_.push_back(contextManager.registerInterface(reflectionSystemHolder_->getObjectManager(), false));
		types_.push_back(contextManager.registerInterface(reflectionSystemHolder_->getDefinitionManager(), false));
		types_.push_back(contextManager.registerInterface(reflectionSystemHolder_->getContextManager(), false));
		types_.push_back(contextManager.registerInterface(reflectionSystemHolder_->getController(), false));
		Reflection::initReflectedTypes(*reflectionSystemHolder_->getDefinitionManager());
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager) override
	{
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		if (uiFramework)
		{
			reflectionComponentProvider_.reset(
			new ReflectionComponentProvider(*reflectionSystemHolder_->getDefinitionManager()));
			uiFramework->registerComponentProvider(*reflectionComponentProvider_);
		}
		auto commandManager = contextManager.queryInterface<ICommandManager>();
		if (commandManager)
		{
			reflectionSystemHolder_->getController()->init(*commandManager);
		}
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager) override
	{
		reflectionSystemHolder_->getController()->fini();
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type);
		}
	}
};

PLG_CALLBACK_FUNC(ReflectionPlugin)
} // end namespace wgt
