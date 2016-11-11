#include "meta_impl.hpp"
#include "meta_types.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/types/reflected_collection.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"

#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "../interfaces/i_enum_generator.hpp"

namespace wgt
{
//==============================================================================
BEGIN_EXPOSE(MetaNoneObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaMinMaxObj, MetaBase, MetaNone())
EXPOSE("min", getMin)
EXPOSE("max", getMax)
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaStepSizeObj, MetaBase, MetaNone())
EXPOSE("stepSize", getStepSize)
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaDecimalsObj, MetaBase, MetaNone())
EXPOSE("decimals", getDecimals)
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaEnumObj, MetaBase, MetaNone())
EXPOSE("enumString", getEnumString)
END_EXPOSE()

//==============================================================================
MetaMinMaxObj::MetaMinMaxObj(float min, float max) : min_(min), max_(max)
{
}

//==============================================================================
const float MetaStepSizeObj::DefaultStepSize = 1.0;
MetaStepSizeObj::MetaStepSizeObj(float stepSize) : stepSize_(stepSize)
{
}

//==============================================================================
const int MetaDecimalsObj::DefaultDecimals = 0;

MetaDecimalsObj::MetaDecimalsObj(int decimals) : decimals_(decimals)
{
}

//==============================================================================
MetaEnumObj::MetaEnumObj() : enumGenerator_(nullptr), enumString_(nullptr)
{
}

//==============================================================================
MetaEnumObj::MetaEnumObj(IEnumGeneratorPtr enumGenerator)
    : enumGenerator_(std::move(enumGenerator)), enumString_(nullptr)
{
}

//==============================================================================
MetaEnumObj::MetaEnumObj(const wchar_t* enumString) : enumGenerator_(nullptr), enumString_(enumString)
{
}

//==============================================================================
MetaEnumObj::~MetaEnumObj()
{
}

//==============================================================================
const wchar_t* MetaEnumObj::getEnumString() const
{
	return enumString_;
}

//==============================================================================
Collection MetaEnumObj::generateEnum(const ObjectHandle& provider, const IDefinitionManager& definitionManager) const
{
	return enumGenerator_->getCollection(provider, definitionManager);
}

//==============================================================================
BEGIN_EXPOSE(MetaSliderObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaGroupObj, MetaBase, MetaAttributeDisplayName("groupName"))
EXPOSE("groupName", groupName_)
END_EXPOSE()

//==============================================================================
MetaGroupObj::MetaGroupObj(const wchar_t* groupName)
    : groupName_(groupName), groupNameHash_(HashUtilities::compute(groupName_))
{
}

//==============================================================================
const wchar_t* MetaGroupObj::getGroupName() const
{
	return groupName_;
}

//==============================================================================
uint64_t MetaGroupObj::getGroupNameHash() const
{
	return groupNameHash_;
}

//==============================================================================
BEGIN_EXPOSE(MetaAttributeDisplayNameObj, MetaBase, MetaNone())
EXPOSE("attributeName", getAttributeName)
END_EXPOSE()

//==============================================================================
MetaAttributeDisplayNameObj::MetaAttributeDisplayNameObj(const char* attributeName) : attributeName_(attributeName)
{
}

//==============================================================================
const char* MetaAttributeDisplayNameObj::getAttributeName() const
{
	return attributeName_;
}

//==============================================================================
BEGIN_EXPOSE(MetaDisplayNameObj, MetaBase, MetaNone())
EXPOSE("displayName", displayName_)
END_EXPOSE()

//==============================================================================
MetaDisplayNameObj::MetaDisplayNameObj(const wchar_t* displayName) : displayName_(displayName)
{
}

//==============================================================================
const wchar_t* MetaDisplayNameObj::getDisplayName(const ObjectHandle& /*handle*/) const
{
	return displayName_;
}

//==============================================================================
BEGIN_EXPOSE(MetaDisplayNameCallbackObj, MetaDisplayNameObj, MetaNone())
END_EXPOSE()

const wchar_t* MetaDisplayNameCallbackObj::getDisplayName(const ObjectHandle& handle) const
{
	if (callback_)
	{
		return callback_(handle);
	}

	return displayName_;
}

//==============================================================================
BEGIN_EXPOSE(MetaDescriptionObj, MetaBase, MetaNone())
EXPOSE("description", getDescription)
END_EXPOSE()

//==============================================================================
MetaDescriptionObj::MetaDescriptionObj(const wchar_t* description) : description_(description)
{
}

//==============================================================================
const wchar_t* MetaDescriptionObj::getDescription() const
{
	return description_;
}

//==============================================================================
BEGIN_EXPOSE(MetaPanelLayoutObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaNoNullObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaColorObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaHiddenObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaThumbnailObj, MetaBase, MetaNone())
EXPOSE("width", getWidth)
EXPOSE("height", getHeight)
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaInPlaceObj, MetaBase, MetaNone())
EXPOSE("propName", getPropName)
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaSelectedObj, MetaBase, MetaNone())
EXPOSE("propName", getPropName)
END_EXPOSE()

//==============================================================================
MetaActionObj::MetaActionObj()
{
}

//==============================================================================
void MetaActionObj::execute(ObjectHandle handle)
{
	if (action_)
	{
		action_(handle);
	}
}

//==============================================================================
BEGIN_EXPOSE(MetaActionObj, MetaBase, MetaNone())
EXPOSE("actionName", getActionName)
EXPOSE_METHOD("execute", execute)
END_EXPOSE()

//==============================================================================
MetaCommandObj::MetaCommandObj() : commandName_(nullptr), commandExecutable_(nullptr)
{
}

//==============================================================================
MetaCommandObj::~MetaCommandObj()
{
	delete commandExecutable_;
}

//==============================================================================
void MetaCommandObj::execute(void* pBase, void* arguments) const
{
	commandExecutable_->execute(pBase, arguments);
}

//==============================================================================
BEGIN_EXPOSE(MetaCommandObj, MetaBase, MetaNone())
EXPOSE("commandName", getCommandName)
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaNoSerializationObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
const char* MetaUniqueIdObj::getId() const
{
	return id_;
}

BEGIN_EXPOSE(MetaUniqueIdObj, MetaBase, MetaNone())
EXPOSE("id", getId)
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaOnStackObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
const char* MetaInPlacePropertyNameObj::getPropertyName() const
{
	return propName_;
}

//==============================================================================
BEGIN_EXPOSE(MetaInPlacePropertyNameObj, MetaBase, MetaNone())
EXPOSE("propertyName", getPropertyName)
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaReadOnlyObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
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

//==============================================================================
bool MetaUrlObj::native() const
{
	return native_;
}

//==============================================================================
const std::string& MetaUrlObj::title() const
{
	return title_;
}

//==============================================================================
const std::string& MetaUrlObj::folder() const
{
	return folder_;
}

//==============================================================================
const std::string& MetaUrlObj::nameFilters() const
{
	return nameFilters_;
}

//==============================================================================
const std::string& MetaUrlObj::selectedNameFilter() const
{
	return selectedNameFilter_;
}

//==============================================================================
bool MetaUrlObj::isAssetBrowserDialog() const
{
	return !native();
}

//==============================================================================
const char* MetaUrlObj::getDialogTitle() const
{
	return title().c_str();
}

//==============================================================================
const char* MetaUrlObj::getDialogDefaultFolder() const
{
	return folder().c_str();
}

//==============================================================================
const char* MetaUrlObj::getDialogNameFilters() const
{
	return nameFilters().c_str();
}

//==============================================================================
const char* MetaUrlObj::getDialogSelectedNameFilter() const
{
	return selectedNameFilter().c_str();
}

//==============================================================================
BEGIN_EXPOSE(MetaUrlObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaPasswordObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaMultilineObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaDirectInvokeObj, MetaBase, MetaNone())
END_EXPOSE()

//==============================================================================
BEGIN_EXPOSE(MetaOnPropertyChangedObj, MetaBase, MetaNone())
EXPOSE_METHOD("onPropertyChanged", onPropertyChanged)
END_EXPOSE()

} // end namespace wgt
