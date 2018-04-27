#include "reflected_property_item_new.hpp"

#include "reflected_collection.hpp"

#include "class_definition_model_new.hpp"
#include "reflected_enum_model_new.hpp"
#include "reflected_object_item_new.hpp"
#include "reflected_tree_model_new.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"

#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"

#include "wg_types/binary_block.hpp"

#include <memory>
#include <codecvt>
#include <limits>

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

class ReflectedPropertyItemNew::Implementation : public Depends<IDefinitionManager, IReflectionController>
{
public:
	std::string displayName_;
	std::vector<std::unique_ptr<ReflectedTreeItemNew>> children_;
	std::unique_ptr<AbstractListModel> enumModel_;
	std::unique_ptr<AbstractListModel> definitionModel_;
};

ReflectedPropertyItemNew::ReflectedPropertyItemNew(const IBasePropertyPtr& property, ReflectedTreeItemNew* parent,
                                                   size_t index, const std::string& inPlacePath)
    : ReflectedTreeItemNew(parent, index, std::string(inPlacePath) + property->getName()), impl_(new Implementation)
{
	// Must have a parent
	TF_ASSERT(parent != nullptr);
	TF_ASSERT(!path_.empty());
}

ReflectedPropertyItemNew::ReflectedPropertyItemNew(const std::string& propertyName, std::string displayName,
                                                   ReflectedTreeItemNew* parent, size_t index)
    : ReflectedTreeItemNew(parent, index, parent ? parent->getPath() + propertyName : ""), impl_(new Implementation)
{
	impl_->displayName_ = std::move(displayName);

	// Must have a parent
	TF_ASSERT(parent != nullptr);
	TF_ASSERT(!path_.empty());
}

ReflectedPropertyItemNew::~ReflectedPropertyItemNew()
{
}

Variant ReflectedPropertyItemNew::getData(int column, ItemRole::Id roleId) const
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return Variant();
	}

	auto obj = getObject();
	auto propertyAccessor = pDefinitionManager->getDefinition(obj)->bindProperty(path_.c_str(), obj);

	if (roleId == ItemRole::displayId)
	{
		switch (column)
		{
		case 0:
		{
			auto displayName = findFirstMetaData<MetaDisplayNameObj>(propertyAccessor, *pDefinitionManager);
			if (displayName != nullptr)
			{
				impl_->displayName_ = StringUtils::to_string(displayName->getDisplayName(propertyAccessor.getObject()));
			}
			else
			{
				impl_->displayName_ = propertyAccessor.getName();
			}
			return impl_->displayName_.c_str();
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
			value =
			Collection(std::make_shared<ReflectedCollection>(propertyAccessor, impl_->get<IReflectionController>()));
		}
		return value;
	}
	else if (roleId == ItemRole::valueTypeId || roleId == ValueTypeRole::roleId_)
	{
		return propertyAccessor.getType().getName();
	}
	else if (roleId == ItemRole::keyId || roleId == KeyRole::roleId_)
	{
		if (parent_ == nullptr)
		{
			return Variant();
		}

		Collection collection;
		const bool parentIsCollection = parent_->getData(0, ItemRole::valueId).tryCast(collection);
		if (!parentIsCollection)
		{
			return Variant();
		}

		auto index = getModel()->index(this).row_;

		int i = 0;
		auto it = collection.begin();

		for (; i < index && it != collection.end(); ++it)
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
		if (parent_ == nullptr)
		{
			return Variant();
		}

		Collection collection;
		const bool parentIsCollection = parent_->getData(0, ItemRole::valueId).tryCast(collection);
		if (!parentIsCollection)
		{
			return Variant();
		}

		auto index = getModel()->index(this).row_;

		int i = 0;
		auto it = collection.begin();

		for (; i < index && it != collection.end(); ++it)
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
		return getId();
	}

	if (roleId == ObjectRole::roleId_)
	{
		return getObject();
	}
	else if (roleId == RootObjectRole::roleId_)
	{
		return getRootObject();
	}
	else if (roleId == NameRole::roleId_)
	{
		return propertyAccessor.getName();
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
		TF_ASSERT(propertyAccessor.canGetValue());

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
		if (enumObj != nullptr)
		{
			if (getObject().isValid() == false)
			{
				return Variant();
			}

			// Always recreate the enum model to support dynamic enum generation
			// if (!impl_->enumModel_)
			{
				impl_->enumModel_.reset(new ReflectedEnumModelNew(propertyAccessor, enumObj));
			}

			return impl_->enumModel_.get();
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
		const_cast<IClassDefinition*>(provider.isValid() ? pDefinitionManager->getDefinition(provider) : nullptr);
		return definition;
	}
	else if (roleId == DefinitionModelRole::roleId_)
	{
		TypeId typeId = propertyAccessor.getType();
		if (typeId.isPointer())
		{
			auto definition = pDefinitionManager->getDefinition(typeId.removePointer().getName());
			if (definition != nullptr)
			{
				if (!impl_->definitionModel_)
				{
					impl_->definitionModel_.reset(new ClassDefinitionModelNew(definition, *pDefinitionManager));
				}

				return impl_->definitionModel_.get();
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
		const char* title = nullptr;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager);
		if (urlObj != nullptr)
		{
			title = urlObj->getDialogTitle();
		}
		return title;
	}
	else if (roleId == UrlDialogDefaultFolderRole::roleId_)
	{
		const char* folder = nullptr;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager);
		if (urlObj != nullptr)
		{
			folder = urlObj->getDialogDefaultFolder();
		}
		return folder;
	}
	else if (roleId == UrlDialogNameFiltersRole::roleId_)
	{
		const char* nameFilters = nullptr;
		auto urlObj = findFirstMetaData<MetaUrlObj>(propertyAccessor, *pDefinitionManager);
		if (urlObj != nullptr)
		{
			nameFilters = urlObj->getDialogNameFilters();
		}
		return nameFilters;
	}
	else if (roleId == UrlDialogSelectedNameFilterRole::roleId_)
	{
		const char* selectedFilter = nullptr;
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
		if (readonly != nullptr)
		{
			return readonly->isReadOnly(propertyAccessor.getObject());
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
	else if (roleId == ItemRole::tooltipId)
	{
		auto tooltipObj = findFirstMetaData<MetaTooltipObj>(propertyAccessor, *pDefinitionManager);
		if (tooltipObj != nullptr)
		{
			return tooltipObj->getTooltip(propertyAccessor.getObject());
		}
		return nullptr;
	}

	return Variant();
}

bool ReflectedPropertyItemNew::setData(int column, ItemRole::Id roleId, const Variant& data)
{
	auto controller = impl_->get<IReflectionController>();

	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto propertyAccessor = pDefinitionManager->getDefinition(obj)->bindProperty(path_.c_str(), obj);

	if (roleId == ValueRole::roleId_)
	{
		if (controller)
		{
			controller->setValue(propertyAccessor, data);
		}
		else
		{
			propertyAccessor.setValue(data);
		}
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

		IClassDefinition* valueDefinition = nullptr;
		if (!data.tryCast<IClassDefinition*>(valueDefinition))
		{
			return false;
		}

		if (valueDefinition == nullptr)
		{
			return false;
		}

		auto value = valueDefinition->createShared();
		if (controller)
		{
			controller->setValue(propertyAccessor, value);
		}
		else
		{
			propertyAccessor.setValue(value);
		}
		return true;
	}
	return false;
}

const ObjectHandle& ReflectedPropertyItemNew::getRootObject() const /* override */
{
	return parent_->getRootObject();
}

const ObjectHandle& ReflectedPropertyItemNew::getObject() const /* override */
{
	return parent_->getObject();
}

ReflectedTreeItemNew* ReflectedPropertyItemNew::getChild(size_t index) const
{
	if (impl_->children_.size() <= index)
	{
		impl_->children_.reserve(index + 1);
		while (impl_->children_.size() <= index)
		{
			impl_->children_.emplace_back(nullptr);
		}
	}

	auto child = impl_->children_[index].get();
	if (child != nullptr)
	{
		return child;
	}

	auto obj = this->getObject();
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return nullptr;
	}
	auto propertyAccessor = pDefinitionManager->getDefinition(obj)->bindProperty(path_.c_str(), obj);

	if (!propertyAccessor.canGetValue())
	{
		return nullptr;
	}

	Collection collection;
	const bool isCollection = propertyAccessor.getValue().tryCast(collection);
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
			std::string propertyName = Collection::getIndexOpen() + std::to_string(static_cast<int>(indexKey)) + Collection::getIndexClose();
			std::string displayName = propertyName;

			// If the item isn't an index
			if (!isIndex)
			{
				// Try to cast the key to a string
				const bool isString = it.key().tryCast(displayName);
				if (isString)
				{
					propertyName = Collection::getIndexOpenStr() + displayName + Collection::getIndexCloseStr();
				}
			}

			child = new ReflectedPropertyItemNew(propertyName, std::move(displayName),
			                                     const_cast<ReflectedPropertyItemNew*>(this), index);
		}
		impl_->children_[index] = std::unique_ptr<ReflectedTreeItemNew>(child);
		return child;
	}

	auto value = propertyAccessor.getValue();
	ObjectHandle baseProvider;
	value.tryCast(baseProvider);
	if (!baseProvider.isValid())
	{
		return nullptr;
	}
	baseProvider = reflectedRoot(baseProvider, *pDefinitionManager);
	child = new ReflectedObjectItemNew(baseProvider, const_cast<ReflectedPropertyItemNew*>(this), index);
	impl_->children_[index] = std::unique_ptr<ReflectedTreeItemNew>(child);
	return child;
}

int ReflectedPropertyItemNew::rowCount() const
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return 0;
	}
	auto obj = getObject();
	auto propertyAccessor = pDefinitionManager->getDefinition(obj)->bindProperty(path_.c_str(), obj);

	if (!propertyAccessor.canGetValue())
	{
		return 0;
	}

	Collection collection;
	const Variant& value = propertyAccessor.getValue();
	bool isCollection = value.tryCast(collection);
	if (isCollection)
	{
		return static_cast<int>(collection.size());
	}

	ObjectHandle handle;
	bool isObjectHandle = value.tryCast(handle);
	if (isObjectHandle)
	{
		handle = reflectedRoot(handle, *pDefinitionManager);
		auto def = pDefinitionManager->getDefinition(handle);
		if (def != nullptr)
		{
			return 1;
		}
	}

	return 0;
}

bool ReflectedPropertyItemNew::preSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto otherObj = accessor.getObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		TypeId typeId = accessor.getType();
		bool isReflectedObject =
		typeId.isPointer() && pDefinitionManager->getDefinition(typeId.removePointer().getName()) != nullptr;
		if (isReflectedObject)
		{
			const IClassDefinition* definition = nullptr;
			ObjectHandle handle;
			if (value.tryCast(handle))
			{
				definition = pDefinitionManager->getDefinition(handle);
			}

			const auto pModel = this->getModel();
			if (pModel != nullptr)
			{
				const auto index = pModel->index(this);
				const int column = 0;
				const ItemRole::Id roleId = DefinitionRole::roleId_;
				const Variant value = definition;
				pModel->preItemDataChanged_(index, column, roleId, value);
			}
			return true;
		}

		const auto pModel = this->getModel();
		if (pModel != nullptr)
		{
			const auto index = pModel->index(this);
			const int column = 0;
			const ItemRole::Id roleId = ValueRole::roleId_;
			pModel->preItemDataChanged_(index, column, roleId, value);
		}
		return true;
	}

	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
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

bool ReflectedPropertyItemNew::postSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto otherObj = accessor.getObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		TypeId typeId = accessor.getType();
		bool isReflectedObject =
		typeId.isPointer() && pDefinitionManager->getDefinition(typeId.removePointer().getName()) != nullptr;
		if (isReflectedObject)
		{
			const IClassDefinition* definition = nullptr;
			ObjectHandle handle;
			if (value.tryCast(handle))
			{
				definition = pDefinitionManager->getDefinition(handle);
			}
			impl_->children_.clear();

			const auto pModel = this->getModel();
			if (pModel != nullptr)
			{
				const auto index = pModel->index(this);
				const int column = 0;
				const ItemRole::Id roleId = DefinitionRole::roleId_;
				const Variant value = definition;
				pModel->postItemDataChanged_(index, column, roleId, value);
			}
			return true;
		}

		const auto pModel = this->getModel();
		if (pModel != nullptr)
		{
			const auto index = pModel->index(this);
			const int column = 0;
			const ItemRole::Id roleId = ValueRole::roleId_;
			pModel->postItemDataChanged_(index, column, roleId, value);
		}
		return true;
	}

	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
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

bool ReflectedPropertyItemNew::preInsert(const PropertyAccessor& accessor, size_t index, size_t count)
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto otherObj = accessor.getObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		auto model = getModel();
		TF_ASSERT(model != nullptr);
		model->preRowsInserted_(model->index(this), static_cast<int>(index), static_cast<int>(count));
		return true;
	}

	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preInsert(accessor, index, count))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedPropertyItemNew::postInserted(const PropertyAccessor& accessor, size_t index, size_t count)
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto otherObj = accessor.getObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		Collection collection;
		const Variant& value = accessor.getValue();
		bool isCollection = value.tryCast(collection);
		TF_ASSERT(isCollection);

		for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
		{
			auto child = static_cast<ReflectedPropertyItemNew*>(it->get());
			if (child == nullptr)
			{
				continue;
			}

			child->impl_->children_.clear();
		}

		for (size_t i = 0; i < count; ++i)
		{
			impl_->children_.emplace(impl_->children_.begin() + index, nullptr);
		}
		for (auto it = impl_->children_.begin() + index + count; it != impl_->children_.end(); ++it)
		{
			auto& item = *it;
			if (item == nullptr)
			{
				continue;
			}

			item->setIndex(item->getIndex() + count);
		}
		if (!collection.isMapping())
		{
			for (auto i = index + count; i < impl_->children_.size(); ++i)
			{
				auto child = static_cast<ReflectedPropertyItemNew*>(impl_->children_[i].get());
				if (child == nullptr)
				{
					continue;
				}

				auto oldPath = child->path_;
				auto parentPath = oldPath.substr(0, oldPath.length() - child->impl_->displayName_.length());
				child->path_ = parentPath + Collection::getIndexOpen() + std::to_string(static_cast<int>(i)) + Collection::getIndexClose();
			}
		}

		auto model = getModel();
		TF_ASSERT(model != nullptr);
		model->postRowsInserted_(model->index(this), static_cast<int>(index), static_cast<int>(count));

		if (!collection.isMapping())
		{
			for (auto i = index + count; i < impl_->children_.size(); ++i)
			{
				auto child = static_cast<ReflectedPropertyItemNew*>(impl_->children_[i].get());
				if (child == nullptr)
				{
					continue;
				}

				auto childIndex = model->index(child);
				model->preItemDataChanged_(childIndex, 0, ItemRole::displayId, child->impl_->displayName_);
				child->impl_->displayName_ = Collection::getIndexOpen() + std::to_string(static_cast<int>(i)) + Collection::getIndexClose();
				model->postItemDataChanged_(childIndex, 0, ItemRole::displayId, child->impl_->displayName_);
			}
		}

		return true;
	}

	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postInserted(accessor, index, count))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedPropertyItemNew::preErase(const PropertyAccessor& accessor, size_t index, size_t count)
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto otherObj = accessor.getObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		auto model = getModel();
		TF_ASSERT(model != nullptr);
		model->preRowsRemoved_(model->index(this), static_cast<int>(index), static_cast<int>(count));
		return true;
	}

	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preErase(accessor, index, count))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedPropertyItemNew::postErased(const PropertyAccessor& accessor, size_t index, size_t count)
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = getObject();
	auto otherObj = accessor.getObject();
	auto otherPath = accessor.getFullPath();

	if (obj == otherObj && path_ == otherPath)
	{
		Collection collection;
		const Variant& value = accessor.getValue();
		bool isCollection = value.tryCast(collection);
		TF_ASSERT(isCollection);

		for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
		{
			auto child = static_cast<ReflectedPropertyItemNew*>(it->get());
			if (child == nullptr)
			{
				continue;
			}

			child->impl_->children_.clear();
		}

		impl_->children_.erase(impl_->children_.begin() + index, impl_->children_.begin() + index + count);
		for (auto it = impl_->children_.begin() + index; it != impl_->children_.end(); ++it)
		{
			auto& item = *it;
			if (item == nullptr)
			{
				continue;
			}

			item->setIndex(item->getIndex() - count);
		}
		if (!collection.isMapping())
		{
			for (auto i = index; i < impl_->children_.size(); ++i)
			{
				auto child = static_cast<ReflectedPropertyItemNew*>(impl_->children_[i].get());
				if (child == nullptr)
				{
					continue;
				}

				auto oldPath = child->path_;
				auto parentPath = oldPath.substr(0, oldPath.length() - child->impl_->displayName_.length());
				child->path_ = parentPath + Collection::getIndexOpen() + std::to_string(static_cast<int>(i)) + Collection::getIndexClose();
			}
		}

		auto model = getModel();
		TF_ASSERT(model != nullptr);
		model->postRowsRemoved_(model->index(this), static_cast<int>(index), static_cast<int>(count));

		if (!collection.isMapping())
		{
			for (auto i = index; i < impl_->children_.size(); ++i)
			{
				auto child = static_cast<ReflectedPropertyItemNew*>(impl_->children_[i].get());
				if (child == nullptr)
				{
					continue;
				}

				auto childIndex = model->index(child);
				model->preItemDataChanged_(childIndex, 0, ItemRole::displayId, child->impl_->displayName_);
				child->impl_->displayName_ = Collection::getIndexOpen() + std::to_string(static_cast<int>(i)) + Collection::getIndexClose();
				model->postItemDataChanged_(childIndex, 0, ItemRole::displayId, child->impl_->displayName_);
			}
		}

		return true;
	}

	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postErased(accessor, index, count))
		{
			return true;
		}
	}
	return false;
}
} // end namespace wgt
