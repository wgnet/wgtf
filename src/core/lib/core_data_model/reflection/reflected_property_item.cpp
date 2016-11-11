#include "reflected_property_item.hpp"
#include "reflected_object_item.hpp"
#include "reflected_enum_model.hpp"
#include "class_definition_model.hpp"

#include "core_data_model/generic_tree_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"
#include <memory>
#include <codecvt>
#include <limits>

namespace wgt
{
ITEMROLE(readOnly)
ITEMROLE(enabled)
ITEMROLE(multipleValues)

namespace
{
struct MaxMinValuePair
{
	MaxMinValuePair(const Variant& min, const Variant& max) : minValue_(min), maxValue_(max)
	{
	}

	Variant minValue_;
	Variant maxValue_;
};

typedef std::unordered_map<const TypeId, MaxMinValuePair> MaxMinValuePairMap;

MaxMinValuePair getValuePair(const TypeId& tid)
{
	static const TypeId int8Type = TypeId::getType<int8_t>();
	static const TypeId int16Type = TypeId::getType<int16_t>();
	static const TypeId int32Type = TypeId::getType<int32_t>();
	static const TypeId int64Type = TypeId::getType<int64_t>();
	static const TypeId uint8Type = TypeId::getType<uint8_t>();
	static const TypeId uint16Type = TypeId::getType<uint16_t>();
	static const TypeId uint32Type = TypeId::getType<uint32_t>();
	static const TypeId uint64Type = TypeId::getType<uint64_t>();
	static const TypeId longType = TypeId::getType<long>();
	static const TypeId ulongType = TypeId::getType<unsigned long>();
	static const TypeId floatType = TypeId::getType<float>();
	static const TypeId doubleType = TypeId::getType<double>();

	if (int8Type == tid)
		return MaxMinValuePair(std::numeric_limits<int8_t>::lowest(), std::numeric_limits<int8_t>::max());

	if (int16Type == tid)
		return MaxMinValuePair(std::numeric_limits<int16_t>::lowest(), std::numeric_limits<int16_t>::max());

	if (int32Type == tid)
		return MaxMinValuePair(std::numeric_limits<int32_t>::lowest(), std::numeric_limits<int32_t>::max());

	if (int64Type == tid)
		return MaxMinValuePair(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());

	if (uint8Type == tid)
		return MaxMinValuePair(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());

	if (uint16Type == tid)
		return MaxMinValuePair(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());

	if (uint32Type == tid)
		return MaxMinValuePair(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint32_t>::max());

	if (uint64Type == tid)
		return MaxMinValuePair(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint64_t>::max());

	if (longType == tid)
		return MaxMinValuePair(std::numeric_limits<long>::lowest(), std::numeric_limits<long>::max());

	if (ulongType == tid)
		return MaxMinValuePair(std::numeric_limits<unsigned long>::min(), std::numeric_limits<unsigned long>::max());

	if (floatType == tid)
		return MaxMinValuePair(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());

	if (doubleType == tid)
		return MaxMinValuePair(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());

	return MaxMinValuePair(Variant(), Variant());
}

Variant getMaxValue(const TypeId& typeId)
{
	return getValuePair(typeId).maxValue_;
}

Variant getMinValue(const TypeId& typeId)
{
	return getValuePair(typeId).minValue_;
}
}

ReflectedPropertyItem::ReflectedPropertyItem(const IBasePropertyPtr& property, ReflectedItem* parent,
                                             const std::string& inplacePath)
    : ReflectedItem(parent, std::string(inplacePath) + property->getName())
{
	// Must have a parent
	assert(parent != nullptr);
	assert(!path_.empty());
}

ReflectedPropertyItem::ReflectedPropertyItem(const std::string& propertyName, std::string displayName,
                                             ReflectedItem* parent)
    : ReflectedItem(parent, parent ? parent->getPath() + propertyName : ""), displayName_(std::move(displayName))
{
	// Must have a parent
	assert(parent != nullptr);
	assert(!path_.empty());
}

ReflectedPropertyItem::~ReflectedPropertyItem()
{
}

const char* ReflectedPropertyItem::getDisplayText(int column) const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition(*getDefinitionManager())->bindProperty(path_.c_str(), obj);

	auto displayName = findFirstMetaData<MetaDisplayNameObj>(propertyAccessor, *getDefinitionManager());
	if (displayName == nullptr)
	{
		displayName_ = propertyAccessor.getName();
	}
	else
	{
		std::wstring_convert<Utf16to8Facet> conversion(Utf16to8Facet::create());
		displayName_ = conversion.to_bytes(displayName->getDisplayName(propertyAccessor.getObject()));
	}

	return displayName_.c_str();
}

ThumbnailData ReflectedPropertyItem::getThumbnail(int column) const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition(*getDefinitionManager())->bindProperty(path_.c_str(), obj);

	if (findFirstMetaData<MetaThumbnailObj>(propertyAccessor, *getDefinitionManager()) == nullptr)
	{
		return nullptr;
	}

	// Should not have a MetaThumbObj for properties that do not have a value
	assert(propertyAccessor.canGetValue());

	ThumbnailData thumbnail;
	Variant value = propertyAccessor.getValue();
	bool ok = false;
	ok = value.tryCast(thumbnail);
	assert(ok);
	return thumbnail;
}

Variant ReflectedPropertyItem::getData(int column, ItemRole::Id roleId) const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition(*getDefinitionManager())->bindProperty(path_.c_str(), obj);

	if (roleId == IndexPathRole::roleId_)
	{
		return this->getPath();
	}
	else if (roleId == NameRole::roleId_)
	{
		return propertyAccessor.getName();
	}
	else if (roleId == ObjectRole::roleId_)
	{
		return getObject();
	}
	else if (roleId == RootObjectRole::roleId_)
	{
		return getRootObject();
	}
	else if (roleId == ValueTypeRole::roleId_)
	{
		return propertyAccessor.getType().getName();
	}
	else if (roleId == DescriptionRole::roleId_)
	{
		auto descObj = findFirstMetaData<MetaDescriptionObj>(propertyAccessor, *getDefinitionManager());
		if (descObj != nullptr)
		{
			return descObj->getDescription();
		}
		return nullptr;
	}
	else if (roleId == IsEnumRole::roleId_)
	{
		return findFirstMetaData<MetaEnumObj>(propertyAccessor, *getDefinitionManager()) != nullptr;
	}
	else if (roleId == IsThumbnailRole::roleId_)
	{
		return findFirstMetaData<MetaThumbnailObj>(propertyAccessor, *getDefinitionManager()) != nullptr;
	}
	else if (roleId == IsSliderRole::roleId_)
	{
		return findFirstMetaData<MetaSliderObj>(propertyAccessor, *getDefinitionManager()) != nullptr;
	}
	else if (roleId == IsColorRole::roleId_)
	{
		return findFirstMetaData<MetaColorObj>(propertyAccessor, *getDefinitionManager()) != nullptr;
	}
	else if (roleId == IsActionRole::roleId_)
	{
		return findFirstMetaData<MetaActionObj>(propertyAccessor, *getDefinitionManager()) != nullptr;
	}
	else if (roleId == IsUrlRole::roleId_)
	{
		return findFirstMetaData<MetaUrlObj>(propertyAccessor, *getDefinitionManager()) != nullptr;
	}
	else if (roleId == ValueRole::roleId_)
	{
		if (!propertyAccessor.canGetValue())
		{
			return Variant();
		}
		return propertyAccessor.getValue();
	}
	else if (roleId == MinValueRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		Variant variant = getMinValue(typeId);
		auto minMaxObj = findFirstMetaData<MetaMinMaxObj>(propertyAccessor, *getDefinitionManager());
		if (minMaxObj != nullptr)
		{
			const float& value = minMaxObj->getMin();
			float minValue = .0f;
			bool isOk = variant.tryCast(minValue);
			if (isOk)
			{
				float diff = minValue - value;
				float epsilon = std::numeric_limits<float>::epsilon();
				if (diff > epsilon)
				{
					NGT_ERROR_MSG("Property %s: MetaMinMaxObj min value exceeded limits.\n", path_.c_str());
					return variant;
				}
			}
			return value;
		}
		else
		{
			return variant;
		}
	}
	else if (roleId == MaxValueRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		Variant variant = getMaxValue(typeId);
		auto minMaxObj = findFirstMetaData<MetaMinMaxObj>(propertyAccessor, *getDefinitionManager());
		if (minMaxObj != nullptr)
		{
			const float& value = minMaxObj->getMax();
			float maxValue = .0f;
			bool isOk = variant.tryCast(maxValue);
			if (isOk)
			{
				float diff = value - maxValue;
				float epsilon = std::numeric_limits<float>::epsilon();
				if (diff > epsilon)
				{
					NGT_ERROR_MSG("Property %s: MetaMinMaxObj max value exceeded limits.\n", path_.c_str());
					return variant;
				}
			}
			return value;
		}
		else
		{
			return variant;
		}
	}
	else if (roleId == StepSizeRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		auto stepSize = findFirstMetaData<MetaStepSizeObj>(propertyAccessor, *getDefinitionManager());
		if (stepSize != nullptr)
		{
			return stepSize->getStepSize();
		}
		else
		{
			return MetaStepSizeObj::DefaultStepSize;
		}
	}
	else if (roleId == DecimalsRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		auto decimals = findFirstMetaData<MetaDecimalsObj>(propertyAccessor, *getDefinitionManager());
		if (decimals != nullptr)
		{
			return decimals->getDecimals();
		}
		else
		{
			return MetaDecimalsObj::DefaultDecimals;
		}
	}
	else if (roleId == EnumModelRole::roleId_)
	{
		auto enumObj = findFirstMetaData<MetaEnumObj>(propertyAccessor, *getDefinitionManager());
		if (enumObj)
		{
			if (getObject().isValid() == false)
			{
				return Variant();
			}
			auto enumModel = std::unique_ptr<IListModel>(new ReflectedEnumModel(propertyAccessor, enumObj));
			return std::move(enumModel);
		}
	}
	else if (roleId == DefinitionRole::roleId_)
	{
		if (!propertyAccessor.canGetValue())
		{
			return Variant();
		}

		auto variant = propertyAccessor.getValue();
		ObjectHandle provider;
		variant.tryCast(provider);
		provider = reflectedRoot(provider, *getDefinitionManager());
		auto definition =
		const_cast<IClassDefinition*>(provider.isValid() ? provider.getDefinition(*getDefinitionManager()) : nullptr);
		return ObjectHandle(definition);
	}
	else if (roleId == DefinitionModelRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		if (typeId.isPointer())
		{
			auto definition = getDefinitionManager()->getDefinition(typeId.removePointer().getName());
			if (definition != nullptr)
			{
				auto definitionModel =
				std::unique_ptr<IListModel>(new ClassDefinitionModel(definition, *getDefinitionManager()));
				return std::move(definitionModel);
			}
		}
	}
	else if (roleId == UrlIsAssetBrowserRole::roleId_)
	{
		bool isAssetBrowserDlg = false;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *getDefinitionManager());
		if (urlObj != nullptr)
		{
			isAssetBrowserDlg = urlObj->isAssetBrowserDialog();
		}
		return isAssetBrowserDlg;
	}
	else if (roleId == UrlDialogTitleRole::roleId_)
	{
		const char* title = nullptr;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *getDefinitionManager());
		if (urlObj != nullptr)
		{
			title = urlObj->getDialogTitle();
		}
		return title;
	}
	else if (roleId == UrlDialogDefaultFolderRole::roleId_)
	{
		const char* folder = nullptr;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *getDefinitionManager());
		if (urlObj != nullptr)
		{
			folder = urlObj->getDialogDefaultFolder();
		}
		return folder;
	}
	else if (roleId == UrlDialogNameFiltersRole::roleId_)
	{
		const char* nameFilters = nullptr;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *getDefinitionManager());
		if (urlObj != nullptr)
		{
			nameFilters = urlObj->getDialogNameFilters();
		}
		return nameFilters;
	}
	else if (roleId == UrlDialogSelectedNameFilterRole::roleId_)
	{
		const char* selectedFilter = nullptr;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *getDefinitionManager());
		if (urlObj != nullptr)
		{
			selectedFilter = urlObj->getDialogSelectedNameFilter();
		}
		return selectedFilter;
	}
	else if (roleId == ItemRole::readOnlyId)
	{
		TypeId typeId = propertyAccessor.getType();
		auto readonly = findFirstMetaData<MetaReadOnlyObj>(propertyAccessor, *getDefinitionManager());
		if (readonly)
		{
			return true;
		}
		return false;
	}
	else if (roleId == ItemRole::enabledId)
	{
		return Variant(true);
	}
	else if (roleId == ItemRole::multipleValuesId)
	{
		return Variant(false);
	}

	return Variant();
}

bool ReflectedPropertyItem::setData(int column, ItemRole::Id roleId, const Variant& data)
{
	auto controller = getController();
	if (controller == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition(*getDefinitionManager())->bindProperty(path_.c_str(), obj);

	if (roleId == ValueRole::roleId_)
	{
		controller->setValue(propertyAccessor, data);
		return true;
	}
	else if (roleId == DefinitionRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		if (!typeId.isPointer())
		{
			return false;
		}

		auto baseDefinition = getDefinitionManager()->getDefinition(typeId.removePointer().getName());
		if (baseDefinition == nullptr)
		{
			return false;
		}

		ObjectHandle provider;
		if (!data.tryCast<ObjectHandle>(provider))
		{
			return false;
		}

		auto valueDefinition = provider.getBase<IClassDefinition>();
		if (valueDefinition == nullptr)
		{
			return false;
		}

		ObjectHandle value;
		value = valueDefinition->create();
		controller->setValue(propertyAccessor, value);
		return true;
	}
	return false;
}

GenericTreeItem* ReflectedPropertyItem::getChild(size_t index) const
{
	if (children_.size() <= index)
	{
		children_.reserve(index + 1);
		while (children_.size() <= index)
		{
			children_.emplace_back(nullptr);
		}
	}

	auto child = children_[index].get();
	if (child != NULL)
	{
		return child;
	}

	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition(*getDefinitionManager())->bindProperty(path_.c_str(), obj);

	if (!propertyAccessor.canGetValue())
	{
		return nullptr;
	}

	Collection collection;
	bool isCollection = propertyAccessor.getValue().tryCast(collection);
	if (isCollection)
	{
		size_t i = 0;
		auto it = collection.begin();

		for (; i < index && it != collection.end(); ++it)
		{
			++i;
		}

		if (it == collection.end())
		{
			return nullptr;
		}

		{
			// FIXME NGT-1603: Change to actually get the proper key type

			// Attempt to use an index into the collection
			// Defaults to i
			size_t indexKey = i;
			const bool isIndex = it.key().tryCast(indexKey);

			// Default to using an index
			std::string propertyName = "[" + std::to_string(static_cast<int>(indexKey)) + "]";
			std::string displayName = propertyName;

			// If the item isn't an index
			if (!isIndex)
			{
				// Try to cast the key to a string
				const bool isString = it.key().tryCast(displayName);
				if (isString)
				{
					// Strings must be quoted to work with TextStream
					propertyName = "[\"" + displayName + "\"]";
				}
			}

			child =
			new ReflectedPropertyItem(propertyName, std::move(displayName), const_cast<ReflectedPropertyItem*>(this));
		}
		children_[index] = std::unique_ptr<ReflectedItem>(child);
		return child;
	}

	auto value = propertyAccessor.getValue();
	ObjectHandle baseProvider;
	value.tryCast(baseProvider);
	if (!baseProvider.isValid())
	{
		return nullptr;
	}
	baseProvider = reflectedRoot(baseProvider, *getDefinitionManager());
	child = new ReflectedObjectItem(baseProvider, const_cast<ReflectedPropertyItem*>(this));
	child->hidden(true);
	children_[index] = std::unique_ptr<ReflectedItem>(child);
	return child;
}

bool ReflectedPropertyItem::empty() const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition(*getDefinitionManager())->bindProperty(path_.c_str(), obj);

	if (!propertyAccessor.canGetValue())
	{
		return true;
	}

	Variant value = propertyAccessor.getValue();
	const bool isCollection = value.typeIs<Collection>();

	if (isCollection)
	{
		const Collection& collection = value.cast<const Collection&>();
		return collection.empty();
	}

	ObjectHandle handle;
	bool isObjectHandle = value.tryCast(handle);
	if (isObjectHandle)
	{
		handle = reflectedRoot(handle, *getDefinitionManager());
		auto def = handle.getDefinition(*getDefinitionManager());

		if (def != nullptr)
		{
			PropertyIteratorRange range = def->allProperties();
			return range.begin() == range.end();
		}
	}

	return true;
}

size_t ReflectedPropertyItem::size() const
{
	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition(*getDefinitionManager())->bindProperty(path_.c_str(), obj);

	if (!propertyAccessor.canGetValue())
	{
		return 0;
	}

	Collection collection;
	const Variant& value = propertyAccessor.getValue();
	bool isCollection = value.tryCast(collection);
	if (isCollection)
	{
		return collection.size();
	}

	ObjectHandle handle;
	bool isObjectHandle = value.tryCast(handle);
	if (isObjectHandle)
	{
		handle = reflectedRoot(handle, *getDefinitionManager());
		auto def = handle.getDefinition(*getDefinitionManager());
		if (def != nullptr)
		{
			return 1;
		}
	}

	return 0;
}

//==============================================================================
bool ReflectedPropertyItem::preSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	auto obj = getObject();
	auto otherObj = accessor.getObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		TypeId typeId = accessor.getType();
		bool isReflectedObject =
		typeId.isPointer() && getDefinitionManager()->getDefinition(typeId.removePointer().getName()) != nullptr;
		if (isReflectedObject)
		{
			const IClassDefinition* definition = nullptr;
			ObjectHandle handle;
			if (value.tryCast(handle))
			{
				definition = handle.getDefinition(*getDefinitionManager());
			}
			getModel()->signalPreItemDataChanged(this, 0, DefinitionRole::roleId_, ObjectHandle(definition));
			return true;
		}

		getModel()->signalPreItemDataChanged(this, 0, ValueRole::roleId_, value);
		return true;
	}

	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preSetValue(accessor, value))
		{
			return true;
		}
	}
	return false;
}

//==============================================================================
bool ReflectedPropertyItem::postSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	auto obj = getObject();
	auto otherObj = accessor.getObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		TypeId typeId = accessor.getType();
		bool isReflectedObject =
		typeId.isPointer() && getDefinitionManager()->getDefinition(typeId.removePointer().getName()) != nullptr;
		if (isReflectedObject)
		{
			const IClassDefinition* definition = nullptr;
			ObjectHandle handle;
			if (value.tryCast(handle))
			{
				definition = handle.getDefinition(*getDefinitionManager());
			}
			children_.clear();
			getModel()->signalPostItemDataChanged(this, 0, DefinitionRole::roleId_, ObjectHandle(definition));
			return true;
		}

		getModel()->signalPostItemDataChanged(this, 0, ValueRole::roleId_, value);
		return true;
	}

	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postSetValue(accessor, value))
		{
			return true;
		}
	}
	return false;
}
} // end namespace wgt
