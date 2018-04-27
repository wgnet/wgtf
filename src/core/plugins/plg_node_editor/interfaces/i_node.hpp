#ifndef __I_NODE_H__
#define __I_NODE_H__

#include <string>

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/collection_model.hpp"

namespace wgt
{
class IGraph;
class ISlot;

/*!
* \class INode
*
* \brief Interface of node model
*
* \author a_starunskiy
* \date May 2016
*/
class INode
{
	DECLARE_REFLECTED
public:
	INode()
	{
	}
	virtual ~INode()
	{
	}

	virtual void Init() = 0;

	/*! Returns an unique node id
	@return unique id of node
	*/
	virtual size_t Id() const = 0;

	/*! Returns a user class of node
	@return The user class of node
	*/
	virtual std::string Class() const = 0;

	/*! Returns a title of node
	@return The title of node
	*/
	virtual std::string Title() const = 0;

	/*! Returns a subtitle of node
	@return The subtitle of node
	*/
	virtual std::string SubTitle() const = 0;

	/*! Returns a user category of node
	@return The user category of node
	*/
	virtual std::string Category() const = 0;

	/*! Returns an icon path of node
	@return icon path
	*/
	virtual std::string Icon() const = 0;

	/*! Returns a color of node
	@return color
	*/
	virtual std::string Color() const = 0;

	/*! Returns x coordinate position of node in global coordinates
	@return x coordinate
	*/
	virtual float X() const = 0;

	/*! Returns y coordinate position of node in global coordinates
	@return y coordinate
	*/
	virtual float Y() const = 0;

	/*! Sets a new position of node
	@param x A new x coordinate
	@param y A new y coordinate
	*/
	virtual void SetPos(float x, float y) = 0;

	/*! Gets child slot by slot id
	@param slotId The slot id which should gets
	@return slot object if node containes slot with current id, null otherwise
	*/
	virtual ObjectHandleT<ISlot> GetSlotById(size_t slotId) const = 0;

	/*! Returns whether the node is enabled
	@return true if the node is enabled, false otherwise
	*/
	virtual bool Enabled() const = 0;

	/*! Sets node enabled
	@param enabled Sets node as enabled if true, otherwise as not enabled
	*/
	virtual void SetEnabled(bool enabled) = 0;

	/*! Returns whether the node is minimized
	@return true if the node is minimized, false otherwise
	*/
	virtual bool Minimized() const = 0;

	/*! Sets node minimized
	@param minimized Sets node as minimized if true, otherwise as not minimized
	*/
	virtual void SetMinimized(bool minimized) = 0;

	/*! Returns whether the node can connect with other slot
	@param mySlot The slot child of current node
	@param otherSlot The slot child of other node
	@return true if connection is possible, false otherwise
	*/
	virtual bool CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) = 0;

	/*! Validates node
	@param &errorMessage The reference where will be written the error message if the node is not valid
	*/
	virtual bool Validate(std::string& errorMessage) = 0;

	/*! Callback which should be called by slot when connection is created
	@param mySlot The slot child of current node which is connected
	@param otherSlot The slot child of other node which is connected
	*/
	virtual void OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) = 0;

	/*! Callback which should be called by slot when connection is disconnected
	@param mySlot The slot child of current node which is connected
	@param otherSlot The slot child of other node which is connected
	*/
	virtual void OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) = 0;

	/*! Gets all input slots of node
	@return input slots
	*/
	virtual const CollectionModel* GetInputSlots() const = 0;

	/*! Gets all output slots of node
	@return output slots
	*/
	virtual const CollectionModel* GetOutputSlots() const = 0;

protected:
	/*! Gets input slots model
	*/
	virtual const AbstractListModel* GetInputSlotsModel() const = 0;

	/*! Gets output slots model
	*/
	virtual const AbstractListModel* GetOutputSlotsModel() const = 0;
};
} // end namespace wgt
#endif // __I_NODE_H__
