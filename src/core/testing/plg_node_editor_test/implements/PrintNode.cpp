#include "PrintNode.h"

namespace wgt
{
const std::string PRINT_NODE_TITLE = "Print";
const std::string PRINT_NODE_SUBTITLE = "";
const std::string PRINT_NODE_ICON = "images/model_16x16.png";
const std::string PRINT_NODE_COLOR = "red";

PrintNode::PrintNode(const std::string& nodeClass)
    : m_x(0.0f), m_y(0.0f), m_class(nodeClass), m_title(PRINT_NODE_TITLE), m_subTitle(PRINT_NODE_SUBTITLE),
      m_icon(PRINT_NODE_ICON), m_color(PRINT_NODE_COLOR), m_enabled(true), m_minimized(false)
{
	m_id = reinterpret_cast<size_t>(this);

	m_inputSlotsModel.push_back(new EventSlot(this, true));
	m_inputSlotsModel.back()->setLabel("input");
	m_inputSlotsModel.push_back(new StringSlot(this, true));
	m_inputSlotsModel.back()->setLabel("value");

	m_outputSlotsModel.push_back(new EventSlot(this, false));
	m_outputSlotsModel.back()->setLabel("out");
}

PrintNode::~PrintNode()
{
}

void PrintNode::SetPos(float x, float y)
{
	m_x = x;
	m_y = y;
}

ObjectHandleT<ISlot> PrintNode::GetSlotById(size_t slotId) const
{
	auto inputSlotPos =
	std::find_if(m_inputSlotsModel.begin(), m_inputSlotsModel.end(),
	             [slotId](const ObjectHandleT<ISlot>& inputSlot) { return slotId == inputSlot->Id(); });

	if (inputSlotPos != m_inputSlotsModel.end())
	{
		return *inputSlotPos;
	}

	auto outputSlotPos =
	std::find_if(m_outputSlotsModel.begin(), m_outputSlotsModel.end(),
	             [slotId](const ObjectHandleT<ISlot>& outputSlot) { return slotId == outputSlot->Id(); });

	if (outputSlotPos != m_outputSlotsModel.end())
	{
		return *outputSlotPos;
	}

	NGT_ERROR_MSG("Slot with id: %d not found\n", slotId);
	return nullptr;
}

bool PrintNode::CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
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
		result = (std::find_if(connectedSlots->begin(), connectedSlots->end(), [&otherSlot](const ISlot* slot) {
			          return otherSlot->Id() == slot->Id();
			      }) == connectedSlots->end());
		break;
	}

	return result;
}

bool PrintNode::Enabled() const
{
	return m_enabled;
}

void PrintNode::SetEnabled(bool enabled)
{
	m_enabled = enabled;
}

bool PrintNode::Minimized() const
{
	return m_minimized;
}

void PrintNode::SetMinimized(bool minimized)
{
	m_minimized = minimized;
}

bool PrintNode::Validate(std::string& errorMessage)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return true;
}

void PrintNode::OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}

void PrintNode::OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}
} // end namespace wgt
