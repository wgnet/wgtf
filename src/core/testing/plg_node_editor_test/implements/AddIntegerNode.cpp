#include "AddIntegerNode.h"
#include "core_object\managed_object.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
const std::string ADD_INTEGER_NODE_TITLE = "Add Integer";
const std::string ADD_INTEGER_NODE_SUBTITLE = "";
const std::string ADD_INTEGER_NODE_ICON = "images/model_16x16.png";
const std::string ADD_INTEGER_NODE_COLOR = "green";

AddIntegerNode::AddIntegerNode(const std::string& nodeClass)
	: m_x(0.0f), m_y(0.0f), m_class(nodeClass), m_title(ADD_INTEGER_NODE_TITLE), m_subTitle(ADD_INTEGER_NODE_SUBTITLE),
	m_icon(ADD_INTEGER_NODE_ICON), m_color(ADD_INTEGER_NODE_COLOR), m_enabled(true), m_minimized(false)
{
}

AddIntegerNode::~AddIntegerNode()
{
}

void AddIntegerNode::Init()
{
	m_id = reinterpret_cast<size_t>(this);

	inputSlotsModel_.setSource(Collection(inputSlots_));
	Collection& inputSlotsCollection = inputSlotsModel_.getSource();

	outputSlotsModel_.setSource(Collection(outputSlots_));
	Collection& outputSlotsCollection = outputSlotsModel_.getSource();

	for (int i = 0; i < 2; ++i)
	{
		std::string label("i");
		label += std::to_string(i);

		ownedSlots_.emplace_back(std::unique_ptr<ISlot>( new IntegerSlot(getThis(), true)));
		auto integerSlot = ownedSlots_.back().getHandleT();
		integerSlot->setLabel(label);

		inputSlotsCollection.insertValue(inputSlotsCollection.size(), integerSlot);
	}

	ownedSlots_.emplace_back(std::unique_ptr<ISlot>(new IntegerSlot(getThis(), false)));
	auto integerSlot = ownedSlots_.back().getHandleT();
	integerSlot->setLabel("res");

	outputSlotsCollection.insertValue(outputSlotsCollection.size(), integerSlot);
}

void AddIntegerNode::SetPos(float x, float y)
{
	m_x = x;
	m_y = y;
}

ObjectHandleT<ISlot> AddIntegerNode::GetSlotById(size_t slotId) const
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

bool AddIntegerNode::CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot)
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

bool AddIntegerNode::Validate(std::string& errorMessage)
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
