#include "reflected_property_item.hpp"
#include "reflected_tree_model.hpp"

#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_data_model/reflection/reflected_collection.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "../reflection/reflected_enum_model.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "../reflection/class_definition_model.hpp"
#include "wg_types/binary_block.hpp"
#include "../file_system/file_system_model.hpp"

namespace wgt
{
ITEMROLE(key)
ITEMROLE(keyType)
ITEMROLE(isCollection)
ITEMROLE(elementValueType)
ITEMROLE(elementKeyType)
ITEMROLE(readOnly)
ITEMROLE(enabled)
ITEMROLE(multipleValues)
ITEMROLE(assetModel)
ITEMROLE(name)

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
	{
		return MaxMinValuePair(std::numeric_limits<int8_t>::lowest(), std::numeric_limits<int8_t>::max());
	}

	if (int16Type == tid)
	{
		return MaxMinValuePair(std::numeric_limits<int16_t>::lowest(), std::numeric_limits<int16_t>::max());
	}

	if (int32Type == tid)
	{
		return MaxMinValuePair(std::numeric_limits<int32_t>::lowest(), std::numeric_limits<int32_t>::max());
	}

	if (int64Type == tid)
	{
		return MaxMinValuePair(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());
	}

	if (uint8Type == tid)
	{
		return MaxMinValuePair(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
	}

	if (uint16Type == tid)
	{
		return MaxMinValuePair(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
	}

	if (uint32Type == tid)
	{
		return MaxMinValuePair(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint32_t>::max());
	}

	if (uint64Type == tid)
	{
		return MaxMinValuePair(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint64_t>::max());
	}

	if (longType == tid)
	{
		return MaxMinValuePair(std::numeric_limits<long>::lowest(), std::numeric_limits<long>::max());
	}

	if (ulongType == tid)
	{
		return MaxMinValuePair(std::numeric_limits<unsigned long>::min(), std::numeric_limits<unsigned long>::max());
	}

	if (floatType == tid)
	{
		return MaxMinValuePair(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
	}

	if (doubleType == tid)
	{
		return MaxMinValuePair(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	}

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
namespace proto
{
ReflectedPropertyItem::ReflectedPropertyItem(const ReflectedTreeModel& model, const ObjectHandle& object,
                                             const std::string& path, const std::string& fullpath)
    : model_(model), object_(object), path_(path), fullPath_(fullpath)
{
}

ReflectedPropertyItem::~ReflectedPropertyItem()
{
}

const ObjectHandle& ReflectedPropertyItem::getObject() const
{
	return object_;
}

const std::string& ReflectedPropertyItem::getPath() const
{
	return path_;
}

const std::string& ReflectedPropertyItem::getFullPath() const
{
	return fullPath_;
}

void ReflectedPropertyItem::setPath(const std::string& path)
{
	path_ = path;
}

Variant ReflectedPropertyItem::getData(int column, ItemRole::Id roleId) const
{
	auto pDefinitionManager = model_.getDefinitionManager();
	if (pDefinitionManager == nullptr)
	{
		return Variant();
	}

	auto propertyAccessor = object_.getDefinition(*pDefinitionManager)->bindProperty(path_.c_str(), object_);

	if (roleId == ItemRole::displayId)
	{
		switch (column)
		{
		case 0:
		{
			auto displayName = findFirstMetaData<MetaDisplayNameObj>(propertyAccessor, *pDefinitionManager);
			if (displayName != nullptr)
			{
				return displayName->getDisplayName(propertyAccessor.getObject());
			}
			return path_.c_str();
		}
		default:
			return "Reflected Property";
		}
	}
	else if (roleId == ItemRole::valueId || roleId == ValueRole::roleId_)
	{
		if (!propertyAccessor.canGetValue())
		{
			return Variant();
		}
		auto value = propertyAccessor.getValue();
		if (value.canCast<Collection>())
		{
			value = Collection(std::make_shared<ReflectedCollection>(propertyAccessor, model_.getController()));
		}
		return value;
	}
	else if (roleId == ItemRole::valueTypeId || roleId == ValueTypeRole::roleId_)
	{
		return propertyAccessor.getType().getName();
	}
	else if (roleId == ItemRole::keyId || roleId == KeyRole::roleId_)
	{
		auto index = model_.index(this);
		auto parent = index.parent_;
		if (parent == nullptr)
		{
			return Variant();
		}

		Collection collection;
		const bool parentIsCollection = parent->getData(-1, 0, ItemRole::valueId).tryCast(collection);
		if (!parentIsCollection)
		{
			return Variant();
		}

		auto row = model_.index(this).row_;

		int i = 0;
		auto it = collection.begin();

		for (; i < row && it != collection.end(); ++it)
		{
			++i;
		}

		if (it == collection.end())
		{
			return Variant();
		}

		return it.key();
	}
	else if (roleId == ItemRole::keyTypeId || roleId == KeyTypeRole::roleId_)
	{
		auto index = model_.index(this);
		auto parent = index.parent_;
		if (parent == nullptr)
		{
			return Variant();
		}

		Collection collection;
		const bool parentIsCollection = parent->getData(-1, 0, ItemRole::valueId).tryCast(collection);
		if (!parentIsCollection)
		{
			return Variant();
		}

		auto row = model_.index(this).row_;

		int i = 0;
		auto it = collection.begin();

		for (; i < row && it != collection.end(); ++it)
		{
			++i;
		}

		if (it == collection.end())
		{
			return Variant();
		}

		return it.keyType().getName();
	}
	else if (roleId == ItemRole::isCollectionId)
	{
		return propertyAccessor.getValue().canCast<Collection>();
	}
	else if (roleId == ItemRole::elementValueTypeId)
	{
		Collection collection;
		const bool isCollection = propertyAccessor.getValue().tryCast(collection);
		if (!isCollection)
		{
			return Variant();
		}
		return collection.valueType().getName();
	}
	else if (roleId == ItemRole::elementKeyTypeId)
	{
		Collection collection;
		const bool isCollection = propertyAccessor.getValue().tryCast(collection);
		if (!isCollection)
		{
			return Variant();
		}
		return collection.keyType().getName();
	}
	else if (roleId == ItemRole::itemIdId)
	{
		return Variant();
	}
	else if (roleId == ItemRole::assetModelId)
	{
		auto assetManager = model_.getAssetManager();
		if (assetManager == nullptr)
		{
			return Variant();
		}
		return ObjectHandle(assetManager->assetModel());
	}
	else if (roleId == ItemRole::nameId)
	{
		return propertyAccessor.getName();
	}

	if (roleId == ObjectRole::roleId_)
	{
		return getObject();
	}
	else if (roleId == RootObjectRole::roleId_)
	{
		return getObject();
	}
	else if (roleId == IsEnumRole::roleId_)
	{
		return findFirstMetaData<MetaEnumObj>(propertyAccessor, *pDefinitionManager) != nullptr;
	}
	else if (roleId == IsThumbnailRole::roleId_)
	{
		return findFirstMetaData<MetaThumbnailObj>(propertyAccessor, *pDefinitionManager) != nullptr;
	}
	else if (roleId == IsSliderRole::roleId_)
	{
		return findFirstMetaData<MetaSliderObj>(propertyAccessor, *pDefinitionManager) != nullptr;
	}
	else if (roleId == IsColorRole::roleId_)
	{
		return findFirstMetaData<MetaColorObj>(propertyAccessor, *pDefinitionManager) != nullptr;
	}
	else if (roleId == IsActionRole::roleId_)
	{
		return findFirstMetaData<MetaActionObj>(propertyAccessor, *pDefinitionManager) != nullptr;
	}
	else if (roleId == DescriptionRole::roleId_)
	{
		auto descObj = findFirstMetaData<MetaDescriptionObj>(propertyAccessor, *pDefinitionManager);
		if (descObj != nullptr)
		{
			return descObj->getDescription();
		}
		return nullptr;
	}
	else if (roleId == IsUrlRole::roleId_)
	{
		return findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager) != nullptr;
	}
	else if (roleId == ThumbnailRole::roleId_)
	{
		if (findFirstMetaData<MetaThumbnailObj>(propertyAccessor, *pDefinitionManager) == nullptr)
		{
			return Variant();
		}

		// Should not have a MetaThumbObj for properties that do not have a value
		assert(propertyAccessor.canGetValue());

		typedef std::shared_ptr<BinaryBlock> ThumbnailData;
		const Variant value = propertyAccessor.getValue();
		if (value.canCast<ThumbnailData>())
		{
			return value;
		}
		return Variant();
	}
	else if (roleId == MinValueRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		Variant variant = getMinValue(typeId);
		auto minMaxObj = findFirstMetaData<MetaMinMaxObj>(propertyAccessor, *pDefinitionManager);
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
		auto minMaxObj = findFirstMetaData<MetaMinMaxObj>(propertyAccessor, *pDefinitionManager);
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
		auto stepSize = findFirstMetaData<MetaStepSizeObj>(propertyAccessor, *pDefinitionManager);
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
		auto decimals = findFirstMetaData<MetaDecimalsObj>(propertyAccessor, *pDefinitionManager);
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
		auto enumObj = findFirstMetaData<MetaEnumObj>(propertyAccessor, *pDefinitionManager);
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
		provider = reflectedRoot(provider, *pDefinitionManager);
		auto definition =
		const_cast<IClassDefinition*>(provider.isValid() ? provider.getDefinition(*pDefinitionManager) : nullptr);
		return ObjectHandle(definition);
	}
	else if (roleId == DefinitionModelRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		if (typeId.isPointer())
		{
			auto definition = pDefinitionManager->getDefinition(typeId.removePointer().getName());
			if (definition != nullptr)
			{
				auto definitionModel =
				std::unique_ptr<IListModel>(new ClassDefinitionModel(definition, *pDefinitionManager));
				return std::move(definitionModel);
			}
		}
	}
	else if (roleId == UrlIsAssetBrowserRole::roleId_)
	{
		bool isAssetBrowserDlg = false;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager);
		if (urlObj != nullptr)
		{
			isAssetBrowserDlg = urlObj->isAssetBrowserDialog();
		}
		return isAssetBrowserDlg;
	}
	else if (roleId == UrlDialogTitleRole::roleId_)
	{
		const char* title = "";
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager);
		if (urlObj != nullptr)
		{
			title = urlObj->getDialogTitle();
		}
		return title;
	}
	else if (roleId == UrlDialogDefaultFolderRole::roleId_)
	{
		const char* folder = "";
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager);
		if (urlObj != nullptr)
		{
			folder = urlObj->getDialogDefaultFolder();
		}
		return folder;
	}
	else if (roleId == UrlDialogNameFiltersRole::roleId_)
	{
		const char* nameFilters = "";
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager);
		if (urlObj != nullptr)
		{
			nameFilters = urlObj->getDialogNameFilters();
		}
		return nameFilters;
	}
	else if (roleId == UrlDialogSelectedNameFilterRole::roleId_)
	{
		const char* selectedFilter = "";
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager);
		if (urlObj != nullptr)
		{
			selectedFilter = urlObj->getDialogSelectedNameFilter();
		}
		return selectedFilter;
	}
	else if (roleId == ItemRole::readOnlyId)
	{
		TypeId typeId = propertyAccessor.getType();
		auto readonly = findFirstMetaData<MetaReadOnlyObj>(propertyAccessor, *pDefinitionManager);
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
	auto controller = model_.getController();
	if (controller == nullptr)
	{
		return false;
	}
	auto pDefinitionManager = model_.getDefinitionManager();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto propertyAccessor = obj.getDefinition(*pDefinitionManager)->bindProperty(path_.c_str(), obj);

	if (roleId == ValueRole::roleId_)
	{
		if (!propertyAccessor.canSetValue())
		{
			return false;
		}

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

		auto baseDefinition = pDefinitionManager->getDefinition(typeId.removePointer().getName());
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
}
}