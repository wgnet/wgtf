
#include "connection_slot.h"
#include "metadata/connection_slot.mpp"
#include "graph_node.h"

namespace wgt
{
void ConnectionSlot::Init(Params&& params)
{
    title = std::move(params.title);
    icon = std::move(params.icon);
    parent = params.parent;
}

const std::string& ConnectionSlot::GetTitle() const
{
    return title;
}

const std::string& ConnectionSlot::GetIcon() const
{
    return icon;
}

size_t ConnectionSlot::GetUID() const
{
    return reinterpret_cast<size_t>(this);
}

ObjectHandleT<GraphNode> ConnectionSlot::GetParentNode()
{
    assert(parent != nullptr);
    return parent;
}
} // end namespace wgt
