
#ifndef __GRAPHEDITOR_CONNECTION_H__
#define __GRAPHEDITOR_CONNECTION_H__

#include <core_reflection/object_handle.hpp>
#include <core_reflection/reflected_object.hpp>

#include <array>

namespace wgt
{
class ConnectionSlot;

class Connector
{
    DECLARE_REFLECTED
public:
    Connector();
    ~Connector();

    void Init(size_t outputSlotID, size_t intputSlotID);

    ObjectHandleT<ConnectionSlot> GetOutputSlot() const;
    ObjectHandleT<ConnectionSlot> GetInputSlot() const;

    size_t GetInputSlotId() const;
    size_t GetOutputSlotId() const;

    size_t GetUID() const;

private:
    size_t outputSlotID;
    size_t intputSlotID;
};
} // end namespace wgt
#endif // __GRAPHEDITOR_CONNECTION_H__
