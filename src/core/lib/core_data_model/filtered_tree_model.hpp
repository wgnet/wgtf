#ifndef FILTERED_TREE_MODEL_HPP
#define FILTERED_TREE_MODEL_HPP

#include "i_tree_model.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"

#include <memory>

namespace wgt
{
class FilteredTreeModel : public ITreeModel
{
public:
	FilteredTreeModel();
	FilteredTreeModel(const FilteredTreeModel& rhs);
	virtual ~FilteredTreeModel();

	FilteredTreeModel& operator=(const FilteredTreeModel& rhs);

	virtual IItem* item(size_t index, const IItem* parent) const override;
	virtual ItemIndex index(const IItem* item) const override;
	virtual bool empty(const IItem* item) const override;
	virtual size_t size(const IItem* item) const override;
	virtual int columnCount() const override;

	virtual Variant getData(int column, ItemRole::Id roleId) const override;
	virtual bool setData(int column, ItemRole::Id roleId, const Variant& data) override;

	void setSource(ITreeModel* source);
	void setFilter(IItemFilter* filter);

	ITreeModel* getSource();
	const ITreeModel* getSource() const;

	void refresh(bool wait = false);

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif
