#include "IntToStringNode.h"
#include "core_object\managed_object.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
const std::string INT_TO_STRING_NODE_TITLE = "Int to String";
const std::string INT_TO_STRING_NODE_SUBTITLE = "";
const std::string INT_TO_STRING_NODE_ICON = "images/model_16x16.png";
const std::string INT_TO_STRING_NODE_COLOR = "yellow";

IntToStringNode::IntToStringNode(const std::string& nodeClass)
    : m_x(0.0f), m_y(0.0f), m_class(nodeClass), m_title(INT_TO_STRING_NODE_TITLE),
      m_subTitle(INT_TO_STRING_NODE_SUBTITLE), m_icon(INT_TO_STRING_NODE_ICON), m_color(INT_TO_STRING_NODE_COLOR),
      m_enabled(true), m_minimized(false)
{
}

IntToStringNode::~IntToStringNode()
{
}

void IntToStringNode::Init()
{
	m_id = reinterpret_cast<size_t>(this);

	ownedSlots_.emplace_back(std::unique_ptr<ISlot>(new IntegerSlot(getThis(), true)));
	inputSlots_.push_back(ownedSlots_.back().getHandleT());
	inputSlots_.back()->setLabel("a");
	ownedSlots_.emplace_back(std::unique_ptr<ISlot>(new StringSlot(getThis(), false)));
	outputSlots_.push_back(ownedSlots_.back().getHandleT());
	outputSlots_.back()->setLabel("res");

	inputSlotsModel_.setSource(Collection(inputSlots_));
	outputSlotsModel_.setSource(Collection(outputSlots_));
}

void IntToStringNode::SetPos(float x, float y)
{
	m_x = x;
	m_y = y;
}

ObjectHandleT<ISlot> IntToStringNode::GetSlotById(size_t slotId) const
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

bool IntToStringNode::CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
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

bool IntToStringNode::Enabled() const
{
	return m_enabled;
}

void IntToStringNode::SetEnabled(bool enabled)
{
	m_enabled = enabled;
}

bool IntToStringNode::Minimized() const
{
	return m_minimized;
}

void IntToStringNode::SetMinimized(bool minimized)
{
	m_minimized = minimized;
}

bool IntToStringNode::Validate(std::string& errorMessage)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return true;
}

void IntToStringNode::OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}

void IntToStringNode::OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}
} // end namespace wgt
