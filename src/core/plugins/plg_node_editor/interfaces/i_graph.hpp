#ifndef __I_GRAPH_H__
#define __I_GRAPH_H__

#include <string>
#include <vector>
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"
#include "core_data_model/abstract_item_model.hpp"

#include "i_node.hpp"
#include "i_connection.hpp"
#include "i_group.hpp"

namespace wgt
{
class IGroup;

/*!
* \class IGraph
*
* \brief Interface contains nodes models and connections models for reflects in view.
* Also is allowing to create and delete nodes and connections.
*
* \author a_starunskiy
* \date May 2016
*/
class IGraph
{
	DECLARE_REFLECTED
public:
	virtual ~IGraph()
	{
	}

	/*! Creates a new node
	@param nodeClass user class of node
	@param x coordinate of position on X axis
	@param y coordinate of position on Y axis
	@return new node object if node is successfully created, null otherwise
	*/
	virtual ObjectHandleT<INode> CreateNode(std::string nodeClass, float x = 0.0f, float y = 0.0f) = 0;

	/*! Deletes the node with current id
	@param id The id of node which should be deleted
	*/
	virtual void DeleteNode(size_t nodeID) = 0;

	/*! Creates a connection between an one slot of node and the other slot of node
	@param nodeIdFrom The node id which contains a first slot in connection
	@oaram slotIdFrom The slot id from which connection starts
	@param nodeIdTo The node id which contains a second slot in connection
	@param slotIdTo The slot id where connection ends
	@return connection object if connection is successfully created, null otherwise
	*/
	virtual ObjectHandleT<IConnection> CreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo,
	                                                    size_t slotIdTo) = 0;

	/*! Delets connection with current connections id
	@param connectionId The connections id
	*/
	virtual void DeleteConnection(size_t connectionId) = 0;

	/*! Validates graph
	@param &errorMessage The reference where will be written the error message if the graph is not valid
	*/
	virtual bool Validate(std::string& errorMessage) = 0;

	/*! Saves graph
	@param fileName The file name where graph will be saved
	*/
	virtual void Save(std::string fileName) = 0;

	/*! Loades graph
	@param fileName The file name from where graph will be loaded
	*/
	virtual void Load(std::string fileName) = 0;

protected:
	/*! Gets nodes model
	*/
	virtual const AbstractListModel* GetNodesModel() const = 0;

	/*! Gets connections model
	*/
	virtual const AbstractListModel* GetConnectionsModel() const = 0;

	/*! Gets node classes model
	*/
	virtual const AbstractListModel* GetNodeClassesModel() const = 0;

	/*! Gets node group model
	*/
	virtual const Collection& GetNodeGroupModel() const = 0;
};
} // end namespace wgt
#endif // __I_GRAPH_H__
