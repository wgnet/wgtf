#include "meta_impl.hpp"

#include "meta_types.hpp"
#include "core_common/assert.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "wg_types/color_utilities.hpp"
#include "wg_types/hash_utilities.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/metadata/meta_command_executable.hpp"
#include "core_reflection/interfaces/i_enum_generator.hpp"
#include "core_reflection/interfaces/i_signal_provider.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
//==============================================================================
MetaMinMaxObj::~MetaMinMaxObj() = default;
MetaMinMaxObj::MetaMinMaxObj(float min, float max) 
    : min_(min)
    , max_(max)
{
}

//==============================================================================
MetaMinMaxMappedObj::MetaMinMaxMappedObj(float min, float max, float mappedMin, float mappedMax)
	: MetaMinMaxObj(min, max)
	, mappedMin_(mappedMin)
	, mappedMax_(mappedMax)
{
}

//==============================================================================
const float MetaStepSizeObj::DefaultStepSize = 1.0;
MetaStepSizeObj::~MetaStepSizeObj() = default;
MetaStepSizeObj::MetaStepSizeObj()
    : stepSize_(DefaultStepSize)
{
}
MetaStepSizeObj::MetaStepSizeObj(float stepSize) 
    : stepSize_(stepSize)
{
}

//==============================================================================
const int MetaDecimalsObj::DefaultDecimals = 0;
MetaDecimalsObj::~MetaDecimalsObj() = default;
MetaDecimalsObj::MetaDecimalsObj()
    : decimals_(DefaultDecimals)
{
}
MetaDecimalsObj::MetaDecimalsObj(int decimals) 
    : decimals_(decimals)
{
}

//==============================================================================
MetaEnumObj::~MetaEnumObj() = default;
MetaEnumObj::MetaEnumObj() 
    : enumGenerator_(nullptr)
    , enumString_(nullptr)
{
}
MetaEnumObj::MetaEnumObj(IEnumGeneratorPtr enumGenerator)
    : enumGenerator_(std::move(enumGenerator))
    , enumString_(nullptr)
{
}
MetaEnumObj::MetaEnumObj(const wchar_t* enumString) 
    : enumGenerator_(nullptr)
    , enumString_(enumString)
{
}

const wchar_t* MetaEnumObj::getEnumString() const
{
	return enumString_;
}

Collection MetaEnumObj::generateEnum(const ObjectHandle& provider)
{
	if (enumGenerator_ == nullptr)
	{
		return Collection();
	}
	return enumGenerator_->getCollection(provider);
}

Collection MetaEnumObj::generateEnum(const ObjectHandle& provider) const
{
	if (enumGenerator_ == nullptr)
{
		return Collection();
	}
	return enumGenerator_->getCollection(provider);
}

//==============================================================================
MetaSliderObj::MetaSliderObj() = default;
MetaSliderObj::~MetaSliderObj() = default;

//==============================================================================
MetaGroupObj::~MetaGroupObj() = default;
MetaGroupObj::MetaGroupObj()
	: groupName_(NULL)
	, groupNameHash_(0)
{
}

MetaGroupObj::MetaGroupObj(const wchar_t* groupName)
	: groupName_(NULL)
	, groupNameHash_(0)
{

	auto addNameToMetaGroup = [this](std::wstring src, int offsetToName, int nameLen) {
		NameAndHash result = {};
		result.name = src.substr(offsetToName, nameLen);

		std::wstring nameToHash = src.substr(0, offsetToName + nameLen);
		result.hash = HashUtilities::compute(nameToHash);
		groupNames_.push_back(result);
	};

	if (groupName) 
	{
		const wchar_t *namePtr = groupName;
		auto groupNameWString  = std::wstring(groupName);

		addNameToMetaGroup(groupNameWString, 0, (int)groupNameWString.length());
		groupName_     = groupNames_[0].name.c_str();
		groupNameHash_ = groupNames_[0].hash;

		int nameOffset        = 0;
		int nameLen           = 0;
		bool singularNameOnly = true;
		while (namePtr[0] != '\0') 
		{
			namePtr++;
			nameLen++;

			if (*namePtr == '\\' || *namePtr == '/') 
			{
				int offset = 0;
				if (!singularNameOnly) offset = nameOffset;
				singularNameOnly = false;

				addNameToMetaGroup(groupNameWString, offset, nameLen);
				namePtr++;
				nameOffset += (nameLen + 1);
				nameLen = 0;
			}
		}

		if (nameLen > 0 && !singularNameOnly) 
		{
			// NOTE: Last subgroup name uses the full group hash, because properties are bound 
			// to the full name hash.
			NameAndHash result = {};
			result.name = groupNameWString.substr(nameOffset, nameLen);
			result.hash = groupNames_[0].hash;
			groupNames_.push_back(result);
		}
	}
}

const wchar_t *MetaGroupObj::getGroupName(const ObjectHandle&, int subGroupIndex) const
{
	if (groupNames_.size() == 0) 
	{
		groupName_ = nullptr;
	}
	else if (subGroupIndex >= 0 && subGroupIndex < static_cast<int>(groupNames_.size()) || subGroupIndex == FULL_GROUP_LEVEL) 
	{
		subGroupIndex = normalizeSubGroupIndex(subGroupIndex);
		groupName_ = groupNames_[subGroupIndex].name.c_str();
	}
	else 
	{
		groupName_ = groupNames_[0].name.c_str();
	}

	return groupName_;
}

uint64_t MetaGroupObj::getGroupNameHash(const ObjectHandle&, int subGroupIndex) const
{
	if (groupNames_.size() == 0) 
	{
		groupNameHash_ = 0;
	}
	else if (subGroupIndex >= 0 && subGroupIndex < static_cast<int>(groupNames_.size()) || 
		     subGroupIndex == FULL_GROUP_LEVEL) 
	{
		subGroupIndex = normalizeSubGroupIndex(subGroupIndex);
		groupNameHash_ =  groupNames_[subGroupIndex].hash;
	}
	else 
	{
		groupNameHash_ =  0;
	}

	return groupNameHash_;
}

const int MetaGroupObj::getNumSubGroups() const
{
	return (int)groupNames_.size();
}

const int MetaGroupObj::normalizeSubGroupIndex(const int index) const
{
	int result;
	if      (index == FULL_GROUP_LEVEL)  result = 0;
	else if (index >= getNumSubGroups()) result = 0;
	else                                 result = index;

	return result;
}

//==============================================================================
MetaGroupCallbackObj::~MetaGroupCallbackObj() = default;
MetaGroupCallbackObj::MetaGroupCallbackObj(const GetTextCallback& callback) : callback_(callback)
{
}

const wchar_t *MetaGroupCallbackObj::getGroupName(const ObjectHandle& object, int /*level*/) const
{
	groupNameString_ = callback_(object);
	return groupNameString_.c_str();
}

uint64_t MetaGroupCallbackObj::getGroupNameHash(const ObjectHandle& object, int /*level*/) const
{
	return HashUtilities::compute(getGroupName(object));
}

//==============================================================================
MetaAttributeDisplayNameObj::~MetaAttributeDisplayNameObj() = default;
MetaAttributeDisplayNameObj::MetaAttributeDisplayNameObj()
    : attributeName_(NULL)
{
}
MetaAttributeDisplayNameObj::MetaAttributeDisplayNameObj(const char* attributeName) 
    : attributeName_(attributeName)
{
}

const char* MetaAttributeDisplayNameObj::getAttributeName() const
{
	return attributeName_;
}

//==============================================================================
MetaDisplayNameObj::~MetaDisplayNameObj() = default;
MetaDisplayNameObj::MetaDisplayNameObj(const wchar_t* displayName) 
    : displayName_(displayName)
{
}

const wchar_t* MetaDisplayNameObj::getDisplayName(const ObjectHandle& /*handle*/) const
{
	return displayName_;
}

const wchar_t* MetaDisplayNameObj::getDisplayName() const
{
	return displayName_;
}

//==============================================================================
MetaDisplayNameCallbackObj::~MetaDisplayNameCallbackObj() = default;
MetaDisplayNameCallbackObj::MetaDisplayNameCallbackObj(GetTextCallback action) 
    : MetaDisplayNameObj(nullptr)
    , callback_(action)
{
}

const wchar_t* MetaDisplayNameCallbackObj::getDisplayName() const
{
	ObjectHandle empty;
	return getDisplayName(empty);
}

const wchar_t* MetaDisplayNameCallbackObj::getDisplayName(const ObjectHandle& handle) const
{
	if (callback_)
	{
		displayNameString_ = callback_(handle);
	}

	return displayNameString_.c_str();
}

//==============================================================================
MetaDisplayPathNameCallbackObj::~MetaDisplayPathNameCallbackObj() = default;
MetaDisplayPathNameCallbackObj::MetaDisplayPathNameCallbackObj(GetPathTextCallback action)
    : callback_(action)
{
}
const std::string MetaDisplayPathNameCallbackObj::getDisplayName(const std::string& path, const ObjectHandle& handle) const
{
	if (callback_)
	{
		return callback_(path, handle);
	}
	return "";
}

//==============================================================================
MetaIconObj::MetaIconObj(const char* icon) : icon_(icon)
{
}

const char* MetaIconObj::getIcon() const
{
	return icon_;
}

//==============================================================================
MetaStaticStringObj::~MetaStaticStringObj() = default;
MetaStaticStringObj::MetaStaticStringObj(const IStaticStringDatabase* staticStringDatabase)
    : staticStringDatabase_(staticStringDatabase) 
{
}

Variant MetaStaticStringObj::getStaticStringId(const std::string& str) 
{
	if(staticStringDatabase_)
	{
		const auto id = staticStringDatabase_->find(str.c_str());
		if(id != staticStringDatabase_->npos())
		{
			return Variant(id);
		}
	}
	return Variant();
}

const char* MetaStaticStringObj::getStaticString(uint64_t stringId) const
{
	return staticStringDatabase_ ? staticStringDatabase_->getText(stringId) : nullptr;
}

const char* MetaStaticStringObj::getStaticStringExpose(uint64_t stringId)
{
	return getStaticString(stringId);
}

Collection MetaStaticStringObj::getAllStaticStringIds() const
{
	return staticStringDatabase_ ? staticStringDatabase_->idToStringMapping() : Collection();
}

Collection MetaStaticStringObj::getAllStaticStringIdsExpose()
{
	return getAllStaticStringIds();
}

//==============================================================================
MetaDescriptionObj::~MetaDescriptionObj() = default;
MetaDescriptionObj::MetaDescriptionObj(const wchar_t* description) : description_(description)
{
}

const wchar_t* MetaDescriptionObj::getDescription() const
{
	return description_;
}

//==============================================================================
MetaPanelLayoutObj::~MetaPanelLayoutObj() = default;
MetaPanelLayoutObj::MetaPanelLayoutObj(const char* layoutFile, const char* bindingsFile)
    : layoutFile_(layoutFile), bindingsFile_(bindingsFile)
{
}

//==============================================================================
MetaNoNullObj::MetaNoNullObj() = default;
MetaNoNullObj::~MetaNoNullObj() = default;

//==============================================================================
MetaColorObj::~MetaColorObj() = default;
MetaColorObj::MetaColorObj() = default;

//==============================================================================
MetaKelvinColorObj::~MetaKelvinColorObj() = default;
MetaKelvinColorObj::MetaKelvinColorObj(std::function<Vector4(int)> converter)
	: MetaComponentObj("colorkelvin")
	, converter_(converter)
{
	TF_ASSERT(converter_);
}

Vector4 MetaKelvinColorObj::convert(int v)
{
	return converter_(v);
}

//==============================================================================
MetaHDRColorObj::~MetaHDRColorObj() = default;
MetaHDRColorObj::MetaHDRColorObj(std::function<Vector4(const Vector4&)> tonemapOperator)
	: MetaComponentObj("colorhdr")
    , tonemapOperator_(tonemapOperator)
{
	TF_ASSERT(tonemapOperator_);
}

Vector4 MetaHDRColorObj::tonemap(const Vector4& v)
{
	return tonemapOperator_(v);
}

//==============================================================================
float MetaHDRColorReinhardTonemapObj::luminanceWhite_ = 2.0f;
MetaHDRColorReinhardTonemapObj::~MetaHDRColorReinhardTonemapObj() = default;
MetaHDRColorReinhardTonemapObj::MetaHDRColorReinhardTonemapObj()
    : MetaHDRColorObj(&MetaHDRColorReinhardTonemapObj::tonemap)
{
}

Vector4 MetaHDRColorReinhardTonemapObj::tonemap(const Vector4& v)
{
	return ColorUtilities::reinhardTonemap(v, luminanceWhite_);
}

float MetaHDRColorReinhardTonemapObj::getLuminanceWhite()
{
	return luminanceWhite_;
}

void MetaHDRColorReinhardTonemapObj::setLuminanceWhite(float v)
{
	luminanceWhite_ = v;
}

//==============================================================================
MetaHiddenObj::~MetaHiddenObj() = default;
MetaHiddenObj::MetaHiddenObj(Predicate predicate) 
    : predicate_(predicate)
{
}

//==============================================================================
MetaThumbnailObj::~MetaThumbnailObj() = default;
MetaThumbnailObj::MetaThumbnailObj(int width, int height) 
    : width_(width)
    , height_(height)
{
}

//==============================================================================
MetaInPlaceObj::~MetaInPlaceObj() = default;
MetaInPlaceObj::MetaInPlaceObj(const char* propName) 
    : propName_(propName)
{
}

//==============================================================================
MetaSelectedObj::~MetaSelectedObj() = default;
MetaSelectedObj::MetaSelectedObj(const char* propName) 
    : propName_(propName)
{
}

//==============================================================================
MetaActionObj::~MetaActionObj() = default;
MetaActionObj::MetaActionObj(const char* actionName, Action action, IDefinitionManager* definitionManager)
    : actionName_(actionName)
    , action_(action)
    , definitionManager_(definitionManager)
{
}

void MetaActionObj::execute(const ObjectHandle& handle, const std::string& path)
{
	if (action_)
	{
		TF_ASSERT(definitionManager_ != nullptr);
		auto definition = definitionManager_->getObjectDefinition(handle);
		TF_ASSERT(definition != nullptr);
		auto propertyAccessor = definition->bindProperty(path.c_str(), handle);

		auto listeners = definitionManager_->getPropertyAccessorListeners();
		auto itBegin = listeners.cbegin();
		auto itEnd = listeners.cend();
		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->preSetValue(propertyAccessor, Variant());
		}

		action_(handle);

		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->postSetValue(propertyAccessor, Variant());
		}
	}
}

//==============================================================================
MetaCommandObj::MetaCommandObj(const wchar_t* commandName, const IMetaCommandExecutable* commandExecutable)
    : commandName_(commandName)
    , commandExecutable_(commandExecutable)
{
}
MetaCommandObj::MetaCommandObj() 
    : commandName_(nullptr)
    , commandExecutable_(nullptr)
{
}
MetaCommandObj::~MetaCommandObj()
{
	delete commandExecutable_;
}

void MetaCommandObj::execute(void* pBase, void* arguments) const
{
	commandExecutable_->execute(pBase, arguments);
}

//==============================================================================
MetaNoSerializationObj::MetaNoSerializationObj() = default;
MetaNoSerializationObj::~MetaNoSerializationObj() = default;

//==============================================================================
MetaUniqueIdObj::~MetaUniqueIdObj() = default;
MetaUniqueIdObj::MetaUniqueIdObj(const char* id) 
    : id_(id)
{
}
const char* MetaUniqueIdObj::getId() const
{
	return id_;
}

//==============================================================================
MetaOnStackObj::MetaOnStackObj() = default;
MetaOnStackObj::~MetaOnStackObj() = default;

//==============================================================================
MetaInPlacePropertyNameObj::~MetaInPlacePropertyNameObj() = default;
MetaInPlacePropertyNameObj::MetaInPlacePropertyNameObj(const char* propertyName) 
    : propName_(propertyName)
{
}
const char* MetaInPlacePropertyNameObj::getPropertyName() const
{
	return propName_;
}

//==============================================================================
MetaReadOnlyObj::~MetaReadOnlyObj() = default;
MetaReadOnlyObj::MetaReadOnlyObj(Predicate predicate)
    : predicate_(predicate)
{
}

//==============================================================================
MetaUrlObj::~MetaUrlObj() = default;
MetaUrlObj::MetaUrlObj(bool native, const std::string& title, const std::string& folder, const std::string& nameFilters,
                       const std::string& selectedNameFilter)
    : native_(native), title_(title), folder_(folder), nameFilters_(nameFilters),
      selectedNameFilter_(selectedNameFilter)
{
	if (nameFilters_.empty())
	{
		nameFilters_ = "All Files (*)";
	}

	if (selectedNameFilter_.empty())
	{
		selectedNameFilter_ = nameFilters_.substr(0, nameFilters_.find('|'));
	}
}

bool MetaUrlObj::native() const
{
	return native_;
}

const std::string& MetaUrlObj::title() const
{
	return title_;
}

const std::string& MetaUrlObj::folder() const
{
	return folder_;
}

const std::string& MetaUrlObj::nameFilters() const
{
	return nameFilters_;
}

const std::string& MetaUrlObj::selectedNameFilter() const
{
	return selectedNameFilter_;
}

bool MetaUrlObj::isAssetBrowserDialog() const
{
	return !native();
}

const char* MetaUrlObj::getDialogTitle() const
{
	return title().c_str();
}

const char* MetaUrlObj::getDialogDefaultFolder() const
{
	return folder().c_str();
}

const char* MetaUrlObj::getDialogNameFilters() const
{
	return nameFilters().c_str();
}

const char* MetaUrlObj::getDialogSelectedNameFilter() const
{
	return selectedNameFilter().c_str();
}

//==============================================================================
MetaUniqueObj::~MetaUniqueObj() = default;
MetaUniqueObj::MetaUniqueObj() = default;

//==============================================================================
MetaParamHelpObj::~MetaParamHelpObj() = default;
MetaParamHelpObj::MetaParamHelpObj(const char* paramName, const MetaParamTypes::MetaParamType paramType, const char* paramDesc)
    : name_(paramName)
    , desc_(paramDesc)
    , type_(paramType)
{
}

//==============================================================================
MetaReturnHelpObj::~MetaReturnHelpObj() = default;
MetaReturnHelpObj::MetaReturnHelpObj(const char* returnName, const MetaParamTypes::MetaParamType returnType, const char* returnDesc)
    : name_(returnName)
    , desc_(returnDesc)
    , type_(returnType)
{
}

//==============================================================================
MetaConsoleHelpObj::~MetaConsoleHelpObj() = default;
MetaConsoleHelpObj::MetaConsoleHelpObj(const char* text) 
    : text_(text)
{
}

//==============================================================================
MetaScriptFunctionHelpObj::~MetaScriptFunctionHelpObj() = default;
MetaScriptFunctionHelpObj::MetaScriptFunctionHelpObj(const char* name) 
    : name_(name)
{
}

//==============================================================================
MetaTooltipObj::~MetaTooltipObj() = default;
MetaTooltipObj::MetaTooltipObj(const wchar_t* tooltip) 
    : tooltip_(tooltip)
    , callback_(nullptr)
{
}
MetaTooltipObj::MetaTooltipObj(const GetTextCallback& callback) 
    : tooltip_(L"")
    , callback_(callback)
{
}

//==============================================================================
MetaCheckBoxObj::~MetaCheckBoxObj() = default;
MetaCheckBoxObj::MetaCheckBoxObj()
	: MetaComponentObj("boolean")
{
}

//==============================================================================
MetaPasswordObj::~MetaPasswordObj() = default;
MetaPasswordObj::MetaPasswordObj() = default;

//==============================================================================
MetaMultilineObj::~MetaMultilineObj() = default;
MetaMultilineObj::MetaMultilineObj() = default;

//==============================================================================
MetaDirectInvokeObj::~MetaDirectInvokeObj() = default;
MetaDirectInvokeObj::MetaDirectInvokeObj() = default;

//==============================================================================
MetaComponentObj::~MetaComponentObj() = default;
MetaComponentObj::MetaComponentObj(const char* componentName) : componentName_(componentName)
{
}

//==============================================================================
MetaAngleObj::~MetaAngleObj() = default;
MetaAngleObj::MetaAngleObj(bool storeRadians, bool showRadians)
    : MetaComponentObj("angle")
    , storeRadians_(storeRadians)
	, showRadians_(showRadians)
{
}

//==============================================================================
MetaTimeObj::~MetaTimeObj() = default;
MetaTimeObj::MetaTimeObj() 
    : MetaComponentObj("time")
{
}

//==============================================================================
MetaSignalObj::~MetaSignalObj() = default;
MetaSignalObj::MetaSignalObj() = default;
MetaSignalObj::MetaSignalObj(ISignalProviderPtr signalProvider) : signalProvider_(std::move(signalProvider))
{
}

Signal<void(Variant&)>* MetaSignalObj::getSignal(const ObjectHandle& provider,
                                                 const IDefinitionManager& definitionManager) const
{
	if (signalProvider_)
	{
		return signalProvider_->getSignal(provider, definitionManager);
	}

	return nullptr;
}

//==============================================================================
MetaCallbackObj::~MetaCallbackObj() = default;
MetaCallbackObj::MetaCallbackObj(Action action) 
    : action_(action)
{
}

//==============================================================================
MetaCollectionItemMetaObj::~MetaCollectionItemMetaObj() = default;
MetaCollectionItemMetaObj::MetaCollectionItemMetaObj(MetaData meta)
    : meta_(std::move(meta))
{
}

void MetaCollectionItemMetaObj::appendMetaData(MetaData&& meta)
{
	meta_ += std::move(meta);
}

const MetaData & MetaCollectionItemMetaObj::getMetaData() const
{
	return meta_;
}

//==============================================================================
MetaDropHitTestObj::~MetaDropHitTestObj() = default;

//==============================================================================
MetaEnableValueComponentForObjectObj::MetaEnableValueComponentForObjectObj(ObjectValueToVariantCallback callback)
	: MetaComponentObj(""), callback_(callback)
{
}

MetaEnableValueComponentForObjectObj::MetaEnableValueComponentForObjectObj(const char* componentName)
	: MetaComponentObj(componentName)
{
}

MetaEnableValueComponentForObjectObj::~MetaEnableValueComponentForObjectObj() = default;

const char* MetaEnableValueComponentForObjectObj::getComponentType(const ObjectHandle& handle) const
{
	return handle.type().getName();
}

Variant MetaEnableValueComponentForObjectObj::getComponentValue(const ObjectHandle& handle) const
{
	if (callback_)
	{
		return callback_(handle);
	}
	return Variant(handle);
}

//==============================================================================
const IDirectBaseHelper::ParentCollection & MetaBasesHolderObj::getParents() const
{
	return impl_->getParents();
}

//------------------------------------------------------------------------------
const IDirectBaseHelper::CasterCollection & MetaBasesHolderObj::getCasters() const
{
	return impl_->getCasters();
}

MetaInsertObj::MetaInsertObj(InsertCallback insertCallback, CanInsertCallback canInsertCallback, CanRemoveCallback canRemoveCallback)
	: insertCallback_(insertCallback)
	, canInsertCallback_(canInsertCallback)
	, canRemoveCallback_(canRemoveCallback)
{
}

void MetaInsertObj::insert(const ObjectHandle& handle, int64_t index) const
{
	if (insertCallback_)
	{
		insertCallback_(handle, index);
	}
}

bool MetaInsertObj::canInsert(const ObjectHandle& handle) const
{
	if (canInsertCallback_)
	{
		return canInsertCallback_(handle);
	}

	return true;
}

bool MetaInsertObj::canRemove(const ObjectHandle& handle) const
{
	if (canRemoveCallback_)
	{
		return canRemoveCallback_(handle);
	}

	return true;
}

} // end namespace wgt
