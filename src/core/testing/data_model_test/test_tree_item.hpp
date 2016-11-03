#ifndef DATA_MODEL_TEST_TREE_ITEM_HPP
#define DATA_MODEL_TEST_TREE_ITEM_HPP

#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"

#include <memory>

namespace wgt
{
class TestTreeItem: public AbstractTreeItem
{
public:
	explicit TestTreeItem(const std::string& name, const AbstractTreeItem* parent);
	virtual ~TestTreeItem();

	const AbstractTreeItem* getParent() const;
	void setParent( AbstractTreeItem* parent);

	virtual Variant getData( int column, ItemRole::Id roleId ) const override;
	virtual bool setData( int column, ItemRole::Id roleId, const Variant& data ) override;

	virtual Connection connectPreDataChanged( DataCallback callback ) override;
	virtual Connection connectPostDataChanged( DataCallback callback ) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif
