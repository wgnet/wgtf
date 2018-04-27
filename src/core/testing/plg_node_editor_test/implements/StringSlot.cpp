#include "StringSlot.h"
#include "core_logging/logging.hpp"

namespace wgt
{
const std::string STRING_SLOT_LABEL = "string";
const std::string STRING_SLOT_ICON = "images/blueSlot.png";
const std::string STRING_SLOT_COLOR = "darkCyan";

StringSlot::StringSlot(ObjectHandleT<INode> node, bool isInput)
    : m_label(STRING_SLOT_LABEL), m_icon(STRING_SLOT_ICON), m_color(STRING_SLOT_COLOR), m_editable(true),
      m_isInput(isInput), m_pNode(node)
{
	m_id = reinterpret_cast<size_t>(this);
	connectedSlotsModel.setSource(Collection(m_connectedSlots));
}

std::string StringSlot::Label() const
{
	return m_label;
}

void StringSlot::setLabel(const std::string& label)
{
	m_label = label;
}

std::string StringSlot::Icon() const
{
	return m_icon;
}

std::string StringSlot::Color() const
{
	return m_color;
}

MetaType* StringSlot::Type() const
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return nullptr;
}

bool StringSlot::Editable() const
{
	return m_editable;
}

ObjectHandleT<INode> StringSlot::Node() const
{
	return m_pNode;
}

const CollectionModel* StringSlot::GetConnectedSlots() const
{
	return &connectedSlotsModel;
}

bool StringSlot::CanConnect(ObjectHandleT<ISlot> slot)
{
	assert(m_pNode != nullptr);
	return m_pNode->CanConnect(getThis(), slot);
}

bool StringSlot::Connect(size_t connectionID, ObjectHandleT<ISlot> slot)
{
	assert(m_pNode != nullptr);
	bool result = m_pNode->CanConnect(getThis(), slot);

	if (result)
	{
		Collection& collection = connectedSlotsModel.getSource();
		collection.insertValue(collection.size(), slot);
		m_connectionIds.insert(connectionID);
		m_pNode->OnConnect(getThis(), m_connectedSlots.back());
	}

	return result;
}

bool StringSlot::Disconnect(size_t connectionID, ObjectHandleT<ISlot> slot)
{
	assert(m_pNode != nullptr);
	bool result = false;
	auto slotPos =
	std::find_if(m_connectedSlots.begin(), m_connectedSlots.end(),
	             [&slot](const ObjectHandleT<ISlot>& connectedSlot) { return slot->Id() == connectedSlot->Id(); });

	if (slotPos != m_connectedSlots.end())
	{
		result = true;
		m_connectedSlots.erase(slotPos);
		m_connectionIds.erase(connectionID);
		m_pNode->OnDisconnect(getThis(), slot);
	}

	return result;
}
} // end namespace wgt
