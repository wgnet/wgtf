#include "reflected_tree_model.hpp"

#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/fixed_memory_stream.hpp"

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

namespace ReflectedTreeModelDetails
{
const char* itemMimeKey = "application/reflectedtreemodel-item";
}

namespace proto
{
class ReflectedTreeModelPropertyListener : public PropertyAccessorListener
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
		auto item = model_.findProperty(accessor.getObject(), accessor.getFullPath());
		if (item == nullptr)
		{
			return;
		}

		auto definitionManager = model_.getDefinitionManager();

		TypeId typeId = accessor.getType();
		bool isReflectedObject =
		typeId.isPointer() && definitionManager->getDefinition(typeId.removePointer().getName()) != nullptr;
		if (isReflectedObject)
		{
			const IClassDefinition* definition = nullptr;
			ObjectHandle handle;
			if (value.tryCast(handle))
			{
				definition = handle.getDefinition(*definitionManager);
			}

			const auto index = model_.index(item);
			const int column = 0;
			const ItemRole::Id roleId = DefinitionRole::roleId_;
			const Variant value = ObjectHandle(definition);
			model_.preItemDataChanged_(index, column, roleId, value);
			return;
		}

		const auto index = model_.index(item);
		const int column = 0;
		const ItemRole::Id roleId = ValueRole::roleId_;
		model_.preItemDataChanged_(index, column, roleId, value);
	}

	virtual void postSetValue(const PropertyAccessor& accessor, const Variant& value) override
	{
		auto item = model_.findProperty(accessor.getObject(), accessor.getFullPath());
		if (item == nullptr)
		{
			return;
		}

		auto definitionManager = model_.getDefinitionManager();

		TypeId typeId = accessor.getType();
		bool isReflectedObject =
		typeId.isPointer() && definitionManager->getDefinition(typeId.removePointer().getName()) != nullptr;
		if (isReflectedObject)
		{
			const IClassDefinition* definition = nullptr;
			ObjectHandle handle;
			if (value.tryCast(handle))
			{
				definition = handle.getDefinition(*definitionManager);
			}
			model_.unmapItem(item);

			const auto index = model_.index(item);
			const int column = 0;
			const ItemRole::Id roleId = DefinitionRole::roleId_;
			const Variant value = ObjectHandle(definition);
			model_.postItemDataChanged_(index, column, roleId, value);
			return;
		}

		const auto index = model_.index(item);
		const int column = 0;
		const ItemRole::Id roleId = ValueRole::roleId_;
		model_.postItemDataChanged_(index, column, roleId, value);
	}

	virtual void preInsert(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		auto item = model_.findProperty(accessor.getObject(), accessor.getFullPath());
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
		auto item = model_.findProperty(accessor.getObject(), accessor.getFullPath());
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

		auto& itemObject = item->getObject();
		auto& itemPath = item->getPath();
		auto& itemFullPath = item->getFullPath();

		auto value = accessor.getValue();
		Collection collection;
		assert(value.tryCast(collection));
		size_t i = 0;
		auto collectionIt = collection.begin();
		for (; i < index; ++i, ++collectionIt)
		{
		}

		// insert into parent mapping
		auto properties = propertiesIt->second;
		auto propertyIt = properties->begin() + index;
		auto parentMapping = model_.mapItem(parent);
		auto mappingIt = parentMapping->children_->begin() + row;
		for (; i < index + count; ++i, ++collectionIt)
		{
			assert(collectionIt != collection.end());

			auto indexKey = i;
			collectionIt.key().tryCast(indexKey);
			std::string indexPath = "[" + std::to_string(static_cast<int>(indexKey)) + "]";
			std::string path = itemPath + indexPath;
			std::string fullPath = itemFullPath + indexPath;
			auto propertyPtr = model_.makeProperty(itemObject, path, fullPath);
			auto property = propertyPtr.get();

			propertyIt = properties->emplace(propertyIt, std::move(propertyPtr)) + 1;
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
				assert(propertyIt != properties->end());

				auto indexKey = i;
				collectionIt.key().tryCast(indexKey);
				std::string path = itemPath + "[" + std::to_string(static_cast<int>(indexKey)) + "]";
				model_.updatePath(propertyIt->get(), path);
			}
		}

		model_.postRowsInserted_(model_.index(parent), row, static_cast<int>(count));
	}

	virtual void preErase(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		auto item = model_.findProperty(accessor.getObject(), accessor.getFullPath());
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
		auto item = model_.findProperty(accessor.getObject(), accessor.getFullPath());
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

		auto& itemPath = item->getPath();

		auto value = accessor.getValue();
		Collection collection;
		assert(value.tryCast(collection));
		size_t i = 0;
		auto collectionIt = collection.begin();
		for (; i < index; ++i, ++collectionIt)
		{
		}

		// delete properties
		auto properties = propertiesIt->second;
		auto propertyIt = properties->begin() + index;
		auto parentMapping = model_.mapItem(parent);
		assert(parentMapping->children_->size() >= row + count);
		auto mappingIt = parentMapping->children_->begin() + row;
		for (size_t j = 0; j < count; ++j)
		{
			model_.unmapItem(propertyIt->get());
			assert(propertyIt != properties->end());
			assert(mappingIt != parentMapping->children_->end());
			propertyIt = properties->erase(propertyIt);
			mappingIt = parentMapping->children_->erase(mappingIt);
		}

		// update property keys
		if (!collection.isMapping())
		{
			for (; i < collection.size(); ++i, ++collectionIt, ++propertyIt)
			{
				assert(propertyIt != properties->end());

				auto indexKey = i;
				collectionIt.key().tryCast(indexKey);
				std::string path = itemPath + "[" + std::to_string(static_cast<int>(indexKey)) + "]";
				model_.updatePath(propertyIt->get(), path);
			}
		}

		model_.postRowsRemoved_(model_.index(parent), row, static_cast<int>(count));
	}

private:
	ReflectedTreeModel& model_;
};

ReflectedTreeModel::ReflectedTreeModel(IComponentContext& context, const ObjectHandle& object)
    : definitionManager_(context), controller_(context), commandManager_(context), assetManager_(context),
      listener_(new ReflectedTreeModelPropertyListener(*this))
{
	assert(definitionManager_ != nullptr);
	definitionManager_->registerPropertyAccessorListener(listener_);

	auto rootMapping = new ItemMapping();
	mappedItems_.insert(std::make_pair(nullptr, std::unique_ptr<ItemMapping>(rootMapping)));

	setObject(object);
}

ReflectedTreeModel::~ReflectedTreeModel()
{
	setObject(nullptr);

	if (definitionManager_ != nullptr)
	{
		definitionManager_->deregisterPropertyAccessorListener(listener_);
	}
}

void ReflectedTreeModel::setObject(const ObjectHandle& object)
{
	preModelChanged_();

	unmapItem(nullptr);
	object_ = object;

	postModelChanged_();
}

MimeData ReflectedTreeModel::mimeData(std::vector<AbstractItemModel::ItemIndex>& indices)
{
	if (indices.empty())
	{
		return MimeData();
	}

	MimeData mimeData;
	ResizingMemoryStream stream;
	BinaryStream s(stream);
	s << indices.size();
	for (auto& index : indices)
	{
		s << index.row_;
		s << static_cast<void*>(const_cast<AbstractItem*>(index.parent_));
	}

	std::string data = stream.takeBuffer();
	mimeData[ReflectedTreeModelDetails::itemMimeKey] = std::vector<char>(data.begin(), data.end());
	return mimeData;
}

std::vector<std::string> ReflectedTreeModel::mimeTypes() const
{
	std::vector<std::string> types;
	types.push_back(ReflectedTreeModelDetails::itemMimeKey);
	return types;
}

bool ReflectedTreeModel::canDropMimeData(const MimeData& mimeData, DropAction action,
                                         const AbstractItemModel::ItemIndex& index) const
{
	auto it = mimeData.find(ReflectedTreeModelDetails::itemMimeKey);
	if (it == mimeData.end())
	{
		return false;
	}

	auto parentItem = index.parent_;
	if (parentItem == nullptr)
	{
		return false;
	}

	const auto collectionVariant = parentItem->getData(0, 0, ItemRole::valueId);
	Collection collection;
	if (!collectionVariant.tryCast<Collection>(collection))
	{
		return false;
	}

	if (collection.isMapping())
	{
		return false;
	}

	FixedMemoryStream stream(it->second.data(), it->second.size());
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

bool ReflectedTreeModel::dropMimeData(const MimeData& mimeData, DropAction action,
                                      const AbstractItemModel::ItemIndex& index)
{
	auto it = mimeData.find(ReflectedTreeModelDetails::itemMimeKey);
	if (it == mimeData.end())
	{
		return false;
	}

	auto parentItem = index.parent_;
	if (parentItem == nullptr)
	{
		return false;
	}

	const auto collectionVariant = parentItem->getData(0, 0, ItemRole::valueId);
	Collection collection;
	if (!collectionVariant.tryCast<Collection>(collection))
	{
		return false;
	}

	if (collection.isMapping())
	{
		return false;
	}

	std::vector<int> rows;
	FixedMemoryStream stream(it->second.data(), it->second.size());
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

	commandManager_->beginBatchCommand();
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
	commandManager_->endBatchCommand();
	return true;
}

AbstractItem* ReflectedTreeModel::item(const ItemIndex& index) const
{
	auto parentMapping = const_cast<ReflectedTreeModel*>(this)->mapItem(index.parent_);
	auto item = parentMapping->children_->at(index.row_);
	return const_cast<AbstractItem*>(item);
}

AbstractTreeModel::ItemIndex ReflectedTreeModel::index(const AbstractItem* item) const
{
	// don't call mapItem for the passed in items as we do not need to iterate its children at this stage
	auto it = mappedItems_.find(item);
	assert(it != mappedItems_.end());
	auto mapping = it->second.get();
	auto parent = mapping->parent_;

	auto parentMapping = const_cast<ReflectedTreeModel*>(this)->mapItem(parent);
	auto childIt = std::find(parentMapping->children_->begin(), parentMapping->children_->end(), item);
	assert(childIt != parentMapping->children_->end());
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

std::vector<std::string> ReflectedTreeModel::roles() const
{
	std::vector<std::string> roles;
	roles.push_back(ItemRole::valueName);
	roles.push_back(ItemRole::valueTypeName);
	roles.push_back(ItemRole::keyName);
	roles.push_back(ItemRole::keyTypeName);
	roles.push_back(ItemRole::isCollectionName);
	roles.push_back(ItemRole::elementValueTypeName);
	roles.push_back(ItemRole::elementKeyTypeName);
	roles.push_back(ItemRole::readOnlyName);
	roles.push_back(ItemRole::enabledName);
	roles.push_back(ItemRole::multipleValuesName);
	roles.push_back(ItemRole::assetModelName);
	roles.push_back(ItemRole::nameName);
	// DEPRECATED
	roles.push_back(EnumModelRole::roleName_);
	roles.push_back(DefinitionRole::roleName_);
	roles.push_back(DefinitionModelRole::roleName_);
	roles.push_back(ObjectRole::roleName_);
	roles.push_back(RootObjectRole::roleName_);
	roles.push_back(MinValueRole::roleName_);
	roles.push_back(MaxValueRole::roleName_);
	roles.push_back(StepSizeRole::roleName_);
	roles.push_back(DecimalsRole::roleName_);
	roles.push_back(IndexPathRole::roleName_);
	roles.push_back(UrlIsAssetBrowserRole::roleName_);
	roles.push_back(UrlDialogTitleRole::roleName_);
	roles.push_back(UrlDialogDefaultFolderRole::roleName_);
	roles.push_back(UrlDialogNameFiltersRole::roleName_);
	roles.push_back(UrlDialogSelectedNameFilterRole::roleName_);
	roles.push_back(IsReadOnlyRole::roleName_);
	roles.push_back(IsEnumRole::roleName_);
	roles.push_back(IsThumbnailRole::roleName_);
	roles.push_back(IsSliderRole::roleName_);
	roles.push_back(IsColorRole::roleName_);
	roles.push_back(IsUrlRole::roleName_);
	roles.push_back(IsActionRole::roleName_);
	roles.push_back(DescriptionRole::roleName_);
	roles.push_back(ThumbnailRole::roleName_);
	return roles;
}

Connection ReflectedTreeModel::connectPreModelReset(VoidCallback callback)
{
	return preModelChanged_.connect(callback);
}

Connection ReflectedTreeModel::connectPostModelReset(VoidCallback callback)
{
	return postModelChanged_.connect(callback);
}

Connection ReflectedTreeModel::connectPreItemDataChanged(AbstractTreeModel::DataCallback callback)
{
	return preItemDataChanged_.connect(callback);
}

Connection ReflectedTreeModel::connectPostItemDataChanged(AbstractTreeModel::DataCallback callback)
{
	return postItemDataChanged_.connect(callback);
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

std::unique_ptr<ReflectedTreeModel::Children> ReflectedTreeModel::getChildren(const AbstractItem* item)
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

AbstractTreeModel::ItemIndex ReflectedTreeModel::childHint(const AbstractItem* item)
{
	return ItemIndex(0, item);
}

std::unique_ptr<ReflectedPropertyItem> ReflectedTreeModel::makeProperty(const ObjectHandle& object,
                                                                        const std::string& path,
                                                                        const std::string& fullPath) const
{
	return std::unique_ptr<ReflectedPropertyItem>(new ReflectedPropertyItem(*this, object, path, fullPath));
}

const ReflectedTreeModel::Properties& ReflectedTreeModel::getProperties(const ReflectedPropertyItem* item)
{
	auto propertiesIt = properties_.find(item);
	if (propertiesIt != properties_.end())
	{
		auto properties = propertiesIt->second;
		assert(properties != nullptr);
		return *properties;
	}

	auto properties = new Properties();
	properties_.insert(std::make_pair(item, properties));

	auto object = object_;
	if (item != nullptr)
	{
		object = nullptr;

		auto& itemObject = item->getObject();
		auto& itemPath = item->getPath();
		auto& itemFullPath = item->getFullPath();

		assert(definitionManager_ != nullptr);
		auto itemDefinition = itemObject.getDefinition(*definitionManager_);
		auto propertyAccessor = itemDefinition->bindProperty(itemPath.c_str(), itemObject);
		if (propertyAccessor.canGetValue())
		{
			auto value = propertyAccessor.getValue();
			if (!value.tryCast(object))
			{
				Collection collection;
				if (value.tryCast(collection))
				{
					// TODO: [NGT-2930] This is causing a crash when the collection is returned by value
					object = itemObject;
					auto it = collection.begin();
					int i = 0;
					for (; it != collection.end(); ++it, ++i)
					{
						auto indexKey = i;
						it.key().tryCast(indexKey);
						std::string indexPath = "[" + std::to_string(static_cast<int>(indexKey)) + "]";
						std::string path = itemPath + indexPath;
						std::string fullPath = itemFullPath + indexPath;
						properties->emplace_back(makeProperty(object, path, fullPath));
					}
					return *properties;
				}
			}
		}
	}

	if (object == nullptr)
	{
		return *properties;
	}

	assert(definitionManager_ != nullptr);
	object = reflectedRoot(object, *definitionManager_);

	if (object == nullptr)
	{
		return *properties;
	}

	if (object == nullptr)
	{
		return *properties;
	}

	auto definition = object.getDefinition(*definitionManager_);
	if (definition == nullptr)
	{
		return *properties;
	}

	for (auto property : definition->allProperties())
	{
		std::string path = property->getName();
		std::string fullPath = path;

		if (item != nullptr)
		{
			fullPath = item->getFullPath();
			fullPath.append(".");
			if (item->getFullPath() != path)
			{
				fullPath.append(path);
			}
		}
		properties->emplace_back(makeProperty(object, path, fullPath));
	}

	return *properties;
}

void ReflectedTreeModel::clearProperties(const ReflectedPropertyItem* item)
{
	auto propertiesIt = properties_.find(item);
	if (propertiesIt == properties_.end())
	{
		return;
	}

	auto properties = propertiesIt->second;
	assert(properties != nullptr);
	properties_.erase(propertiesIt);
	for (auto& property : *properties)
	{
		clearProperties(property.get());
	}
	delete properties;
}

const ReflectedPropertyItem* ReflectedTreeModel::findProperty(const ObjectHandle& object, const std::string& path) const
{
	for (auto propertyIt = properties_.begin(); propertyIt != properties_.end(); ++propertyIt)
	{
		auto property = propertyIt->first;
		if (property == nullptr)
		{
			continue;
		}

		if (property->getObject() == object && property->getPath() == path)
		{
			return property;
		}
	}
	return nullptr;
}

void ReflectedTreeModel::updatePath(ReflectedPropertyItem* item, const std::string& path)
{
	item->setPath(path);

	auto propertiesIt = properties_.find(item);
	if (propertiesIt == properties_.end())
	{
		return;
	}

	auto& itemObject = item->getObject();
	auto& itemPath = item->getPath();

	assert(definitionManager_ != nullptr);
	auto itemDefinition = itemObject.getDefinition(*definitionManager_);
	auto propertyAccessor = itemDefinition->bindProperty(itemPath.c_str(), itemObject);
	if (propertyAccessor.canGetValue())
	{
		auto value = propertyAccessor.getValue();

		Collection collection;
		if (value.tryCast(collection))
		{
			auto it = collection.begin();
			int i = 0;
			for (; it != collection.end(); ++it, ++i)
			{
				auto indexKey = i;
				it.key().tryCast(indexKey);
				std::string path = itemPath + "[" + std::to_string(static_cast<int>(indexKey)) + "]";
				updatePath(propertiesIt->second->at(i).get(), path);
			}
		}
	}
}

ReflectedTreeModel::ItemMapping* ReflectedTreeModel::mapItem(const AbstractItem* item)
{
	auto it = mappedItems_.find(item);
	assert(it != mappedItems_.end());
	auto mapping = it->second.get();
	if (mapping->children_ == nullptr)
	{
		mapping->children_ = getChildren(item);
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
	assert(it != mappedItems_.end());
	auto mapping = it->second.get();
	if (mapping->children_ != nullptr)
	{
		for (auto& child : *mapping->children_)
		{
			unmapItem(child);
			auto childIt = mappedItems_.find(child);
			assert(childIt != mappedItems_.end());
			mappedItems_.erase(childIt);
		}
		clearChildren(item);
		mapping->children_.reset();
	}
}
}
}