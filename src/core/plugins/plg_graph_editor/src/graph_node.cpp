
#include "graph_node.h"
#include "connection_slot.h"
#include "BaseModel.h"

#include "metadata/graph_node.mpp"

#include <core_dependency_system/i_interface.hpp>
#include <core_reflection/i_definition_manager.hpp>
#include <core_reflection/property_accessor.hpp>

namespace wgt
{
class ConnectionSlotsModel : public BaseModel<ConnectionSlot>
{
    typedef BaseModel<ConnectionSlot> TBase;

public:
    ConnectionSlotsModel(std::vector<ObjectHandleT<ConnectionSlot>>&& objects)
        : TBase(std::move(objects))
    {
    }
};

template <typename T>
void setProperty(GraphNode* node, const char* propertyName, const T& value)
{
    IDefinitionManager* defMng = Context::queryInterface<IDefinitionManager>();
    assert(defMng != nullptr);

    IClassDefinition* definition = defMng->getDefinition<GraphNode>();
    assert(definition != nullptr);

    definition->bindProperty(propertyName, node).setValue(value);
}

std::string const& GraphNode::GetTitle() const
{
    return title;
}

void GraphNode::SetTitle(std::string const& title_)
{
    title = title_;
}

size_t GraphNode::GetUID() const
{
    return reinterpret_cast<size_t>(this);
}

GraphNode::GraphNode()
	: modelX( 0 )
	, modelY( 0 )
{
}

GraphNode::~GraphNode()
{
}

void GraphNode::Init(Params&& params)
{
    inputSlots.reset(new ConnectionSlotsModel(std::move(params.inputSlots)));
    outputSlots.reset(new ConnectionSlotsModel(std::move(params.outputSlots)));
    typeId = std::move(params.typeId);
}

float GraphNode::GetPosX() const
{
    return modelX;
}

void GraphNode::SetPosX(float x)
{
    modelX = x;
    setProperty(this, "nodePosX", modelY);
}

float GraphNode::GetPosY() const
{
    return modelY;
}

void GraphNode::SetPosY(float y)
{
    modelY = y;
    setProperty(this, "nodePosY", modelY);
}

void GraphNode::Shift(float modelShiftX, float modelShiftY)
{
    //MoveNodes(modelShiftX, modelShiftY);
    ShiftImpl(modelShiftX, modelShiftY);
}

void GraphNode::ShiftImpl(float modelShiftX, float modelShiftY)
{
    SetPosX(GetPosX() + modelShiftX);
    SetPosY(GetPosY() + modelShiftY);

    Changed(this);
}

IListModel* GraphNode::GetInputSlots() const
{
    return inputSlots.get();
}

IListModel* GraphNode::GetOutputSlots() const
{
    return outputSlots.get();
}

void GraphNode::PosXChanged(const float& x)
{

}

void GraphNode::PosYChanged(const float& y)
{

}
} // end namespace wgt
