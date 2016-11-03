#ifndef PROTO_REFLECTED_TREE_MODEL_HPP
#define PROTO_REFLECTED_TREE_MODEL_HPP

#include "reflected_property_item.hpp"
#include "core_data_model/abstract_item_model.hpp"

#include <map>
#include <memory>
#include <vector>
#include "core_reflection/property_accessor_listener.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/di_ref.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_command_system/i_command_manager.hpp"

namespace wgt
{
namespace proto
{
class ReflectedTreeModel : public AbstractTreeModel
    {
	public:
	    ReflectedTreeModel(IComponentContext& context, const ObjectHandle& object);
	    virtual ~ReflectedTreeModel();

	    IDefinitionManager* getDefinitionManager() const
	    {
		    return definitionManager_.get();
	    }
	    IReflectionController* getController() const
	    {
		    return controller_.get();
	    }
	    const ObjectHandle& getObject() const
	    {
		    return object_;
	    }

	    // AbstractTreeModel
	    virtual AbstractItem* item(const ItemIndex& index) const override;
	    virtual ItemIndex index(const AbstractItem* item) const override;

	    virtual int rowCount(const AbstractItem* item) const override;
	    virtual int columnCount() const override;

	    virtual MimeData mimeData(std::vector<AbstractItemModel::ItemIndex>& indices) override;
	    virtual std::vector<std::string> mimeTypes() const override;
	    virtual bool canDropMimeData(const MimeData& mimeData, DropAction action, const AbstractItemModel::ItemIndex& index) const override;
	    virtual bool dropMimeData(const MimeData& mimeData, DropAction action, const AbstractItemModel::ItemIndex& index) override;

	    virtual std::vector<std::string> roles() const override;

	    virtual Connection connectPreItemDataChanged(DataCallback callback) override;
	    virtual Connection connectPostItemDataChanged(DataCallback callback) override;
	    virtual Connection connectPreRowsInserted(RangeCallback callback) override;
	    virtual Connection connectPostRowsInserted(RangeCallback callback) override;
	    virtual Connection connectPreRowsRemoved(RangeCallback callback) override;
	    virtual Connection connectPostRowsRemoved(RangeCallback callback) override;

	protected:
	    typedef std::vector<const AbstractItem*> Children;
	    virtual std::unique_ptr<Children> getChildren(const AbstractItem* item);
	    virtual void removeChildren(const AbstractItem* item, std::unique_ptr<Children>& children);

	    virtual ItemIndex childHint(const ReflectedPropertyItem* item);

	    typedef std::vector<std::unique_ptr<ReflectedPropertyItem>> Properties;
	    const Properties& enumerateProperties(const ReflectedPropertyItem* item);
	    void clearProperties(const ReflectedPropertyItem* item);
	    const ReflectedPropertyItem* findProperty(const ObjectHandle& object, const std::string& path) const;

	    void updatePath(ReflectedPropertyItem* item, const std::string& path);

	    struct ItemMapping
	    {
		    const AbstractItem* parent_;
		    std::unique_ptr<Children> children_;
	    };
	    ItemMapping* mapItem(const AbstractItem* item);
	    void unmapItem(const AbstractItem* item);

	private:
	    DIRef<IDefinitionManager> definitionManager_;
	    DIRef<IReflectionController> controller_;
	    DIRef<ICommandManager> commandManager_;
	    ObjectHandle object_;

	    std::map<const ReflectedPropertyItem*, Properties*> properties_;
	    std::map<const AbstractItem*, std::unique_ptr<ItemMapping>> mappedItems_;

	    std::shared_ptr<PropertyAccessorListener> listener_;

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

#endif //PROTO_ABSTRACT_REFLECTED_MODEL_HPP