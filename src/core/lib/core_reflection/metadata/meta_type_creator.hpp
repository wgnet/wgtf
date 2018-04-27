#include "i_meta_type_creator.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/definition_helpers.hpp"
#include "core_reflection/interfaces/i_signal_provider.hpp"
#include "core_object/object_handle_cast_utils.hpp"
#include "core_reflection/metadata/meta_command_executable.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_object/managed_object.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "wg_types/color_utilities.hpp"

namespace wgt
{
class MetaTypeCreator : public Implements<IMetaTypeCreator>
{
public:
	//==============================================================================
	virtual MetaData MetaMinMax(float min, float max) override
	{
		return MetaData::create<MetaMinMaxObj>(min, max);
	}

	//==============================================================================
	virtual MetaData MetaMinMaxMapped(float min, float max, float mappedMin, float mappedMax) override
	{
		return MetaData::create<MetaMinMaxMappedObj>(min, max, mappedMin, mappedMax);
	}

	//==============================================================================
	virtual MetaData MetaEnum(IEnumGeneratorPtr enumGenerator) override
	{
        return MetaData::create<MetaEnumObj>(std::move(enumGenerator));
	}

	//==============================================================================
	virtual MetaData MetaEnum(const wchar_t* enumString) override
	{
		return MetaData::create<MetaEnumObj>(enumString);
	}

	//==============================================================================
	virtual MetaData MetaSlider() override
	{
		return MetaData::create<MetaSliderObj>();
	}

	//==============================================================================
	virtual MetaData MetaGroup(const wchar_t* groupName) override
	{
        return MetaData::create<MetaGroupObj>(groupName);
	}

	//==============================================================================
	virtual MetaData MetaGroupCallback(const GetTextCallback& callback) override
	{
		return MetaData::create<MetaGroupCallbackObj>(callback);
	}

	//==============================================================================
	virtual MetaData MetaAttributeDisplayName(const char* attributePath) override
	{
		return MetaData::create<MetaAttributeDisplayNameObj>(attributePath);
	}

	//==============================================================================
	virtual MetaData MetaDisplayName(const wchar_t* displayName) override
	{
		return MetaData::create<MetaDisplayNameObj>(displayName);
	}

	//==============================================================================
	virtual MetaData MetaDisplayNameCallback(GetTextCallback callback) override
	{
		return MetaData::create<MetaDisplayNameCallbackObj>(callback);
	}

	//==============================================================================
	virtual MetaData MetaIcon(const char* icon) override
	{
		return MetaData::create<MetaIconObj>(icon);
	}

	//==============================================================================
	virtual MetaData MetaDescription(const wchar_t* description) override
	{
		return MetaData::create<MetaDescriptionObj>(description);
	}

	//==============================================================================
	virtual MetaData MetaPanelLayout(const char* layoutFile, const char* bindingsFile) override
	{
		return MetaData::create<MetaPanelLayoutObj>(layoutFile, bindingsFile);
	}

	//==============================================================================
	virtual MetaData MetaNoNull() override
	{
		return MetaData::create<MetaNoNullObj>();
	}

	//==============================================================================
	virtual MetaData MetaColor() override
	{
		return MetaData::create<MetaColorObj>();
	}

	//==============================================================================
	virtual MetaData MetaKelvinColor(std::function<Vector4(int)> converter) override
	{
		return converter ?
			MetaData::create<MetaKelvinColorObj>(converter) :
			MetaData::create<MetaKelvinColorObj>(&ColorUtilities::convertKelvinToRGB);
	}

	//==============================================================================
	virtual MetaData MetaHDRColor(std::function<Vector4(const Vector4&)> tonemapOperator) override
	{
        return tonemapOperator ?
            MetaData::create<MetaHDRColorObj>(tonemapOperator) :
            MetaData::create<MetaHDRColorReinhardTonemapObj>();
	}

	//==============================================================================
	virtual MetaData MetaDirectInvoke() override
	{
		return MetaData::create<MetaDirectInvokeObj>();
	}

	//==============================================================================
	virtual MetaData MetaStepSize(float stepSize) override
	{
		return MetaData::create<MetaStepSizeObj>(stepSize);
	}

	//==============================================================================
	virtual MetaData MetaDecimals(int decimals) override
	{
		return MetaData::create<MetaDecimalsObj>(decimals);
	}

	//==============================================================================
	virtual MetaData MetaHidden(const Predicate& predicate) override
	{
		return MetaData::create<MetaHiddenObj>(predicate);
	}

	//==============================================================================
	virtual MetaData MetaThumbnail(int width, int height) override
	{
		return MetaData::create<MetaThumbnailObj>(width, height);
	}

	//==============================================================================
	virtual MetaData MetaInPlace(const char* propertyName) override
	{
		return MetaData::create<MetaInPlaceObj>(propertyName);
	}

	//==============================================================================
	virtual MetaData MetaSelected(const char* propertyName) override
	{
		return MetaData::create<MetaSelectedObj>(propertyName);
	}

	//==============================================================================
	virtual MetaData MetaInPlacePropertyName(const char* propertyName) override
	{
		return MetaData::create<MetaInPlacePropertyNameObj>(propertyName);
	}

	//==============================================================================
	virtual MetaData MetaAction(const char* actionName, Action action) override
	{
        DependsLocal<IDefinitionManager> depends;
		return MetaData::create<MetaActionObj>(actionName, action, depends.get<IDefinitionManager>());
	}

	//==============================================================================
	virtual MetaData MetaCommandBase(const wchar_t* commandName, const IMetaCommandExecutable* commandExecutable) override
	{
		return MetaData::create<MetaCommandObj>(commandName, commandExecutable);
	}

	//==============================================================================
	virtual MetaData MetaComponent(const char* componentType) override
	{
		return MetaData::create<MetaComponentObj>(componentType);
	}

	//==============================================================================
	virtual MetaData MetaNoSerialization() override
	{
		return MetaData::create<MetaNoSerializationObj>();
	}

	//==============================================================================
	virtual MetaData MetaUniqueId(const char* id) override
	{
		return MetaData::create<MetaUniqueIdObj>(id);
	}

	//==============================================================================
	virtual MetaData MetaOnStack() override
	{
		return MetaData::create<MetaOnStackObj>();
	}

	//==============================================================================
	virtual MetaData MetaReadOnly(const Predicate& predicate) override
	{
		return MetaData::create<MetaReadOnlyObj>(predicate);
	}

	//==============================================================================
	virtual MetaData MetaUrl(bool native, 
							  const std::string& title, 
							  const std::string& folder, 
							  const std::string& nameFilters,
							  const std::string& selectedNameFilter) override
	{
		return MetaData::create<MetaUrlObj>(native, title, folder, nameFilters, selectedNameFilter);
	}

	//==============================================================================
	virtual MetaData MetaUnique() override
	{
		return MetaData::create<MetaUniqueObj>();
	}

	//==============================================================================
	virtual MetaData MetaParamHelp(const char* paramName, const MetaParamTypes::MetaParamType paramType, const char* paramDesc) override
	{
		return MetaData::create<MetaParamHelpObj>(paramName, paramType, paramDesc);
	}

	//==============================================================================
	virtual MetaData MetaReturnHelp(const char* returnName, const MetaParamTypes::MetaParamType returnType, const char* returnDesc) override
	{
		return MetaData::create<MetaReturnHelpObj>(returnName, returnType, returnDesc);
	}

	//==============================================================================
	virtual MetaData MetaConsoleHelp(const char* text) override
	{
		return MetaData::create<MetaConsoleHelpObj>(text);
	}

	//==============================================================================
	virtual MetaData MetaScriptFunctionHelp(const char* name) override
	{
		return MetaData::create<MetaScriptFunctionHelpObj>(name);
	}

	//==============================================================================
	virtual MetaData MetaTooltip(const wchar_t* tooltip) override
	{
		return MetaData::create<MetaTooltipObj>(tooltip);
	}

	//==============================================================================
	virtual MetaData MetaTooltip(const GetTextCallback& callback) override
	{
		return MetaData::create<MetaTooltipObj>(callback);
	}

	//==============================================================================
	virtual MetaData MetaPassword() override
	{
		return MetaData::create<MetaPasswordObj>();
	}

	//==============================================================================
	virtual MetaData MetaCheckBox() override
	{
		return MetaData::create<MetaCheckBoxObj>();
	}

	//==============================================================================
	virtual MetaData MetaSignal(ISignalProviderPtr signalProvider) override
	{
		return MetaData::create<MetaSignalObj>(std::move(signalProvider));
	}

	//==============================================================================
	virtual MetaData MetaMultiline() override
	{
		return MetaData::create<MetaMultilineObj>();
	}

	//==============================================================================
	virtual MetaData MetaAngle(bool storeRadians, bool showRadians) override
	{
		return MetaData::create<MetaAngleObj>(storeRadians, showRadians);
	}

	//==============================================================================
	virtual MetaData MetaTime() override
	{
		return MetaData::create<MetaTimeObj>();
	}

	//==============================================================================
	virtual MetaData MetaInvalidatesObject() override
	{
		return MetaData::create<MetaInvalidatesObjectObj>();
	}

	//==============================================================================
	virtual MetaData MetaCallback(Action action) override
	{
		return MetaData::create<MetaCallbackObj>(action);
	}
	
	//==============================================================================
	virtual MetaData MetaCollectionItemMeta(MetaData meta) override
	{
        return MetaData::create<MetaCollectionItemMetaObj>(std::move(meta));
	}

	//==============================================================================
	virtual MetaData MetaDisplayPathNameCallback(GetPathTextCallback callback) override
	{
		return MetaData::create<MetaDisplayPathNameCallbackObj>(callback);
	}
	
	//==============================================================================
	virtual MetaData MetaStaticString() override
	{
		DependsLocal<IStaticStringDatabase> depends;
		return MetaData::create<MetaStaticStringObj>(depends.get<IStaticStringDatabase>());
	}

	//-------------------------------------------------------------------------------
	virtual MetaData MetaEnableValueComponentForObject() override
	{
		return MetaData::create<MetaEnableValueComponentForObjectObj>();
	}

	//-------------------------------------------------------------------------------
	virtual MetaData MetaEnableValueComponentForObject(std::function<Variant (const ObjectHandle& handle)> callback) override
	{
		return MetaData::create<MetaEnableValueComponentForObjectObj>(callback);
	}

	//-------------------------------------------------------------------------------
	virtual MetaData MetaBasesHolder(std::unique_ptr< IDirectBaseHelper > & impl ) override
	{
		return MetaData::create<MetaBasesHolderObj>( impl );
	}

	//-------------------------------------------------------------------------------
	virtual MetaData MetaAutoAction(const char* actionId) override
	{
		return MetaData::create<MetaAutoActionObj>(actionId);
	}

	//-------------------------------------------------------------------------------
	virtual MetaData MetaInsert(InsertCallback insertCallback, CanInsertCallback canInsertCallback, CanRemoveCallback canRemoveCallback) override
	{
		return MetaData::create<MetaInsertObj>(insertCallback, canInsertCallback, canRemoveCallback);
	}
};
} // end namespace wgt
