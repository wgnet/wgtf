#include "AddIntegerNode.h"

namespace wgt
{
const std::string ADD_INTEGER_NODE_TITLE = "Add Integer";
const std::string ADD_INTEGER_NODE_SUBTITLE = "";
const std::string ADD_INTEGER_NODE_ICON = "images/model_16x16.png";
const std::string ADD_INTEGER_NODE_COLOR = "green";

AddIntegerNode::AddIntegerNode(const std::string &nodeClass)
: m_x(0.0f)
, m_y(0.0f)
, m_class(nodeClass)
, m_title(ADD_INTEGER_NODE_TITLE)
, m_subTitle(ADD_INTEGER_NODE_SUBTITLE)
, m_icon(ADD_INTEGER_NODE_ICON)
, m_color(ADD_INTEGER_NODE_COLOR)
, m_enabled(true)
, m_minimized(false)
{
    m_id = reinterpret_cast<size_t>(this);

    for (int i = 0; i < 2; ++i)
    {
        m_inputSlotsModel.push_back(new IntegerSlot(this, true));
	std::string label("i");
	label += std::to_string(i);
        m_inputSlotsModel.back()->setLabel(label);
    }

    m_outputSlotsModel.push_back(new IntegerSlot(this, false));
    m_outputSlotsModel.back()->setLabel("res");
}

AddIntegerNode::~AddIntegerNode()
{}

void AddIntegerNode::SetPos(float x, float y)
{
    m_x = x;
    m_y = y;
}

ObjectHandleT<ISlot> AddIntegerNode::GetSlotById(size_t slotId) const
{
    auto inputSlotPos = std::find_if(m_inputSlotsModel.begin(), m_inputSlotsModel.end(), [slotId](const ObjectHandleT<ISlot> &inputSlot) {
        return slotId == inputSlot->Id();
    });

    if (inputSlotPos != m_inputSlotsModel.end())
    {
        return *inputSlotPos;
    }

    auto outputSlotPos = std::find_if(m_outputSlotsModel.begin(), m_outputSlotsModel.end(), [slotId](const ObjectHandleT<ISlot> &outputSlot) {
        return slotId == outputSlot->Id();
    });

    if (outputSlotPos != m_outputSlotsModel.end())
    {
        return *outputSlotPos;
    }

    NGT_ERROR_MSG("Slot with id: %d not found\n", slotId);
    return nullptr;
}

bool AddIntegerNode::CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    bool result = false;

    while (true)
    {
        ObjectHandleT<INode> otherNode = otherSlot->Node();
        if (this == otherNode.get())
            break;

        if (!(mySlot->IsInput() ^ otherSlot->IsInput()))
            break;

        if (mySlot->Type() != otherSlot->Type())
            break;

        if (mySlot->Color() != otherSlot->Color())
            break;

        auto connectedSlots = mySlot->GetConnectedSlots();
        result = (std::find_if(connectedSlots->begin(), connectedSlots->end(), [&otherSlot](const ISlot *slot){ return otherSlot->Id() == slot->Id(); }) == connectedSlots->end());
        break;
    }

    return result;
}

bool AddIntegerNode::Enabled() const
{
    return m_enabled;
}

void AddIntegerNode::SetEnabled(bool enabled)
{
    m_enabled = enabled;
}

bool AddIntegerNode::Minimized() const
{
    return m_minimized;
}

void AddIntegerNode::SetMinimized(bool minimized)
{
    m_minimized = minimized;
}

bool AddIntegerNode::Validate(std::string &errorMessage)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
    return true;
}

void AddIntegerNode::OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}

void AddIntegerNode::OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
    NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}
} // end namespace wgt
