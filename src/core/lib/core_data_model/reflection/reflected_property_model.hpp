#ifndef REFLECTED_PROPERTY_MODEL_HPP
#define REFLECTED_PROPERTY_MODEL_HPP

#include "child_creator.hpp"

#include "core_data_model/reflection/property_model_extensions.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/abstract_item.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/type_id.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{
class ReflectedPropertyModel;

class RefPropertyItem : public AbstractItem
{
public:
	RefPropertyItem(ReflectedPropertyModel& model);

	Variant getData(int row, int column, ItemRole::Id roleId) const override;
	bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override;

	Connection connectPreDataChanged(DataCallback callback) override;
	Connection connectPostDataChanged(DataCallback callback) override;

	const std::string& getIndexPath() const;
	IBasePropertyPtr getProperty() const;
	Variant getValue() const;

	const std::vector<std::shared_ptr<const PropertyNode>>& getObjects() const
	{
		return nodes;
	}

	void injectData(ItemRole::Id roleId, const Variant& value);
	Variant getInjectedData(ItemRole::Id roleId);

	AbstractItemModel::ItemIndex getItemIndex() const;
	const RefPropertyItem* getParent() const;

private:
	//![don't call it]
	RefPropertyItem(const RefPropertyItem& other) : model(other.model)
	{
	}
	RefPropertyItem(RefPropertyItem&& other) : model(other.model)
	{
	}
	RefPropertyItem& operator=(const RefPropertyItem& other)
	{
		return *this;
	}
	RefPropertyItem& operator=(RefPropertyItem&& other)
	{
		return *this;
	}
	//![don't call it]

	friend class ReflectedPropertyModel;
	RefPropertyItem* getNonConstParent() const;
	int getPosition() const;
	RefPropertyItem* createChild();

	int getChildCount() const;
	RefPropertyItem* getChild(size_t index) const;
	void removeChild(size_t index);
	void removeChildren();

	void addObject(const std::shared_ptr<const PropertyNode>& node);
	void removeObject(const std::shared_ptr<const PropertyNode>& node);
	void removeObjects();
	bool hasObjects() const;

	Variant evalValue(IDefinitionManager& definitionManager) const;
	void setValue(Variant&& newValue);

	RefPropertyItem(RefPropertyItem* parent, size_t position);

private:
	ReflectedPropertyModel& model;
	RefPropertyItem* parent;
	size_t position;
	std::vector<std::unique_ptr<RefPropertyItem>> children;
	std::vector<std::shared_ptr<const PropertyNode>> nodes;
	Variant itemValue;
	mutable std::string indexPath;

	std::unordered_map<ItemRole::Id, Variant> injectedData;

	Signal<AbstractItem::DataSignature> preDataChanged;
	Signal<AbstractItem::DataSignature> postDataChanged;
};

class ReflectedPropertyModel : public AbstractItemModel
{
public:
	ReflectedPropertyModel(IComponentContext& context);
	~ReflectedPropertyModel();

	void update();

	void setObjects(const std::vector<ObjectHandle>& objects);

	AbstractItem* item(const ItemIndex& index) const override;
	void index(const AbstractItem* item, ItemIndex& o_Index) const override;

	int rowCount(const AbstractItem* item) const override;
	int columnCount(const AbstractItem* item) const override;

	void registerExtension(const std::shared_ptr<ExtensionChain>& extension);
	void unregisterExtension(const std::shared_ptr<ExtensionChain>& extension);

	Connection connectPreItemDataChanged(DataCallback callback) override;
	Connection connectPostItemDataChanged(DataCallback callback) override;
	Connection connectPreRowsInserted(RangeCallback callback) override;
	Connection connectPostRowsInserted(RangeCallback callback) override;
	Connection connectPreRowsRemoved(RangeCallback callback) override;
	Connection connectPostRowsRemoved(RangeCallback callback) override;

private:
	void childAdded(const std::shared_ptr<const PropertyNode>& parent, const std::shared_ptr<const PropertyNode>& node,
	                size_t childPosition);
	void childRemoved(const std::shared_ptr<const PropertyNode>& node);

	const RefPropertyItem* getEffectiveParent(const AbstractItem* modelParent) const;
	RefPropertyItem* getEffectiveParent(AbstractItem* modelParent) const;

	AbstractListModel::ItemIndex getModelParent(const RefPropertyItem* effectiveParent) const;

	void update(RefPropertyItem* item);

	template <typename T>
	std::shared_ptr<T> getExtensionChain() const;

private:
	friend class RefPropertyItem;
	Variant getDataImpl(const RefPropertyItem* item, int column, ItemRole::Id roleId) const;
	bool setDataImpl(RefPropertyItem* item, int column, ItemRole::Id roleId, const Variant& data);

	Depends<IDefinitionManager, ICommandManager> interfacesHolder;
	std::unique_ptr<RefPropertyItem> rootItem;
	std::unordered_map<std::shared_ptr<const PropertyNode>, RefPropertyItem*> nodeToItem;

	ChildCreator childCreator;
	std::map<TypeId, std::shared_ptr<ExtensionChain>> extensions;

	Signal<AbstractItemModel::DataSignature> preDataChanged;
	Signal<AbstractItemModel::DataSignature> postDataChanged;
	Signal<AbstractItemModel::RangeSignature> preRowInserted;
	Signal<AbstractItemModel::RangeSignature> postRowInserted;
	Signal<AbstractItemModel::RangeSignature> preRowRemoved;
	Signal<AbstractItemModel::RangeSignature> postRowRemoved;
};

template <typename Dst, typename Src>
std::shared_ptr<Dst> polymorphCast(std::shared_ptr<Src> ptr)
{
	assert(dynamic_cast<Dst*>(ptr.get()) != nullptr);
	return std::static_pointer_cast<Dst>(ptr);
}

template <typename T>
std::shared_ptr<T> ReflectedPropertyModel::getExtensionChain() const
{
	static_assert(!std::is_same<T, ChildCreatorExtension>::value,
	              "There is no reason to request ChildCreatorExtension");
	static_assert(std::is_base_of<ExtensionChain, T>::value, "ExtensionChain should be base of extension");
	TypeId typeId = TypeId::getType<T>();
	auto iter = extensions.find(typeId);
	if (iter == extensions.end())
	{
		assert(false);
		return nullptr;
	}

	return polymorphCast<T>(iter->second);
}

} // namespace wgt

#endif