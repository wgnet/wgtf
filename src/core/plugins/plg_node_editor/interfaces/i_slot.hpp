#ifndef __I_SLOT_H__
#define __I_SLOT_H__

#include <string>
#include <set>

#include "core_reflection/reflected_object.hpp"
#include "core_data_model/i_list_model.hpp"

#include "i_node.hpp"

namespace wgt
{
class MetaType;

/*!
* \class ISlot
*
* \brief Interface of slot which is used by nodes and connections
*
* \author a_starunskiy
* \date May 2016
*/
class ISlot
{
    DECLARE_REFLECTED
public:
	ISlot(){}
    virtual ~ISlot(){}

    /*! Returns a unique slot id
    */
    virtual size_t Id() const = 0;

    /*! Returns whether the slot is input
    @return true if the slot is input, false otherwise
    */
    virtual bool IsInput() const = 0;

    /*! Returns the slot label
    @return the slot label
    */
    virtual std::string Label() const = 0;

    /*! Sets a label
    @param label label of slot
    */
    virtual void setLabel(const std::string &label) = 0;

    /*! Returns a path of icon
    @return icon path
    */
    virtual std::string Icon() const = 0;

    /*! Returns a color of the slot
    @return a current color of the slot
    */
    virtual std::string Color() const = 0;

    /*! Returns a user type of the slot
    @return a current metatype of the slot
    */
    virtual MetaType* Type() const = 0;

    /*! Returns whether the slot is editable
    @return true if the slot is editable, false otherwise
    */
    virtual bool Editable() const = 0;

    /*! Returns a node owner of the slot
    @return the node object which is owner of the slot
    */
    virtual INode* Node() const = 0;

    /*! Returns whether the slot is connected
    @return true if the slot is connected, false otherwise
    */
    virtual bool IsConnected() const = 0;

    /*! Returns all connectections ids which is connected to the current slot
    @return connections ids
    */
    virtual const std::set<size_t>* GetConnectionIds() const = 0;

    /*! Returns all slots which is connected to the current slot
    @return slots
    */
    virtual const GenericListT<ISlot*>* GetConnectedSlots() const = 0;

    /*! Returns whether the slot can connect
    @param slot The other slot
    @return true if the current slot can connect to the other slot, false otherwise
    */
    virtual bool CanConnect(ObjectHandleT<ISlot> slot) = 0;

    /*! Trying connect a current slot with the other slot
    @param connectionId The id of current connection
    @param slot The other slot of current connection
    @return true if the connection is successfully created, false otherwise
    */
    virtual bool Connect(size_t connectionId, ObjectHandleT<ISlot> slot) = 0;

    /*! Trying disconnect a current slot with the other slot
    @param connectionId The id of connection which should disconnect
    @param slot The other slot of connection which shloud disconnect
    @return true if connection is successfully disconnected, false otherwise
    */
    virtual bool Disconnect(size_t connectionId, ObjectHandleT<ISlot> slot) = 0;
};
} // end namespace wgt
#endif //__I_SLOT_H__
