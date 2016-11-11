#ifndef I_TREE_ITEM_HPP
#define I_TREE_ITEM_HPP

#include "i_list_item.hpp"
#include "cstdmf/bw_vector.hpp"

namespace wgt
{
BW_BEGIN_NAMESPACE

class IRoleBinder;

class ITreeItem : public IListItem
{
public:
	enum Role
	{
		NameRole,
		ValueRole,
		MinRole,
		MaxRole,
		WidthRole,
		HeightRole,
		ValueTypeRole,
		ValueRealTypeRole,
		HasChildrenRole,
		LevelRole,
		ExpandedRole,
		SelectedRole,
		DataModelRole,
		ContextMenuRole
	};

	virtual ~ITreeItem()
	{
	}

	virtual const void* data(const IRoleBinder* roleBinder, int role) const = 0;

	virtual void setData(const IRoleBinder* roleBinder, int role, const void* data) = 0;

	virtual TypeId type(const IRoleBinder* roleBinder, int role) const = 0;
	virtual void setDataItem(void* dataItem) = 0;

	virtual int getLevel() const = 0;

	virtual bool equals(const BW::string& path) = 0;

	virtual void emitSelected() const
	{
	}
	virtual void emitContextMenuSelection(void* selection, const BW::vector<const ITreeItem*>& selectedItems) const
	{
	}
};

BW_END_NAMESPACE
} // end namespace wgt
#endif // I_TREE_ITEM_HPP
