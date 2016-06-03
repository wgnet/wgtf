
#ifndef __GRAPHEDITOR_BASEMODEL_H__
#define __GRAPHEDITOR_BASEMODEL_H__

#include <core_data_model/i_list_model.hpp>
#include <core_data_model/i_item.hpp>
#include <core_data_model/i_item_role.hpp>

namespace wgt
{
template <typename T>
class BaseModel : public IListModel
{
    class BaseItem;
    typedef std::vector<BaseItem*> TCollection;
    typedef typename TCollection::iterator TItemIter;
    typedef typename TCollection::const_iterator TConstItemIter;

    class BaseItem : public IItem
    {
    public:
        BaseItem(ObjectHandleT<T> object_)
            : object(object_)
        {
        }

        const char* getDisplayText(int /*column*/) const override
        {
            return nullptr;
        }

        ThumbnailData getThumbnail(int /*column*/) const override
        {
            return nullptr;
        }

        Variant getData(int /*column*/, size_t roleId) const override
        {
            if (ValueRole::roleId_ == roleId)
                return ObjectHandle(object);

            return Variant();
        }

        bool setData(int /*column*/, size_t /*roleId*/, const Variant& /*data*/) override
        {
            return false;
        }

        T* GetObject()
        {
            return object.get();
        }

    private:
        ObjectHandleT<T> object;
    };

public:
    BaseModel()
	{

	}

    BaseModel(std::vector<ObjectHandleT<T>>&& object)
    {
        for (ObjectHandleT<T> const& item : object)
            items.push_back(new BaseItem(item));
    }

    void AddItem(ObjectHandleT<T> const& object)
    {
        size_t index = items.size();
        signalPreItemsInserted(index, 1);
        items.push_back(new BaseItem(object));
        signalPostItemsInserted(index, 1);
    }

    void RemoveItem(ObjectHandleT<T> const& object)
    {
        TItemIter iter = std::find_if(items.begin(), items.end(), [&object](BaseItem* item) {
            return item->GetObject() == object.get();
        });

        if (iter == items.end())
        {
            return;
        }

        size_t index = std::distance(items.begin(), iter);
        signalPreItemsRemoved(index, 1);
        items.erase(iter);
        signalPostItemsRemoved(index, 1);
    }

    ~BaseModel()
    {
        for_each(items.begin(), items.end(), [](BaseItem* item) { delete item; });
        items.clear();
    }

    IItem* item(size_t index) const override
    {
        assert(index < items.size());
        return items[index];
    }

    size_t index(const IItem* item) const override
    {
        TConstItemIter iter = std::find(items.begin(), items.end(), item);
        assert(iter != items.end());
        return std::distance(items.begin(), iter);
    }

    bool empty() const override
    {
        return items.empty();
    }

    size_t size() const override
    {
        return items.size();
    }

	int columnCount() const override
	{
		return 1;
	}

protected:
    TCollection items;
};
} // end namespace wgt
#endif // __GRAPHEDITOR_BASEMODEL_H__
