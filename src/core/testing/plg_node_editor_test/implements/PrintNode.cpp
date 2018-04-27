#include "PrintNode.h"
#include "core_object\managed_object.hpp"
#include "core_logging/logging.hpp"

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
}

PrintNode::~PrintNode()
{
}

void PrintNode::Init()
{
	m_id = reinterpret_cast<size_t>(this);

	ownedSlots_.emplace_back(std::unique_ptr<ISlot>(new EventSlot(getThis(), true)));
	inputSlots_.push_back(ownedSlots_.back().getHandleT());
	inputSlots_.back()->setLabel("input");
	ownedSlots_.emplace_back(std::unique_ptr<ISlot>(new StringSlot(getThis(), true)));
	inputSlots_.push_back(ownedSlots_.back().getHandleT());
	inputSlots_.back()->setLabel("value");

	ownedSlots_.emplace_back(std::unique_ptr<ISlot>(new EventSlot(getThis(), false)));
	outputSlots_.push_back(ownedSlots_.back().getHandleT());
	outputSlots_.back()->setLabel("out");

	inputSlotsModel_.setSource(Collection(inputSlots_));
	outputSlotsModel_.setSource(Collection(outputSlots_));
}

void PrintNode::SetPos(float x, float y)
{
	m_x = x;
	m_y = y;
}

ObjectHandleT<ISlot> PrintNode::GetSlotById(size_t slotId) const
{
	auto inputSlotPos =
	std::find_if(inputSlots_.begin(), inputSlots_.end(),
	             [slotId](const ObjectHandleT<ISlot>& inputSlot) { return slotId == inputSlot->Id(); });

	if (inputSlotPos != inputSlots_.end())
	{
		return *inputSlotPos;
	}

	auto outputSlotPos =
	std::find_if(outputSlots_.begin(), outputSlots_.end(),
	             [slotId](const ObjectHandleT<ISlot>& outputSlot) { return slotId == outputSlot->Id(); });

	if (outputSlotPos != outputSlots_.end())
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
		auto otherNode = otherSlot->Node();
		if (this == otherNode.get())
			break;

		if (!(mySlot->IsInput() ^ otherSlot->IsInput()))
			break;

		if (mySlot->Type() != otherSlot->Type())
			break;

		if (mySlot->Color() != otherSlot->Color())
			break;

		auto connectedSlots = mySlot->GetConnectedSlots()->getSource();
		auto callback = [&otherSlot](const Variant& slotVariant) {
			auto slot = slotVariant.value<ObjectHandleT<ISlot>>();
			return otherSlot->Id() == slot->Id();
		};

		result = (std::find_if(connectedSlots.begin(), connectedSlots.end(), callback) == connectedSlots.end());
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
