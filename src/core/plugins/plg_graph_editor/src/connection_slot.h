
#ifndef __GRAPHEDITOR_CONNECTIONSLOT_H__
#define __GRAPHEDITOR_CONNECTIONSLOT_H__

#include <core_reflection/object_handle.hpp>
#include <core_reflection/reflected_object.hpp>
#include <string>

#include <QPointF>

namespace wgt
{
class GraphNode;

class ConnectionSlot
{
    DECLARE_REFLECTED
public:
    struct Params
    {
        Params(const std::string& title_, const std::string& icon_, ObjectHandleT<GraphNode> parent_)
            : title(std::move(title_))
            , icon(std::move(icon_))
            , parent(parent_)
        {
        }

        std::string title;
        std::string icon;
        ObjectHandleT<GraphNode> parent;
    };

    void Init(Params&& params);

    const std::string& GetTitle() const;
    const std::string& GetIcon() const;
    size_t GetUID() const;

    ObjectHandleT<GraphNode> GetParentNode();

private:
    std::string title;
    std::string icon;
    ObjectHandleT<GraphNode> parent;
};
} // end namespace wgt
#endif // __GRAPHEDITOR_CONNECTIONSLOT_H__
