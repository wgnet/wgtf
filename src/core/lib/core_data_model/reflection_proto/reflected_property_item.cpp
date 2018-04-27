#include "reflected_property_item.hpp"

#include "reflected_tree_model.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_data_model/reflection/reflected_collection.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/object/object_reference.hpp"
#include "../reflection/reflected_enum_model_new.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "../reflection/class_definition_model_new.hpp"
#include "wg_types/binary_block.hpp"
#include "../file_system/file_system_model.hpp"
#include "core_variant/collection.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/interfaces/i_property_path.hpp"

namespace wgt
{
ITEMROLE(path)
ITEMROLE(fullPath)
ITEMROLE(indexPath)
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
ITEMROLE(objectHierarchy)
ITEMROLE(staticString)
ITEMROLE(minMappedValue)
ITEMROLE(maxMappedValue)
ITEMROLE(parentCollection)
ITEMROLE(canInsert)
ITEMROLE(canRemove)
ITEMROLE(canInsertIntoParent)
ITEMROLE(canRemoveFromParent)
ITEMROLE(collectionIndex)

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
ReflectedPropertyItem::ReflectedPropertyItem(const ReflectedTreeModel& model, const std::shared_ptr< const IPropertyPath > & path, bool recordHistory)
    : model_(model), path_(path), recordHistory_(recordHistory)
{
	auto rootObject = model_.getObject();
	std::shared_ptr<ObjectReference> rootReference = std::dynamic_pointer_cast<ObjectReference>(rootObject.storage());
	if (rootReference != nullptr)
	{
		auto pDefinitionManager = get<IDefinitionManager>();
		auto rootDefinition = pDefinitionManager->getObjectDefinition(rootObject);
		if (rootDefinition != nullptr)
		{
			auto propertyAccessor = rootDefinition->bindProperty(path_, rootObject);
			objectReference_ = propertyAccessor.getRootObject();
			referencePath_ = propertyAccessor.getFullPath();
		}
	}
}

ReflectedPropertyItem::~ReflectedPropertyItem()
{
}

const std::shared_ptr< const IPropertyPath > & ReflectedPropertyItem::getPath() const
{
	return path_;
}

void ReflectedPropertyItem::setPath(const std::shared_ptr< const IPropertyPath >& path)
{
	path_ = path;
}

const std::string ReflectedPropertyItem::getPathName() const
{
	std::shared_ptr< const IPropertyPath > lastProperty;
	for( auto path = path_; path != nullptr; path = path->getParent() )
	{
		if (path->getType() == IPropertyPath::TYPE_PROPERTY ||
			path->getType() == IPropertyPath::TYPE_COLLECTION &&
			lastProperty != nullptr)
		{
			lastProperty = path;
			break;
		}
	}
	for (auto path = path_; path != nullptr; path = path->getParent())
	{
		if (path == lastProperty)
		{
			break;
		}
		if (path->getType() == IPropertyPath::TYPE_COLLECTION_ITEM)
		{
			return path_->generateDecoratedPath();
		}
	}
	if (lastProperty)
	{
		return lastProperty->getPath();
	}
	static std::string s_Empty;
	return s_Empty;
}

bool ReflectedPropertyItem::isReadOnly(const PropertyAccessor& propertyAccessor) const
{
	auto readonly = findFirstMetaData<MetaReadOnlyObj>(propertyAccessor, *get<IDefinitionManager>());
	if (readonly != nullptr)
	{
		return readonly->isReadOnly(propertyAccessor.getObject());
	}
	Collection collection;
	const bool isCollection = propertyAccessor.getValue().tryCast(collection);
	if (isCollection == false)
	{
		return false;
	}
	return !collection.canResize();
}

PropertyAccessor ReflectedPropertyItem::parentCollectionPropertyAccessor(const PropertyAccessor& propertyAccessor, IClassDefinition* definition) const
{
	auto object = propertyAccessor.getRootObject();
	std::string path = propertyAccessor.getFullPath();

	if (path[path.length() - 1] != Collection::getIndexClose())
	{
		return PropertyAccessor();
	}

	size_t indexPosition = path.find_last_of(Collection::getIndexOpen());
	path = path.substr(0, indexPosition);
	return definition->bindProperty(path.c_str(), object);
}

Variant ReflectedPropertyItem::getData(int column, ItemRole::Id roleId) const
{
	auto pDefinitionManager = get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return Variant();
	}

	const auto& object = model_.getObject();
	auto definition = pDefinitionManager->getObjectDefinition(object);
	if (definition == nullptr)
	{
		return Variant();
	}

	auto propertyAccessor = definition->bindProperty(path_, object);

	if (roleId == ItemRole::displayId)
	{
		switch (column)
		{
		case 0:
		{
			auto indexDisplayName = findFirstMetaData<MetaDisplayPathNameCallbackObj>(propertyAccessor, *pDefinitionManager);
			if (indexDisplayName != nullptr)
			{
				return indexDisplayName->getDisplayName(getPathName(), propertyAccessor.getObject());
			}
			auto displayName = findFirstMetaData<MetaDisplayNameObj>(propertyAccessor, *pDefinitionManager);
			if (displayName != nullptr)
			{
				return displayName->getDisplayName(propertyAccessor.getObject());
			}
			ObjectHandle object;
			auto attrDisplayName =
				findFirstMetaData<MetaAttributeDisplayNameObj>(propertyAccessor, *pDefinitionManager);
			if (attrDisplayName != nullptr)
			{
				object = propertyAccessor.getObject();
				definition = pDefinitionManager->getObjectDefinition(object);
			}
			else
			{
				auto value = propertyAccessor.getValue();
				if (value.tryCast(object))
				{
					definition =
						pDefinitionManager->getObjectDefinition(object);
					if (definition)
					{
						attrDisplayName =
							findFirstMetaData<MetaAttributeDisplayNameObj>(
								*definition, *pDefinitionManager);
					}
				}
			}
			if (attrDisplayName != nullptr)
			{
				propertyAccessor = definition->bindProperty(
					attrDisplayName->getAttributeName(), object);
				return propertyAccessor.getValue();
			}

			return getPathName().c_str();
		}
		default:
			return "Reflected Property";
		}
	}
	else if (roleId == ItemRole::pathId)
	{
		return propertyAccessor.getPath();
	}
	else if (roleId == ItemRole::fullPathId)
	{
		return propertyAccessor.getFullPath();
	}
	else if (roleId == ItemRole::indexPathId)
	{
		return getPath()->getRecursivePath();
	}
	else if (roleId == ItemRole::valueId || roleId == ValueRole::roleId_)
	{
		if (!propertyAccessor.canGetValue())
		{
			return Variant();
		}
		auto value = propertyAccessor.getValue();
		auto objectValueComponent = findFirstMetaData<MetaEnableValueComponentForObjectObj>(propertyAccessor, *pDefinitionManager);
		if (objectValueComponent != nullptr)
		{
			if (value.canCast<ObjectHandle>())
			{
				auto handle = value.cast<ObjectHandle>();
				return objectValueComponent->getComponentValue(handle);
			}
		}
		if (value.canCast<Collection>())
		{
			value = Collection(std::make_shared<ReflectedCollection>(propertyAccessor, get<IReflectionController>()));
		}
		return value;
	}
	else if (roleId == ItemRole::valueTypeId || roleId == ValueTypeRole::roleId_)
	{
		auto objectValueComponent = findFirstMetaData<MetaEnableValueComponentForObjectObj>(propertyAccessor, *pDefinitionManager);
		if (objectValueComponent != nullptr)
		{
			auto objectValue = propertyAccessor.getValue();
			if (objectValue.canCast<ObjectHandle>())
			{
				auto handle = objectValue.cast<ObjectHandle>();
				return objectValueComponent->getComponentType(handle);
			}
		}
		static TypeId s_valueType = TypeId::getType<Variant>();
		auto typeId = propertyAccessor.getType();
		if (typeId == s_valueType)
		{
			auto value = propertyAccessor.getValue();
			typeId = value.type()->typeId();
		}
		return typeId.getName();
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
		auto assetManager = get<IAssetManager>();
		if (assetManager == nullptr)
		{
			return Variant();
		}
		return assetManager->assetModel();
	}
	else if (roleId == ItemRole::nameId)
	{
		return propertyAccessor.getName();
	}
	else if (roleId == ItemRole::componentTypeId)
	{
		auto metaComponent = findFirstMetaData<MetaComponentObj>(propertyAccessor, *get<IDefinitionManager>());
		if (metaComponent == nullptr)
		{
			return nullptr;
		}
		return metaComponent->getComponentName();
	}
	else if (roleId == ItemRole::objectHierarchyId)
	{
		// HACK: this is to get around the limitation of the 'Object' role for models that don't return by value.
		// Objects returned through the Object role may be copies or properties of other objects that are also copies.
		// The returned ObjectHandle will keep the returned object alive, but once the property accessor goes out of
		// scope the parent object (which is also a copy) will be destroyed. If the returned object tries to reference
		// any data from its parent you have a problem
		auto objects = std::make_shared<CollectionHolder<std::vector<ObjectHandle>>>();
		auto pa = &propertyAccessor;
		ObjectHandle object = pa->getObject();

		while (object.isValid())
		{
			objects->storage().push_back(object);
			object = object.parent();
		}

		return Collection(objects);
	}

	if (roleId == EnumModelRole::roleId_)
	{
		auto enumObj = findFirstMetaData<MetaEnumObj>(propertyAccessor, *get<IDefinitionManager>());
		if (enumObj != nullptr)
		{
			return enumObj->generateEnum(propertyAccessor.getObject());
		}
	}
	else if (roleId == ObjectRole::roleId_)
	{
		return propertyAccessor.getObject();
	}
	else if (roleId == RootObjectRole::roleId_)
	{
		return model_.getObject();
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
	else if (roleId == ItemRole::staticStringId)
	{
		return findFirstMetaData<MetaStaticStringObj>(propertyAccessor, *pDefinitionManager) != nullptr;
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
	else if (roleId == ItemRole::tooltipId)
	{
		auto tooltipObj = findFirstMetaData<MetaTooltipObj>(propertyAccessor, *pDefinitionManager);
		if (tooltipObj != nullptr)
		{
			return tooltipObj->getTooltip(propertyAccessor.getObject());
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
			if (variant.tryCast(minValue))
			{
				if (minValue - value > std::numeric_limits<float>::epsilon())
				{
					NGT_ERROR_MSG("Property %s: MetaMinMaxObj min value exceeded limits.\n", path_->getRecursivePath().str().c_str());
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
			if (variant.tryCast(maxValue))
			{
				if (value - maxValue > std::numeric_limits<float>::epsilon())
				{
					NGT_ERROR_MSG("Property %s: MetaMinMaxObj max value exceeded limits.\n", path_->getRecursivePath().str().c_str());
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
	else if (roleId == ItemRole::minMappedValueId)
	{
		auto minMaxObj = findFirstMetaData<MetaMinMaxMappedObj>(propertyAccessor, *pDefinitionManager);
		if (minMaxObj != nullptr)
		{
			TypeId typeId = propertyAccessor.getType();
			Variant variant = getMinValue(typeId);
			const float& value = minMaxObj->getMappedMin();
			float minValue = .0f;
			if (variant.tryCast(minValue))
			{
				if (minValue - value > std::numeric_limits<float>::epsilon())
				{
					NGT_ERROR_MSG("Property %s: MetaMinMaxMappedObj min value exceeded limits.\n", path_->getRecursivePath().str().c_str());
					return variant;
				}
			}
			return value;
		}
		return Variant(); // undefined
	}
	else if (roleId == ItemRole::maxMappedValueId)
	{
		auto minMaxObj = findFirstMetaData<MetaMinMaxMappedObj>(propertyAccessor, *pDefinitionManager);
		if (minMaxObj != nullptr)
		{
			TypeId typeId = propertyAccessor.getType();
			Variant variant = getMaxValue(typeId);
			const float& value = minMaxObj->getMappedMax();
			float maxValue = .0f;
			if (variant.tryCast(maxValue))
			{
				if (value - maxValue > std::numeric_limits<float>::epsilon())
				{
					NGT_ERROR_MSG("Property %s: MetaMinMaxMappedObj max value exceeded limits.\n", path_->getRecursivePath().str().c_str());
					return variant;
				}
			}
			return value;
		}
		return Variant(); // undefined
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
				if (!definitionModel_ || static_cast<ClassDefinitionModelNew*>(definitionModel_.get())->definition() != definition)
				{
					definitionModel_.reset(new ClassDefinitionModelNew(definition, *pDefinitionManager));
				}

				return definitionModel_.get();
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
		return isReadOnly(propertyAccessor);
	}
	else if (roleId == ItemRole::enabledId)
	{
		return Variant(true);
	}
	else if (roleId == ItemRole::multipleValuesId)
	{
		return Variant(false);
	}
	else if (roleId == ItemRole::parentCollectionId)
	{
		auto parentCollectionPA = parentCollectionPropertyAccessor(propertyAccessor, definition);
		if (!parentCollectionPA.isValid())
		{
			return Variant();
		}

		Variant variant = parentCollectionPA.getValue();
		if (!variant.canCast<Collection>())
		{
			return Variant();
		}

		return Collection(std::make_shared<ReflectedCollection>(parentCollectionPA, get<IReflectionController>()));
	}
	else if (roleId == ItemRole::canInsertId)
	{
		if (!propertyAccessor.getProperty()->isCollection())
		{
			return false;
		}

		if (isReadOnly(propertyAccessor))
		{
			return false;
		}

		auto customInsert = findFirstMetaData<MetaInsertObj>(propertyAccessor, *pDefinitionManager);
		if (customInsert != nullptr)
		{
			return customInsert->canInsert(propertyAccessor.getObject());
		}

		return true;
	}
	else if (roleId == ItemRole::canRemoveId)
	{
		if (!propertyAccessor.getProperty()->isCollection())
		{
			return false;
		}

		if (isReadOnly(propertyAccessor))
		{
			return false;
		}

		auto customInsert = findFirstMetaData<MetaInsertObj>(propertyAccessor, *pDefinitionManager);
		if (customInsert != nullptr)
		{
			return customInsert->canRemove(propertyAccessor.getObject());
		}

		Collection collection;
		const bool isCollection = propertyAccessor.getValue().tryCast(collection);
		if (isCollection == false)
		{
			return false;
		}

		return collection.size() > 0;
	}
	else if (roleId == ItemRole::canInsertIntoParentId)
	{
		// Check that parent item is a collection and can be resized.
		auto parentCollectionPA = parentCollectionPropertyAccessor(propertyAccessor, definition);
		if (!parentCollectionPA.isValid())
		{
			return false;
		}

		if (!parentCollectionPA.getProperty()->isCollection())
		{
			return false;
		}

		if (isReadOnly(parentCollectionPA))
		{
			return false;
		}

		auto customInsert = findFirstMetaData<MetaInsertObj>(parentCollectionPA, *pDefinitionManager);
		if (customInsert != nullptr)
		{
			return customInsert->canInsert(parentCollectionPA.getObject());
		}

		return true;
	}
	else if (roleId == ItemRole::canRemoveFromParentId)
	{
		// Check that parent item is a collection and can be resized.
		auto parentCollectionPA = parentCollectionPropertyAccessor(propertyAccessor, definition);
		if (!parentCollectionPA.isValid())
		{
			return false;
		}

		if (!parentCollectionPA.getProperty()->isCollection())
		{
			return false;
		}

		if (isReadOnly(parentCollectionPA))
		{
			return false;
		}

		auto customInsert = findFirstMetaData<MetaInsertObj>(parentCollectionPA, *pDefinitionManager);
		if (customInsert != nullptr)
		{
			return customInsert->canRemove(parentCollectionPA.getObject());
		}

		Collection collection;
		const bool isCollection = parentCollectionPA.getValue().tryCast(collection);
		if (isCollection == false)
		{
			return false;
		}

		return collection.size() > 0;
	}
	else if (roleId == ItemRole::collectionIndexId)
	{
		auto object = propertyAccessor.getRootObject();
		std::string path = propertyAccessor.getFullPath();

		if (path[path.length() - 1] != Collection::getIndexClose())
		{
			return Variant();
		}

		size_t position = path.find_last_of(Collection::getIndexOpen()) + 1;
		size_t count = path.length() - 1 - position;
		std::string stringIndex = path.substr(position, count);

		for (char& c: stringIndex)
		{
			if (std::isdigit(c) == 0)
			{
				return stringIndex;
			}
		}

		return std::stoi(stringIndex);
	}

	return Variant();
}

bool ReflectedPropertyItem::setData(int column, ItemRole::Id roleId, const Variant& data)
{
	auto controller = get<IReflectionController>();

	auto pDefinitionManager = get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	const auto& object = model_.getObject();
	auto definition = pDefinitionManager->getObjectDefinition(object);
	if (definition == nullptr)
	{
		return false;
	}

	auto propertyAccessor = definition->bindProperty(path_, object);

	if (roleId == ValueRole::roleId_)
	{
		if (!propertyAccessor.canSetValue())
		{
			return false;
		}

		if (controller && recordHistory_)
		{
			// Note: Should return result from this function
			controller->setValue(propertyAccessor, data);
		}
		else
		{
			// Note: Should return result from this function
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
		if (controller && recordHistory_)
		{
			controller->setValue(propertyAccessor, value);
		}
		else
		{
			propertyAccessor.setValue(value);
		}
		return true;
	}
	else if (roleId == ItemRole::parentCollectionId)
	{
		auto object = propertyAccessor.getRootObject();
		std::string path = propertyAccessor.getFullPath();

		if (path[path.length() - 1] != Collection::getIndexClose())
		{
			return false;
		}

		size_t indexPosition = path.find_last_of(Collection::getIndexOpen());
		path = path.substr(0, indexPosition);
		PropertyAccessor accessor = definition->bindProperty(path.c_str(), object);
		Variant oldValue = accessor.getValue();
		Variant newValue = data;
		Collection* oldCollection = nullptr;
		Collection* newCollection = nullptr;

		if (!oldValue.tryCast(oldCollection) || oldCollection == nullptr ||
			!newValue.tryCast(newCollection) || newCollection == nullptr)
		{
			return false;
		}

		size_t oldSize = oldCollection->size();
		size_t newSize = newCollection->size();

		bool same = oldCollection->flags() == newCollection->flags() &&
			oldCollection->keyType() == newCollection->keyType() &&
			oldCollection->valueType() == newCollection->valueType();

		if (!same)
		{
			return false;
		}

		return accessor.setValue(newValue);
	}

	return false;
}
}
}