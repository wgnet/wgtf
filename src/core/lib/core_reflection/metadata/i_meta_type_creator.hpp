#pragma once

#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/interfaces/i_direct_base_helper.hpp"
#include "core_reflection/interfaces/i_enum_generator.hpp"
#include "core_reflection/types/signal_provider_impl.hpp"

namespace wgt
{
class IMetaCommandExecutable;
typedef std::unique_ptr<IEnumGenerator> IEnumGeneratorPtr;
typedef std::function<void(const ObjectHandle&)> Action;
typedef std::function<bool(const ObjectHandle&)> Predicate;
typedef std::function<std::wstring(const ObjectHandle&)> GetTextCallback;
typedef std::function<std::string(const std::string& path, const ObjectHandle&)> GetPathTextCallback;
typedef std::function<void(const ObjectHandle&, int64_t index)> InsertCallback;
typedef std::function<bool(const ObjectHandle&)> CanInsertCallback;
typedef std::function<bool(const ObjectHandle&)> CanRemoveCallback;

class IMetaTypeCreator
{
public:
	virtual ~IMetaTypeCreator() {}

	virtual MetaData MetaMinMax(float min, float max) = 0;

	virtual MetaData MetaMinMaxMapped(float min, float max, float mappedMin, float mappedMax) = 0;

	virtual MetaData MetaStepSize(float stepSize) = 0;

	virtual MetaData MetaDecimals(int decimals) = 0;

	virtual MetaData MetaEnum(IEnumGeneratorPtr enumGenerator) = 0;

	virtual MetaData MetaEnum(const wchar_t* enumString) = 0;

	virtual MetaData MetaSlider() = 0;

	virtual MetaData MetaGroup(const wchar_t* groupName) = 0;

	virtual MetaData MetaGroupCallback(const GetTextCallback& callback) = 0;

	virtual MetaData MetaAttributeDisplayName(const char* attributePath) = 0;

	virtual MetaData MetaDisplayName(const wchar_t* displayName) = 0;

	virtual MetaData MetaDisplayNameCallback(GetTextCallback callback) = 0;

	virtual MetaData MetaIcon(const char* icon) = 0;

	virtual MetaData MetaDescription(const wchar_t* displayName) = 0;

	virtual MetaData MetaPanelLayout(const char* layoutFile, const char* bindingsFile = NULL) = 0;

	virtual MetaData MetaNoNull() = 0;

	virtual MetaData MetaColor() = 0;

	virtual MetaData MetaKelvinColor(std::function<Vector4(int)> converter) = 0;

	virtual MetaData MetaHDRColor(std::function<Vector4(const Vector4&)> tonemapOperator = nullptr) = 0;

	virtual MetaData MetaDirectInvoke() = 0;

	virtual MetaData MetaHidden(const Predicate& predicate) = 0;

	virtual MetaData MetaThumbnail(int width = -1, int height = -1) = 0;

	virtual MetaData MetaInPlace(const char* propertyName = NULL) = 0;

	virtual MetaData MetaSelected(const char* propertyName) = 0;

	virtual MetaData MetaInPlacePropertyName(const char* propertyName) = 0;

	virtual MetaData MetaReadOnly(const Predicate& predicate) = 0;

	virtual MetaData MetaUrl(	bool native,
								const std::string& title = "", 
								const std::string& folder = "",
								const std::string& nameFilters = "", 
								const std::string& selectedNameFilter = "") = 0;

	virtual MetaData MetaAction(const char* actionName, Action action) = 0;

	virtual MetaData MetaCommandBase(const wchar_t* commandName, const IMetaCommandExecutable* executeFunc) = 0;

	virtual MetaData MetaComponent(const char* componentType) = 0;

	virtual MetaData MetaNoSerialization() = 0;

	virtual MetaData MetaUniqueId(const char* id) = 0;
		
	virtual MetaData MetaOnStack() = 0;

	virtual MetaData MetaUnique() = 0;

	virtual MetaData MetaParamHelp(const char* paramName, 
									 const MetaParamTypes::MetaParamType paramType,
									 const char* paramDesc) = 0;

	virtual MetaData MetaReturnHelp(const char* returnName, 
									  const MetaParamTypes::MetaParamType returnType,
									  const char* returnDesc) = 0;

	virtual MetaData MetaConsoleHelp(const char* text) = 0;
	
	virtual MetaData MetaScriptFunctionHelp(const char* name) = 0;

	virtual MetaData MetaTooltip(const wchar_t* tooltip) = 0;
	virtual MetaData MetaTooltip(const GetTextCallback& callback) = 0;

	virtual MetaData MetaCheckBox() = 0;

	virtual MetaData MetaPassword() = 0;

	virtual MetaData MetaMultiline() = 0;

	virtual MetaData MetaSignal(ISignalProviderPtr signalProvider) = 0;

	virtual MetaData MetaAngle(bool storeRadians, bool showRadians) = 0;

	virtual MetaData MetaTime() = 0;

	virtual MetaData MetaInvalidatesObject() = 0;

	virtual MetaData MetaCallback(Action action) = 0;
	
	virtual MetaData MetaCollectionItemMeta(MetaData meta) = 0;

	virtual MetaData MetaDisplayPathNameCallback(GetPathTextCallback callback) = 0;

	virtual MetaData MetaStaticString() = 0;

	virtual MetaData MetaEnableValueComponentForObject() = 0;

	virtual MetaData MetaEnableValueComponentForObject(std::function<Variant (const ObjectHandle& handle)>) = 0;

	virtual MetaData MetaBasesHolder(std::unique_ptr< IDirectBaseHelper > & impl) = 0;

	virtual MetaData MetaAutoAction(const char* actionId) = 0;

	virtual MetaData MetaInsert(InsertCallback, CanInsertCallback, CanRemoveCallback) = 0;
};

} // end namespace wgt
