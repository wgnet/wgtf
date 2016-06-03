
#include "connector.h"
#include "Metadata/Connector.mpp"

#include "graph_node.h"
#include "connection_slot.h"
#include "connection_manager.h"

#include <core_reflection/i_definition_manager.hpp>
#include <core_dependency_system/i_interface.hpp>

#include <assert.h>

namespace wgt
{
Connector::Connector()
{
}

Connector::~Connector()
{
}

void Connector::Init(size_t outputSlotID_, size_t intputSlotID_)
{
    outputSlotID = outputSlotID_;
    intputSlotID = intputSlotID_;
}

ObjectHandleT<ConnectionSlot> Connector::GetOutputSlot() const
{
    assert(ConnectionManager::Instance().GetSlot(outputSlotID) != nullptr);
    return ConnectionManager::Instance().GetSlot(outputSlotID);
}

ObjectHandleT<ConnectionSlot> Connector::GetInputSlot() const
{
    assert(ConnectionManager::Instance().GetSlot(intputSlotID) != nullptr);
    return ConnectionManager::Instance().GetSlot(intputSlotID);
}

size_t Connector::GetInputSlotId() const
{
    return intputSlotID;
}

size_t Connector::GetOutputSlotId() const
{
    return outputSlotID;
}

size_t Connector::GetUID() const
{
    size_t uid = reinterpret_cast<size_t>(this);
    return uid;
}
} // end namespace wgt
