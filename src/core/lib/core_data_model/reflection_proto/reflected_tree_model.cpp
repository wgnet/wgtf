#include "reflected_tree_model.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/object/object_reference.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/file_system/file_system_model.hpp"

#include "core_common/scoped_stop_watch.hpp"
#include "core_reflection/interfaces/i_property_path.hpp"
#include "core_reflection/reflection_batch_query.hpp"
#include <algorithm>


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

namespace ReflectedTreeModelDetails
{
const char* itemMimeKey = "application/reflectedtreemodel-item";
const char* textMimeKey = "text/plain";

static const std::string s_RolesArr[] = {
	ItemRole::pathName, ItemRole::fullPathName, ItemRole::indexPathName, ItemRole::componentTypeName, ItemRole::valueName, 
	ItemRole::valueTypeName, ItemRole::keyName, ItemRole::keyTypeName, ItemRole::isCollectionName, ItemRole::elementValueTypeName, 
	ItemRole::elementKeyTypeName, ItemRole::readOnlyName, ItemRole::enabledName, ItemRole::multipleValuesName, ItemRole::assetModelName,
	ItemRole::nameName, ItemRole::objectHierarchyName, ItemRole::tooltipName, 
	ItemRole::staticStringName, ItemRole::minMappedValueName, ItemRole::maxMappedValueName, ItemRole::parentCollectionName,
	ItemRole::canInsertName, ItemRole::canRemoveName, ItemRole::canInsertIntoParentName, ItemRole::canRemoveFromParentName, ItemRole::collectionIndexName,
	// DEPRECATED
	EnumModelRole::roleName_, DefinitionRole::roleName_, DefinitionModelRole::roleName_, ObjectRole::roleName_,
	RootObjectRole::roleName_, MinValueRole::roleName_, MaxValueRole::roleName_, StepSizeRole::roleName_,
	DecimalsRole::roleName_, IndexPathRole::roleName_, UrlIsAssetBrowserRole::roleName_, UrlDialogTitleRole::roleName_,
	UrlDialogDefaultFolderRole::roleName_, UrlDialogNameFiltersRole::roleName_,
	UrlDialogSelectedNameFilterRole::roleName_, IsReadOnlyRole::roleName_, IsEnumRole::roleName_,
	IsThumbnailRole::roleName_, IsSliderRole::roleName_, IsColorRole::roleName_, IsUrlRole::roleName_,
	IsActionRole::roleName_, DescriptionRole::roleName_, ThumbnailRole::roleName_
};

static const std::vector<std::string> s_RolesVec(&s_RolesArr[0],
                                                 &s_RolesArr[0] + std::extent<decltype(s_RolesArr)>::value);

static const std::string s_MimeTypesArr[] = { ReflectedTreeModelDetails::itemMimeKey,
	ReflectedTreeModelDetails::textMimeKey, FileSystemModel::s_mimeFilePath };

static const std::vector<std::string> s_MimeTypesVec(&s_MimeTypesArr[0],
                                                     &s_MimeTypesArr[0] + std::extent<decltype(s_MimeTypesArr)>::value);


static std::string getIndexPath(const Variant &key, size_t index)
{
	std::string stringKey;
	size_t indexKey = index;
	std::string indexPath;
	if (key.tryCast(indexKey)) {
		indexPath = Collection::getIndexOpen() + std::to_string(indexKey) + Collection::getIndexClose();
	}
	else if (key.tryCast(stringKey))
	{
		indexPath = Collection::getIndexOpenStr() + stringKey + Collection::getIndexCloseStr();
	}
	else {
		indexPath = Collection::getIndexOpen() + std::to_string(index) + Collection::getIndexClose();
	}

	return indexPath;
}

/**
 *	Check if the given type is a Vector 2, 3 or 4.
 */
bool isVectorType(const char* typeName)
{
	// @see string constants in MetaType::find()
	const char* VECTOR_STRING = "Vector";
	return (strstr(typeName, VECTOR_STRING) != nullptr);
}

bool isVectorType(const MetaType* pType)
{
	if (pType == nullptr)
	{
		return false;
	}
	return isVectorType(pType->name());
}

bool isVectorType(const Variant& value)
{
	return isVectorType(value.type());
}

}

namespace proto
{
class ReflectedTreeModelPropertyListener : public PropertyAccessorListener, Depends<IDefinitionManager, ICommandManager>
{
public:
	ReflectedTreeModelPropertyListener(ReflectedTreeModel& model) : model_(model)
	{
	}

	~ReflectedTreeModelPropertyListener()
	{
	}

	// PropertyAccessorListener
	virtual void preSetValue(const PropertyAccessor& accessor, const Variant& value) override
	{
		auto property = model_.findProperty(accessor.getRootObject(), accessor.getFullPath());
		if (property == nullptr)
		{
			return;
		}

		auto definitionManager = get<IDefinitionManager>();

		const TypeId typeId = accessor.getType();
		const bool isPolyStruct = typeId.isPointer() && definitionManager->getDefinition(typeId.removePointer().getName()) != nullptr;
		const bool mapped = model_.isMapped(property);
		bool isCollection = false;

		if (!mapped && accessor.getRootObject() == model_.getObject())
		{
			Collection* collection = nullptr;
			Variant data = value;
			isCollection = data.tryCast(collection);
		}
		
		bool invalidateItem = false;
		auto metaInvalidatesObject = findFirstMetaData<MetaInvalidatesObjectObj>(accessor, *get<IDefinitionManager>());

		if (metaInvalidatesObject != nullptr)
		{
			if (!get<ICommandManager>()->executingCommandGroup())
			{
				property = model_.parentProperty(property);
				invalidateItem = true;
			}
		}
		else
		{
			auto definitionManager = get<IDefinitionManager>();
			TypeId typeId = accessor.getType();
			if (isPolyStruct || isCollection)
			{
				invalidateItem = true;
				property = model_.parentProperty(property);
			}
		}
		
		if (invalidateItem)
		{
			auto item = model_.mappedItem(property);
			const auto index = model_.index(item);

			if (index.isValid())
			{
				model_.preLayoutChanged_(index);
			}
			else
			{
				model_.preModelReset_();
			}

			return;
		}
		
		if (mapped)
		{
			const auto index = model_.index(property);
			const int column = 0;
			ItemRole::Id roleId = ValueRole::roleId_;
			model_.preItemDataChanged_(index, column, roleId, value);

			if (isPolyStruct)
			{
				roleId = DefinitionRole::roleId_;
				model_.preItemDataChanged_(index, column, roleId, value);
			}
		}
	}

	virtual void postSetValue(const PropertyAccessor& accessor, const Variant& value) override
	{
		auto property = model_.findProperty(accessor.getRootObject(), accessor.getFullPath());
		if (property == nullptr)
		{
			return;
		}

		auto definitionManager = get<IDefinitionManager>();
		const TypeId typeId = accessor.getType();
		const bool isPolyStruct = typeId.isPointer() && definitionManager->getDefinition(typeId.removePointer().getName()) != nullptr;
		const bool mapped = model_.isMapped(property);
		bool isCollection = false;

		if (!mapped && accessor.getRootObject() == model_.getObject())
		{
			Collection* collection = nullptr;
			Variant data = value;
			isCollection = data.tryCast(collection);
		}

		bool invalidateItem = false;
		auto metaInvalidatesObject = findFirstMetaData<MetaInvalidatesObjectObj>(accessor, *get<IDefinitionManager>());

		if (metaInvalidatesObject != nullptr)
		{
			if (!get<ICommandManager>()->executingCommandGroup())
			{
				property = model_.parentProperty(property);
				invalidateItem = true;
			}
		}
		else
		{
			auto definitionManager = get<IDefinitionManager>();
			TypeId typeId = accessor.getType();
			if (isPolyStruct || isCollection)
			{
				invalidateItem = true;
				property = model_.parentProperty(property);
			}
		}

		if (invalidateItem)
		{
			auto item = model_.mappedItem(property);
			const auto index = model_.index(item);

			if (index.isValid())
			{
				model_.unmapItem(item);
				model_.postLayoutChanged_(index);
			}
			else
			{
				model_.unmapItem(nullptr);
				model_.postModelReset_();
			}

			return;
		}
	
		if (mapped)
		{
			const auto index = model_.index(property);
			const int column = 0;
			ItemRole::Id roleId = ValueRole::roleId_;
			model_.postItemDataChanged_(index, column, roleId, value);

			if (isPolyStruct)
			{
				roleId = DefinitionRole::roleId_;
				model_.postItemDataChanged_(index, column, roleId, value);
			}
		}
	}

	virtual void preInsert(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		auto item = model_.findProperty(accessor.getRootObject(), accessor.getFullPath());
		if (item == nullptr)
		{
			return;
		}

		auto propertiesIt = model_.properties_.find(item);
		if (propertiesIt == model_.properties_.end())
		{
			return;
		}

		auto childHint = model_.childHint(item);
		if (!childHint.isValid())
		{
			return;
		}
		auto parent = childHint.parent_;
		auto row = childHint.row_ + static_cast<int>(index);

		model_.preRowsInserted_(model_.index(parent), row, static_cast<int>(count));
	}

	virtual void postInserted(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		auto item = model_.findProperty(accessor.getRootObject(), accessor.getFullPath());
		if (item == nullptr)
		{
			return;
		}

		auto propertiesIt = model_.properties_.find(item);
		if (propertiesIt == model_.properties_.end())
		{
			return;
		}

		auto childHint = model_.childHint(item);
		if (!childHint.isValid())
		{
			return;
		}
		auto parent = childHint.parent_;
		auto row = childHint.row_ + static_cast<int>(index);

		auto value = accessor.getValue();
		Collection collection;
		value.tryCast(collection);
		auto collectionIt = collection.begin();
		collectionIt += index;

		// insert into parent mapping
		auto& properties = propertiesIt->second;
		auto propertyIt = properties.begin() + index;
		auto parentMapping = model_.mapItem(parent);
		auto mappingIt = parentMapping->children_->begin() + row;
		size_t i = index;
		auto && collectionPath = item->getPath();
		for (; i < index + count; ++i, ++collectionIt)
		{
			TF_ASSERT(collectionIt != collection.end());

			auto && propertyPath = collectionPath->generateChildPath( collectionPath, collectionIt.key());
			auto propertyPtr = model_.makeProperty(propertyPath);
			auto property = propertyPtr.get();

			propertyIt = properties.emplace(propertyIt, std::move(propertyPtr)) + 1;
			mappingIt = parentMapping->children_->insert(mappingIt, property) + 1;

			auto mapping = new ReflectedTreeModel::ItemMapping();
			mapping->parent_ = parent;
			model_.mappedItems_.insert(
			std::make_pair(property, std::unique_ptr<ReflectedTreeModel::ItemMapping>(mapping)));
		}

		// update property keys
		if (!collection.isMapping())
		{
			for (; i < collection.size(); ++i, ++collectionIt, ++propertyIt)
			{
				TF_ASSERT(propertyIt != properties.end());
				auto && propertyPath = collectionPath->generateChildPath(collectionPath, collectionIt.key());
				model_.updatePath(propertyIt->get(), propertyPath);
			}
		}

		model_.postRowsInserted_(model_.index(parent), row, static_cast<int>(count));

		const auto parentIndex = model_.index(parent);
		if (parentIndex.isValid())
		{
			model_.preLayoutChanged_(parentIndex);
			model_.unmapItem(parent);
			model_.postLayoutChanged_(parentIndex);
		}
		else
		{
			model_.preModelReset_();
			model_.unmapItem(nullptr);
			model_.postModelReset_();
		}
	}

	virtual void preErase(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		auto item = model_.findProperty(accessor.getRootObject(), accessor.getFullPath());
		if (item == nullptr)
		{
			return;
		}

		auto propertiesIt = model_.properties_.find(item);
		if (propertiesIt == model_.properties_.end())
		{
			return;
		}

		auto childHint = model_.childHint(item);
		if (!childHint.isValid())
		{
			return;
		}
		auto parent = childHint.parent_;
		auto row = childHint.row_ + static_cast<int>(index);

		model_.preRowsRemoved_(model_.index(parent), row, static_cast<int>(count));
	}

	virtual void postErased(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		auto item = model_.findProperty(accessor.getRootObject(), accessor.getFullPath());
		if (item == nullptr)
		{
			return;
		}

		auto propertiesIt = model_.properties_.find(item);
		if (propertiesIt == model_.properties_.end())
		{
			return;
		}

		auto childHint = model_.childHint(item);
		if (!childHint.isValid())
		{
			return;
		}
		auto parent = childHint.parent_;
		auto row = childHint.row_ + static_cast<int>(index);

		auto value = accessor.getValue();
		Collection collection;
		value.tryCast(collection);
		auto collectionIt = collection.begin();
		collectionIt += index;

		// delete properties
		auto& properties = propertiesIt->second;
		auto propertyIt = properties.begin() + index;
		auto parentMapping = model_.mapItem(parent);
		TF_ASSERT(parentMapping->children_->size() >= row + count);
		auto mappingIt = parentMapping->children_->begin() + row;
		for (size_t j = 0; j < count; ++j)
		{
			model_.unmapItem(propertyIt->get());
			TF_ASSERT(propertyIt != properties.end());
			TF_ASSERT(mappingIt != parentMapping->children_->end());
			propertyIt = properties.erase(propertyIt);
			mappingIt = parentMapping->children_->erase(mappingIt);
		}

		// update property keys
		if (!collection.isMapping())
		{
			size_t i = index;
			auto && collectionPath = item->getPath();
			for (; i < collection.size(); ++i, ++collectionIt, ++propertyIt)
			{
				TF_ASSERT(propertyIt != properties.end());
				model_.updatePath(
					propertyIt->get(),
					collectionPath->generateChildPath(collectionPath,collectionIt.key()));
			}
		}

		model_.postRowsRemoved_(model_.index(parent), row, static_cast<int>(count));

		const auto parentIndex = model_.index(parent);
		if (parentIndex.isValid())
		{
			model_.preLayoutChanged_(parentIndex);
			model_.unmapItem(parent);
			model_.postLayoutChanged_(parentIndex);
		}
		else
		{
			model_.preModelReset_();
			model_.unmapItem(nullptr);
			model_.postModelReset_();
		}
	}

private:
	ReflectedTreeModel& model_;
};

ReflectedTreeModel::ReflectedTreeModel(const ObjectHandle& object)
    : recordHistory_(true), listener_(new ReflectedTreeModelPropertyListener(*this))
{
	auto definitionManager = get<IDefinitionManager>();
	TF_ASSERT(definitionManager != nullptr);
	definitionManager->registerPropertyAccessorListener(listener_);

	auto rootMapping = new ItemMapping();
	mappedItems_.insert(std::make_pair(nullptr, std::unique_ptr<ItemMapping>(rootMapping)));

	setObject(object);
}

ReflectedTreeModel::~ReflectedTreeModel()
{
	setObject(nullptr);

	auto definitionManager = get<IDefinitionManager>();
	if (definitionManager != nullptr)
	{
		definitionManager->deregisterPropertyAccessorListener(listener_);
	}
}

void ReflectedTreeModel::setRecordHistory(bool recordHistory)
{
	recordHistory_ = recordHistory;
}

void ReflectedTreeModel::setObject(const ObjectHandle& object)
{
	SCOPE_TAG
	ReflectionBatchQuery batchQuery;
	preModelReset_();

	unmapItem(nullptr);
	object_ = object;
	modelChanged_();

	postModelReset_();
}

MimeData ReflectedTreeModel::mimeData(std::vector<AbstractItemModel::ItemIndex>& indices)
{
	if (indices.empty())
	{
		return MimeData();
	}

	MimeData mimeData;
	{
		ResizingMemoryStream stream;
		BinaryStream s(stream);
		s << indices.size();
		for (const auto& index : indices)
		{
			s << index.row_;
			s << static_cast<void*>(const_cast<AbstractItem*>(index.parent_));
		}

		std::string data = stream.takeBuffer();
		mimeData[ReflectedTreeModelDetails::itemMimeKey] = std::vector<char>(data.begin(), data.end());
	}
	{
		ResizingMemoryStream stream;
		TextStream s(stream);
		for (const auto& index : indices)
		{
			const auto pItem = this->item(ItemIndex(index.row_, index.parent_));
			if (pItem != nullptr)
			{
				const auto value = pItem->getData(index.row_, index.column_, ValueRole::roleId_);

				const char* typeName = value.type()->name();
				s << quoted(typeName);
				s << ' ';
				s << value;
			}
		}

		if (!s.fail())
		{
			const auto& data = stream.buffer();
			mimeData[ReflectedTreeModelDetails::textMimeKey] = std::vector<char>(data.begin(), data.end());
		}
	}

	return mimeData;
}

//------------------------------------------------------------------------------
void ReflectedTreeModel::iterateMimeTypes(const std::function<void(const char*)>& iterFunc) const
{
	for (auto&& role : ReflectedTreeModelDetails::s_MimeTypesVec)
	{
		iterFunc(role.c_str());
	}
}

//------------------------------------------------------------------------------
std::vector<std::string> ReflectedTreeModel::mimeTypes() const
{
	return ReflectedTreeModelDetails::s_MimeTypesVec;
}

bool ReflectedTreeModel::canDropMimeData(const MimeData& mimeData, DropAction action,
                                         const AbstractItemModel::ItemIndex& index) const
{
	if (action == DropAction::MoveAction)
	{
		{
		const auto it = mimeData.find(ReflectedTreeModelDetails::itemMimeKey);
		if (it != mimeData.end())
		{
			const auto canDropItem = this->canDropItemMimeData(it->second, action, index);
			if (canDropItem)
			{
				return true;
			}
		}
	}

		{
			const auto it = mimeData.find(FileSystemModel::s_mimeFilePath);
			if (it != mimeData.end())
			{
				return this->canDropFilePathMimeData(it->second, action, index);
			}
		}
	}
	else if (action == DropAction::CopyAction)
	{
		{
		const auto it = mimeData.find(ReflectedTreeModelDetails::textMimeKey);
		if (it != mimeData.end())
		{
			const auto canDropText = this->canDropTextMimeData(it->second, action, index);
			if (canDropText)
			{
				return true;
			}
		}
	}

		{
			const auto it = mimeData.find(FileSystemModel::s_mimeFilePath);
			if (it != mimeData.end())
			{
				return this->canDropFilePathMimeData(it->second, action, index);
			}
		}
	}

	return false;
}

bool ReflectedTreeModel::canDropItemMimeData(const std::vector<char>& data,
	DropAction action,
	const AbstractItemModel::ItemIndex& index) const
{
	auto parentItem = index.parent_;
	if (parentItem == nullptr)
	{
		return false;
	}

	auto isCollection = parentItem->getData(0, 0, ItemRole::isCollectionId);
	if (isCollection.isVoid() || isCollection == false)
	{
		return false;
	}

	auto collection = parentItem->getData(0, 0, ItemRole::valueId).cast<Collection>();
	if (collection.isMapping())
	{
		return false;
	}

	FixedMemoryStream stream(data.data(), data.size());
	BinaryStream s(stream);
	size_t numIndices;
	s >> numIndices;
	for (size_t i = 0; i < numIndices; ++i)
	{
		int row;
		s >> row;
		void* parent;
		s >> parent;
		if (static_cast<AbstractItem*>(parent) == parentItem)
		{
			return true;
		}
	}

	return false;
}

bool ReflectedTreeModel::canDropTextMimeData(const std::vector<char>& data,
	DropAction action,
	const AbstractItemModel::ItemIndex& index) const
{
	const auto pItem = this->item(ItemIndex(index.row_, index.parent_));
	if (pItem == nullptr)
	{
		return false;
	}

	FixedMemoryStream stream(data.data(), data.size());
	TextStream s(stream);
	std::string fromTypeName;
	s >> quoted(fromTypeName);
	const auto pFromMetaType = MetaType::find(fromTypeName.c_str());
	if (pFromMetaType == nullptr)
	{
		return false;
	}

	const auto toValue = pItem->getData(index.row_, index.column_, ValueRole::roleId_);
	
	// Special conversions for truncating vectors
	if (ReflectedTreeModelDetails::isVectorType(pFromMetaType) &&
		ReflectedTreeModelDetails::isVectorType(toValue))
	{
		return true;
	}

	// Standard conversion
	return pFromMetaType->canConvertTo(toValue.type());
}

bool ReflectedTreeModel::canDropFilePathMimeData(const std::vector<char>& data,
	DropAction action,
	const AbstractItemModel::ItemIndex& index) const
{
	const auto pItem = this->item(ItemIndex(index.row_, index.parent_));
	if (pItem == nullptr)
	{
		return false;
	}

	const auto readOnlyVariant = pItem->getData(index.row_, index.column_, ItemRole::readOnlyId);
	bool isReadOnly = false;
	readOnlyVariant.tryCast(isReadOnly);
	if (isReadOnly)
	{
		return false;
	}
	bool isUrl = false;
	const auto urlVariant = pItem->getData(index.row_, index.column_, IsUrlRole::roleId_);
	urlVariant.tryCast(isUrl);
	return isUrl;
}

bool ReflectedTreeModel::dropMimeData(const MimeData& mimeData, DropAction action,
                                      const AbstractItemModel::ItemIndex& index)
{
	if (action == DropAction::MoveAction)
	{
		{
		const auto it = mimeData.find(ReflectedTreeModelDetails::itemMimeKey);
		if (it != mimeData.end())
		{
			const auto itemDropped = this->dropItemMimeData(it->second, action, index);
			if (itemDropped)
			{
				return true;
			}
		}
	}

		{
			const auto it = mimeData.find(FileSystemModel::s_mimeFilePath);
			if (it != mimeData.end())
			{
				return this->dropFilePathMimeData(it->second, action, index);
			}
		}
	}
	else if (action == DropAction::CopyAction)
	{
		{
		const auto it = mimeData.find(ReflectedTreeModelDetails::textMimeKey);
		if (it != mimeData.end())
		{
			const auto textDropped = this->dropTextMimeData(it->second, action, index);
			if (textDropped)
			{
				return true;
			}
		}
	}

		{
			const auto it = mimeData.find(FileSystemModel::s_mimeFilePath);
			if (it != mimeData.end())
			{
				return this->dropFilePathMimeData(it->second, action, index);
			}
		}
	}

	return false;
}

bool ReflectedTreeModel::dropItemMimeData(const std::vector<char>& data,
	DropAction action,
	const AbstractItemModel::ItemIndex& index)
{
	auto parentItem = index.parent_;
	if (parentItem == nullptr)
	{
		return false;
	}

	auto isCollection = parentItem->getData(0, 0, ItemRole::isCollectionId);
	if (isCollection.isVoid() || isCollection == false)
	{
		return false;
	}

	auto collection = parentItem->getData(0, 0, ItemRole::valueId).cast<Collection>();
	if (collection.isMapping())
	{
		return false;
	}

	std::vector<int> rows;
	FixedMemoryStream stream(data.data(), data.size());
	BinaryStream s(stream);
	size_t numIndices;
	s >> numIndices;
	for (size_t i = 0; i < numIndices; ++i)
	{
		int row;
		s >> row;
		void* parent;
		s >> parent;
		if (static_cast<AbstractItem*>(parent) == parentItem)
		{
			rows.push_back(row);
		}
	}
	std::sort(rows.begin(), rows.end());

	auto commandManager = get<ICommandManager>();
	commandManager->beginBatchCommand();
	int sourceOffset = 0;
	int destOffset = 0;
	for (auto row : rows)
	{
		auto key = row + sourceOffset;
		auto value = collection.find(key).value();
		collection.eraseKey(key);
		if (row < index.row_)
		{
			--destOffset;
			--sourceOffset;
		}
		collection.insertValue(index.row_ + destOffset, value);
		++destOffset;
	}
	commandManager->endBatchCommand();
	return true;
}

bool ReflectedTreeModel::dropFilePathMimeData(const std::vector<char>& data,
	DropAction action,
	const AbstractItemModel::ItemIndex& index)
{
	const auto pItem = this->item(ItemIndex(index.row_, index.parent_));
	if (pItem == nullptr)
	{
		return false;
	}

	FixedMemoryStream stream(data.data(), data.size());
	TextStream s(stream);

	std::string filePath;
	s >> filePath;
	return pItem->setData(index.row_, index.column_, ValueRole::roleId_, filePath);
}

bool ReflectedTreeModel::dropTextMimeData(const std::vector<char>& data,
	DropAction action,
	const AbstractItemModel::ItemIndex& index)
{
	const auto pItem = this->item(ItemIndex(index.row_, index.parent_));
	if (pItem == nullptr)
	{
		return false;
	}

	FixedMemoryStream stream(data.data(), data.size());
	TextStream s(stream);

	std::string typeName;
	s >> quoted(typeName);
	const auto pFromMetaType = MetaType::find(typeName.c_str());
	if (pFromMetaType == nullptr)
	{
		return false;
	}

	// Special conversions for truncating vectors
	// E.g. want to paste a Vector3 into a Vector4 and ignore the last component
	auto toValue = pItem->getData(index.row_, index.column_, ValueRole::roleId_);

	if (ReflectedTreeModelDetails::isVectorType(pFromMetaType) &&
		ReflectedTreeModelDetails::isVectorType(toValue))
	{
		// Get 2, 3 or 4 component count from name
		// Depends on the name being in the format "Vector2", "Vector3", "Vector4"
		// @see string constants in MetaType::find()
		const auto fromName = pFromMetaType->name();
		const auto fromNameComponents = fromName + strlen(fromName) - 1;
		const auto fromComponentCount = atoi(fromNameComponents);

		// Get old value
		Vector4 finalVector;
		auto toComponentCount = 0;
		{
			Vector2 tmp2;
			Vector3 tmp3;
			Vector4 tmp4;
			if (toValue.tryCast(tmp2))
			{
				finalVector.x = tmp2.x;
				finalVector.y = tmp2.y;
				toComponentCount = 2;
			}
			else if (toValue.tryCast(tmp3))
			{
				finalVector.x = tmp3.x;
				finalVector.y = tmp3.y;
				finalVector.z = tmp3.z;
				toComponentCount = 3;
			}
			else if (toValue.tryCast(tmp4))
			{
				finalVector.x = tmp4.x;
				finalVector.y = tmp4.y;
				finalVector.z = tmp4.z;
				finalVector.w = tmp4.w;
				toComponentCount = 4;
			}
			else
			{
				assert(false && "isVectorType is true, but could not cast to vector");
			}
		}

		// Get new value using a strongly typed variant
		if (fromComponentCount == 2)
		{
			Variant fromComponentVector = Vector2(0.0f, 0.0f);
			s >> fromComponentVector;
			if (s.fail())
			{
				return false;
			}
			Vector2 value;
			const auto success = fromComponentVector.tryCast(value);
			assert(success);
			finalVector.x = value.x;
			finalVector.y = value.y;
		}
		else if (fromComponentCount == 3)
		{
			Variant fromComponentVector = Vector3(0.0f, 0.0f, 0.0f);
			s >> fromComponentVector;
			if (s.fail())
			{
				return false;
			}
			Vector3 value;
			const auto success = fromComponentVector.tryCast(value);
			assert(success);
			finalVector.x = value.x;
			finalVector.y = value.y;
			finalVector.z = value.z;
		}
		else if (fromComponentCount == 4)
		{
			Variant fromComponentVector = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			s >> fromComponentVector;
			if (s.fail())
			{
				return false;
			}
			Vector4 value;
			const auto success = fromComponentVector.tryCast(value);
			assert(success);
			finalVector.x = value.x;
			finalVector.y = value.y;
			finalVector.z = value.z;
			finalVector.w = value.w;
		}
		else
		{
			assert(false && "Invalid vector components");
		}
		
		// Set new value
		if (toComponentCount == 2)
		{
			Vector2 value;
			value.x = finalVector.x;
			value.y = finalVector.y;
			return pItem->setData(index.row_, index.column_, ValueRole::roleId_, value);
		}
		else if (toComponentCount == 3)
		{
			Vector3 value;
			value.x = finalVector.x;
			value.y = finalVector.y;
			value.z = finalVector.z;
			return pItem->setData(index.row_, index.column_, ValueRole::roleId_, value);
		}
		else if (toComponentCount == 4)
		{
			Vector4 value;
			value.x = finalVector.x;
			value.y = finalVector.y;
			value.z = finalVector.z;
			value.w = finalVector.w;
			return pItem->setData(index.row_, index.column_, ValueRole::roleId_, value);
		}
	}

	// Standard variant conversion
	Variant newValue(pFromMetaType);
	auto ptr = newValue.value<const void*>();
	if (ptr == nullptr)
	{
		return false;
	}
	pFromMetaType->streamIn(s, const_cast<void*>(ptr));
	if (s.fail())
	{
		return false;
	}

	return pItem->setData(index.row_, index.column_, ValueRole::roleId_, newValue);
}

AbstractItem* ReflectedTreeModel::item(const ItemIndex& index) const
{
	if (!index.isValid())
	{
		return nullptr;
	}

	auto parentMapping = const_cast<ReflectedTreeModel*>(this)->mapItem(index.parent_);
	if(parentMapping == nullptr || parentMapping->children_ == nullptr || index.row_ >= static_cast<int>(parentMapping->children_->size()))
	{
		return nullptr;
	}

	auto item = parentMapping->children_->at(index.row_);
	return const_cast<AbstractItem*>(item);
}

AbstractTreeModel::ItemIndex ReflectedTreeModel::index(const AbstractItem* item) const
{
	if (item == nullptr)
	{
		return ItemIndex();
	}

	// don't call mapItem for the passed in items as we do not need to iterate its children at this stage
	auto it = mappedItems_.find(item);
	TF_ASSERT(it != mappedItems_.end());
	auto mapping = it->second.get();
	auto parent = mapping->parent_;

	auto parentMapping = const_cast<ReflectedTreeModel*>(this)->mapItem(parent);
	auto childIt = std::find(parentMapping->children_->begin(), parentMapping->children_->end(), item);
	TF_ASSERT(childIt != parentMapping->children_->end());
	auto row = static_cast<int>(childIt - parentMapping->children_->begin());

	return ItemIndex(row, parent);
}

int ReflectedTreeModel::rowCount(const AbstractItem* item) const
{
	auto mapping = const_cast<ReflectedTreeModel*>(this)->mapItem(item);
	return static_cast<int>(mapping->children_->size());
}

int ReflectedTreeModel::columnCount() const
{
	return 1;
}

int ReflectedTreeModel::getColumnCount() const
{
	return (int)mappedItems_.size();
}

//------------------------------------------------------------------------------
std::vector<std::string> ReflectedTreeModel::roles() const
{
	return ReflectedTreeModelDetails::s_RolesVec;
}

//------------------------------------------------------------------------------
void ReflectedTreeModel::iterateRoles(const std::function<void(const char*)>& iterFunc) const
{
	for (auto&& role : ReflectedTreeModelDetails::s_RolesVec)
	{
		iterFunc(role.c_str());
	}
}

//------------------------------------------------------------------------------
Connection ReflectedTreeModel::connectPreItemDataChanged(AbstractTreeModel::DataCallback callback)
{
	return preItemDataChanged_.connect(callback);
}

Connection ReflectedTreeModel::connectPostItemDataChanged(AbstractTreeModel::DataCallback callback)
{
	return postItemDataChanged_.connect(callback);
}

Connection ReflectedTreeModel::connectPreLayoutChanged(LayoutCallback callback)
{
	return preLayoutChanged_.connect(callback);
}

Connection ReflectedTreeModel::connectPostLayoutChanged(LayoutCallback callback)
{
	return postLayoutChanged_.connect(callback);
}

Connection ReflectedTreeModel::connectPreModelReset(VoidCallback callback)
{
	return preModelReset_.connect(callback);
}

Connection ReflectedTreeModel::connectPostModelReset(VoidCallback callback)
{
	return postModelReset_.connect(callback);
}

Connection ReflectedTreeModel::connectModelChanged(VoidCallback callback)
{
	return modelChanged_.connect(callback);
}

Connection ReflectedTreeModel::connectPreRowsInserted(AbstractTreeModel::RangeCallback callback)
{
	return preRowsInserted_.connect(callback);
}

Connection ReflectedTreeModel::connectPostRowsInserted(AbstractTreeModel::RangeCallback callback)
{
	return postRowsInserted_.connect(callback);
}

Connection ReflectedTreeModel::connectPreRowsRemoved(AbstractTreeModel::RangeCallback callback)
{
	return preRowsRemoved_.connect(callback);
}

Connection ReflectedTreeModel::connectPostRowsRemoved(AbstractTreeModel::RangeCallback callback)
{
	return postRowsRemoved_.connect(callback);
}

std::unique_ptr<ReflectedTreeModel::Children> ReflectedTreeModel::mapChildren(const AbstractItem* item)
{
	auto children = new Children();
	auto& properties = getProperties(static_cast<const ReflectedPropertyItem*>(item));
	for (auto& property : properties)
	{
		children->push_back(property.get());
	}
	return std::unique_ptr<Children>(children);
}

void ReflectedTreeModel::clearChildren(const AbstractItem* item)
{
	clearProperties(static_cast<const ReflectedPropertyItem*>(item));
}

AbstractTreeModel::ItemIndex ReflectedTreeModel::childHint(const ReflectedPropertyItem* item) const
{
	auto row = 0;
	auto mapped = mappedItem(item);
	if (item == mapped)
	{
		return ItemIndex(row, mapped);
	}

	std::vector<const ReflectedPropertyItem*> ancestry;
	{
		auto ancestor = item;
		while (true)
		{
			ancestry.insert(ancestry.begin(), ancestor);
			if (ancestor == nullptr || ancestor == mapped)
			{
				break;
			}
			ancestor = parentProperty(ancestor);
		}
	}

	auto mappedItemsIt = mappedItems_.find(mapped);
	TF_ASSERT(mappedItemsIt != mappedItems_.end());
	auto mapping = mappedItemsIt->second.get();

	for (; row < static_cast<int>(mapping->children_->size()); ++row)
	{
		auto propertyItem = dynamic_cast<const ReflectedPropertyItem*>(mapping->children_->at(row));
		if (propertyItem == nullptr)
		{
			continue;
		}

		auto commonParent = propertyItem;
		auto ancestryIt = ancestry.end();
		while (ancestryIt == ancestry.end())
		{
			commonParent = parentProperty(commonParent);
			ancestryIt = std::find(ancestry.begin(), ancestry.end(), commonParent);
		}

		if (++ancestryIt == ancestry.end())
		{
			break;
		}
		auto ancestor = *ancestryIt;

		auto propertiesIt = properties_.find(commonParent);
		TF_ASSERT(propertiesIt != properties_.end());
		auto& siblings = propertiesIt->second;

		auto it1 = std::find_if(siblings.begin(), siblings.end(), [ancestor](const ReflectedPropertyItemPtr & sibling) { return sibling.get() == ancestor; });
		TF_ASSERT(it1 != siblings.end());
		auto it2 = std::find_if(siblings.begin(), siblings.end(), [propertyItem](const ReflectedPropertyItemPtr & sibling) { return sibling.get() == propertyItem; });
		TF_ASSERT(it2 != siblings.end());
		if (std::distance(siblings.begin(), it1) < std::distance(siblings.begin(), it2))
		{
			break;
		}
	}
	
	return ItemIndex(row, mapped);
}

bool ReflectedTreeModel::isMapped(const ReflectedPropertyItem* item) const
{
	return mappedItems_.find(item) != mappedItems_.end();
}

const AbstractItem* ReflectedTreeModel::mappedItem(const ReflectedPropertyItem* item) const
{
	while (item != nullptr && !isMapped(item))
	{
		item = parentProperty(item);
	}
	return item;
}

std::unique_ptr<ReflectedPropertyItem> ReflectedTreeModel::makeProperty(const std::shared_ptr< const IPropertyPath > & path) const
{
	return std::unique_ptr<ReflectedPropertyItem>(new ReflectedPropertyItem(*this, path, recordHistory_));
}

const ReflectedTreeModel::Properties& ReflectedTreeModel::getProperties(const ReflectedPropertyItem* item)
{
	auto propertiesIt = properties_.find(item);
	if (propertiesIt != properties_.end())
	{
		return propertiesIt->second;
	}

	auto& properties = properties_[item];

	auto object = object_;
	auto definitionManager = get<IDefinitionManager>();
	auto definition = definitionManager->getObjectDefinition(object);
	std::shared_ptr< const IPropertyPath > path = nullptr;

	if (item != nullptr)
	{
		path = item->getPath();

		auto propertyAccessor = definition->bindProperty(path, object);
		if (!propertyAccessor.canGetValue())
		{
			return properties;
		}

		auto value = propertyAccessor.getValue();
		if (value.tryCast(object))
		{
			object = reflectedRoot(object, *definitionManager);
			definition = definitionManager->getObjectDefinition(object);
		}
		else
		{
			object = nullptr;
			Collection collection;
			if (value.tryCast(collection))
			{
				auto it = collection.begin();
				int i = 0;
				for (; it != collection.end(); ++it, ++i)
				{
					auto childPath = path->generateChildPath(path, it.key());
					properties.emplace_back(makeProperty(childPath));
				}
				return properties;
			}
		}
	}

	if (object == nullptr || definition == nullptr)
	{
		return properties;
	}

	for (const auto& property : definition->allProperties())
	{
		properties.emplace_back(
			makeProperty(property->generatePropertyName(path)));
	}

	return properties;
}

void ReflectedTreeModel::clearProperties(const ReflectedPropertyItem* item)
{
	auto propertiesIt = properties_.find(item);
	if (propertiesIt == properties_.end())
	{
		return;
	}

	auto& properties = propertiesIt->second;
	for (auto& property : properties)
	{
		clearProperties(property.get());
	}
	properties_.erase(propertiesIt);
}

const ReflectedPropertyItem* ReflectedTreeModel::findProperty(const ObjectHandle& object, const std::string& path) const
{
	// NEW
	if (std::dynamic_pointer_cast<ObjectReference>(object.storage()) != nullptr)
	{
		for (auto propertiesIt = properties_.begin(); propertiesIt != properties_.end(); ++propertiesIt)
		{
			auto& properties = propertiesIt->second;
			for (auto propertyIt = properties.begin(); propertyIt != properties.end(); ++propertyIt)
			{
				auto property = propertyIt->get();
				if (property == nullptr)
				{
					continue;
				}

				if (property->objectReference_ == object && property->referencePath_ == path)
				{
					return property;
				}
			}
		}

		return nullptr;
	}

	// OLD
	if (object != getObject())
	{
		return nullptr;
	}

	for (auto propertiesIt = properties_.begin(); propertiesIt != properties_.end(); ++propertiesIt)
	{
		auto& properties = propertiesIt->second;
		for (auto propertyIt = properties.begin(); propertyIt != properties.end(); ++propertyIt)
		{
			auto property = propertyIt->get();
			if (property == nullptr)
			{
				continue;
			}

			if (*property->getPath() == path)
			{
				return property;
			}
		}
	}
	return nullptr;
}

const ReflectedPropertyItem* ReflectedTreeModel::parentProperty(const ReflectedPropertyItem* item) const
{
	auto && parentPath = item->getPath()->getParent();
	if (parentPath == nullptr)
	{
		return nullptr;
	}

	if (auto parentItem = 
			findProperty(getObject(), parentPath->getRecursivePath().str()))
	{
		return parentItem;
	}

	auto definitionManager = get<IDefinitionManager>();
	TF_ASSERT(definitionManager != nullptr);
	auto& object = getObject();
	auto definition = definitionManager->getDefinition(object);
	auto propertyAccessor = definition->bindProperty(parentPath, object);

	auto parentItem = findProperty(propertyAccessor.getRootObject(), propertyAccessor.getFullPath());
	TF_ASSERT(parentItem != nullptr);
	return parentItem;
}

void ReflectedTreeModel::updatePath(ReflectedPropertyItem* item, IPropertyPath::ConstPtr & path)
{
	item->setPath(path);

	auto propertiesIt = properties_.find(item);
	if (propertiesIt == properties_.end())
	{
		return;
	}

	auto definitionManager = get<IDefinitionManager>();
	TF_ASSERT(definitionManager != nullptr);
	auto& object = getObject();
	auto definition = definitionManager->getDefinition(object);
	auto propertyAccessor = definition->bindProperty(item->getPath(), object);
	if (propertyAccessor.canGetValue())
	{
		auto value = propertyAccessor.getValue();

		Collection collection;
		if (value.tryCast(collection))
		{
			auto it = collection.begin();
			int i = 0;
			auto && collectionPath = item->getPath();
			for (; it != collection.end(); ++it, ++i)
			{
				updatePath(
					propertiesIt->second.at(i).get(),
					collectionPath->generateChildPath(collectionPath, it.key()));
			}
		}
	}
}

ReflectedTreeModel::ItemMapping* ReflectedTreeModel::mapItem(const AbstractItem* item)
{
	auto it = mappedItems_.find(item);
	TF_ASSERT(it != mappedItems_.end());
	auto mapping = it->second.get();
	if (mapping->children_ == nullptr)
	{
		mapping->children_ = mapChildren(item);
		for (auto& child : *mapping->children_)
		{
			auto childMapping = new ItemMapping();
			childMapping->parent_ = item;
			mappedItems_.insert(std::make_pair(child, std::unique_ptr<ItemMapping>(childMapping)));
		}
	}
	return mapping;
}

void ReflectedTreeModel::unmapItem(const AbstractItem* item)
{
	auto it = mappedItems_.find(item);
	TF_ASSERT(it != mappedItems_.end());
	auto mapping = it->second.get();
	if (mapping->children_ != nullptr)
	{
		for (auto& child : *mapping->children_)
		{
			unmapItem(child);
			auto childIt = mappedItems_.find(child);
			TF_ASSERT(childIt != mappedItems_.end());
			mappedItems_.erase(childIt);
		}
		clearChildren(item);
		mapping->children_.reset();
	}

	clearChildren(item);
}

void ReflectedTreeModel::firePostItemDataChanged(const ItemIndex& index, int column, ItemRole::Id roleId, Variant value)
{
	postItemDataChanged_(index, column, roleId, value);
}
}
}