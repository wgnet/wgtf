
#ifndef __GRAPHEDITOR_QUICKITEMSMANAGER_H__
#define __GRAPHEDITOR_QUICKITEMSMANAGER_H__

#include <unordered_map>

namespace wgt
{
class ConnectionItem;
class QuickItemsManager
{
public:
    static QuickItemsManager& Instance();

    void RegisterObject(ConnectionItem* item);
    void UnregisterObject(ConnectionItem* item);
    void RepaintItem(size_t uid);

private:
    std::unordered_map<size_t, ConnectionItem*> items;
};
} // end namespace wgt
#endif // __GRAPHEDITOR_QUICKITEMSMANAGER_H__
