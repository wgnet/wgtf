
#include "quick_items_manager.h"
#include "connection_item.h"

namespace wgt
{
QuickItemsManager& QuickItemsManager::Instance()
{
    static QuickItemsManager mng;
    return mng;
}

void QuickItemsManager::RegisterObject(ConnectionItem* item)
{
    items.emplace(item->GetUID(), item);
}

void QuickItemsManager::UnregisterObject(ConnectionItem* item)
{
    items.erase(item->GetUID());
}

void QuickItemsManager::RepaintItem(size_t uid)
{
    auto iter = items.find(uid);
    if (iter != items.end())
        iter->second->update();
}
} // end namespace wgt
