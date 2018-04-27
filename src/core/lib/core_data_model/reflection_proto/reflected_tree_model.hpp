#ifndef PROTO_REFLECTED_TREE_MODEL_HPP
#define PROTO_REFLECTED_TREE_MODEL_HPP

#include "reflected_property_item.hpp"
#include "core_data_model/abstract_item_model.hpp"

#include <unordered_map>
#include <memory>
#include <vector>
#include "core_reflection/property_accessor_listener.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/interfaces/i_property_path.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_serialization/i_file_system.hpp"
#include "asset_manager/i_asset_manager.hpp"

namespace wgt
{
namespace proto
{
class ReflectedTreeModel : public AbstractTreeModel,
                           protected Depends<IDefinitionManager, IReflectionController, ICommandManager, IAssetManager>
{
public:
	ReflectedTreeModel(const ObjectHandle& object = nullptr);
    virtual ~ReflectedTreeModel();

	void setRecordHistory(bool recordHistory);
	void setObject(const ObjectHandle& object);

	const ObjectHandle& getObject() const
	{
		return object_;
	}

	// AbstractTreeModel
	virtual AbstractItem* item(const ItemIndex& index) const override;
	virtual ItemIndex index(const AbstractItem* item) const override;

	virtual int rowCount(const AbstractItem* item) const override;
	virtual int columnCount() const override;
	int getColumnCount() const;

	virtual MimeData mimeData(std::vector<AbstractItemModel::ItemIndex>& indices) override;

	void iterateMimeTypes(const std::function<void(const char*)>& iterFunc) const override;
	virtual std::vector<std::string> mimeTypes() const override;
	virtual bool canDropMimeData(const MimeData& mimeData, DropAction action,
	                             const AbstractItemModel::ItemIndex& index) const override;
	virtual bool dropMimeData(const MimeData& mimeData, DropAction action,
	                          const AbstractItemModel::ItemIndex& index) override;

	virtual std::vector<std::string> roles() const override;
	void iterateRoles(const std::function<void(const char*)>& iterFunc) const override;

	virtual Connection connectPreItemDataChanged(DataCallback callback) override;
	virtual Connection connectPostItemDataChanged(DataCallback callback) override;
	virtual Connection connectPreLayoutChanged(LayoutCallback callback) override;
	virtual Connection connectPostLayoutChanged(LayoutCallback callback) override;
	virtual Connection connectModelChanged(VoidCallback callback) override;
	virtual Connection connectPreModelReset(VoidCallback callback) override;
	virtual Connection connectPostModelReset(VoidCallback callback) override;
	virtual Connection connectPreRowsInserted(RangeCallback callback) override;
	virtual Connection connectPostRowsInserted(RangeCallback callback) override;
	virtual Connection connectPreRowsRemoved(RangeCallback callback) override;
	virtual Connection connectPostRowsRemoved(RangeCallback callback) override;

	void firePostItemDataChanged(const ItemIndex& index, int column, ItemRole::Id roleId, Variant value);

protected:
	typedef std::vector<const AbstractItem*> Children;
	virtual std::unique_ptr<Children> mapChildren(const AbstractItem* item);
	virtual void clearChildren(const AbstractItem* item);
	virtual ItemIndex childHint(const ReflectedPropertyItem* item) const;

	bool isMapped(const ReflectedPropertyItem* item) const;
	virtual const AbstractItem* mappedItem(const ReflectedPropertyItem* item) const;

	virtual ReflectedPropertyItemPtr makeProperty(IPropertyPath::ConstPtr & path) const;

	typedef std::vector<ReflectedPropertyItemPtr> Properties;
	const Properties& getProperties(const ReflectedPropertyItem* item);
	void clearProperties(const ReflectedPropertyItem* item);
	const ReflectedPropertyItem* findProperty(const ObjectHandle& object, const std::string& path) const;
	const ReflectedPropertyItem* parentProperty(const ReflectedPropertyItem* item) const;

	void updatePath(ReflectedPropertyItem* item, IPropertyPath::ConstPtr & path );

	struct ItemMapping
	{
		const AbstractItem* parent_;
		std::unique_ptr<Children> children_;
	};
	ItemMapping* mapItem(const AbstractItem* item);
	void unmapItem(const AbstractItem* item);

	std::unordered_map<const ReflectedPropertyItem*, Properties> properties_;

private:
	bool canDropItemMimeData(const std::vector<char>& data,
		DropAction action,
		const AbstractItemModel::ItemIndex& index) const;
	bool canDropTextMimeData(const std::vector<char>& data,
		DropAction action,
		const AbstractItemModel::ItemIndex& index) const;

	bool canDropFilePathMimeData(const std::vector<char>& data,
		DropAction action,
		const AbstractItemModel::ItemIndex& index) const;

	bool dropItemMimeData(const std::vector<char>& data,
		DropAction action,
	    const AbstractItemModel::ItemIndex& index);
	bool dropTextMimeData(const std::vector<char>& data,
		DropAction action,
	    const AbstractItemModel::ItemIndex& index);

	bool dropFilePathMimeData(const std::vector<char>& data,
		DropAction action,
		const AbstractItemModel::ItemIndex& index);

	ObjectHandle object_;
	bool recordHistory_;

	std::unordered_map<const AbstractItem*, std::unique_ptr<ItemMapping>> mappedItems_;

	std::shared_ptr<PropertyAccessorListener> listener_;

	Signal<VoidSignature> preModelReset_;
	Signal<VoidSignature> postModelReset_;
	Signal<VoidSignature> modelChanged_;

	Signal<AbstractTreeModel::LayoutChangedSignature> preLayoutChanged_;
	Signal<AbstractTreeModel::LayoutChangedSignature> postLayoutChanged_;

	Signal<AbstractTreeModel::DataSignature> preItemDataChanged_;
	Signal<AbstractTreeModel::DataSignature> postItemDataChanged_;

	Signal<AbstractTreeModel::RangeSignature> preRowsInserted_;
	Signal<AbstractTreeModel::RangeSignature> postRowsInserted_;

	Signal<AbstractTreeModel::RangeSignature> preRowsRemoved_;
	Signal<AbstractTreeModel::RangeSignature> postRowsRemoved_;

	friend class ReflectedTreeModelPropertyListener;
};
}
}

#endif // PROTO_ABSTRACT_REFLECTED_MODEL_HPP