#include "core_generic_plugin/generic_plugin.hpp"
#include "core_dependency_system/wgt_static_init.hpp"
#include "reflection_system_holder.hpp"
#include "reflection_component_provider.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_object/object_manager.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_reflection/meta_callback_property_accessor_listener.hpp"

namespace wgt
{

/**
 * Usage: {,,plg_reflection.dll} Reflection::inspect( <Address_to ObjectHandle > )
 *		  {,,plg_reflection_d.dll} Reflection::inspectVariant( <Address_to Variant > )
 */
namespace Reflection
{
static IComponentContext* s_ReflectionDebugContext = nullptr;

WGTStaticExecutor s_Initializer([](IComponentContext& context) { s_ReflectionDebugContext = &context; });

static std::pair<std::string, std::string> inspectVariant(const Variant* variant = nullptr);

static std::map<std::string, std::pair<std::string, std::string>> inspect(const ObjectHandle* handle = nullptr)
{
	std::map<std::string, std::pair<std::string, std::string>> debugData;
	if (handle == nullptr)
	{
		debugData.insert(std::make_pair("Empty handle", std::make_pair("empty", "empty")));
		return debugData;
	}
	auto defManager = s_ReflectionDebugContext->queryInterface<IDefinitionManager>();
	if (defManager == nullptr)
	{
		debugData.insert(std::make_pair("Empty handle", std::make_pair("empty", "empty")));
		return debugData;
	}
	auto definition = defManager->getDefinition(*handle);
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
		std::wstring wString;
		variant.tryCast<std::wstring>(wString);
		auto output = StringUtils::to_string(wString);
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

/**
* A plugin which registers core reflection interfaces to allow reflection
* on class members for easy and unified serialization of data and exposing to UI code
*
* @ingroup plugins
* @ingroup coreplugins
*/
class ReflectionPlugin : public PluginMain
{
private:
	InterfacePtrs types_;
	std::unique_ptr<ReflectionComponentProvider> reflectionComponentProvider_;
	std::unique_ptr<ReflectionSystemHolder> reflectionSystemHolder_;
	std::shared_ptr<PropertyAccessorListener> metaCallbackListener_;

public:
	//==========================================================================
	ReflectionPlugin(IComponentContext& contextManager) : reflectionSystemHolder_(new ReflectionSystemHolder)
	{
		// Force linkage
		Reflection::inspect();
		Reflection::inspectVariant();
		ReflectionShared::initContext( contextManager );

		auto* definitionManager = reflectionSystemHolder_->getGlobalDefinitionManager();

		types_.push_back(contextManager.registerInterface(reflectionSystemHolder_->getGlobalMetaTypeCreator(), false));
		types_.push_back(contextManager.registerInterface(reflectionSystemHolder_->getGlobalObjectManager(), false));
		ReflectionShared::initDefinitionManager(*definitionManager);
		types_.push_back(contextManager.registerInterface(definitionManager, false));
		types_.push_back(
		contextManager.registerInterface(reflectionSystemHolder_->contextDefinitionManagerCreator(), false));
		types_.push_back(
		contextManager.registerInterface(reflectionSystemHolder_->contextObjectManagerCreator(), false));

		metaCallbackListener_ = std::make_shared<MetaCallbackPropertyAccessorListener>();
		definitionManager->registerPropertyAccessorListener(metaCallbackListener_);
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager) override
	{
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		if (uiFramework)
		{
			reflectionComponentProvider_.reset(
			new ReflectionComponentProvider(*reflectionSystemHolder_->getGlobalDefinitionManager()));
			uiFramework->registerComponentProvider(*reflectionComponentProvider_);
		}
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager) override
	{
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager) override
	{
        reflectionSystemHolder_->finalise(metaCallbackListener_);

		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}
};

PLG_CALLBACK_FUNC(ReflectionPlugin)
} // end namespace wgt
