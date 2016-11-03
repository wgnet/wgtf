#ifndef __I_NODE_EDITOR_H__
#define __I_NODE_EDITOR_H__

#include <string>
#include <memory>

#include "core_reflection/object_handle.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_reflection/reflected_object.hpp"

#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"

#include "i_graph.hpp"
#include "i_group.hpp"

namespace wgt
{
class Vector4;
/*!
* \class INodeEditor
*
* \brief Main interface of node editor used for to set and to connect graph model
* with graph view
*
* \author a_starunskiy
* \date May 2016
*/
class INodeEditor
{
	DECLARE_REFLECTED;
public:
	virtual ~INodeEditor(){}

    /*! Sets a new graph to node editor
    @param graph the exact graph which model will be used for display in view
    */
    virtual void SetGraph(std::shared_ptr<IGraph> graph) = 0;

    /*! Creates a new node in current graph
    @param nodeClass user class of node
    @param x coordinate of position on X axis
    @param y coordinate of position on Y axis
    @return new node object if node is succesfully created, null otherwise
    */
	virtual std::shared_ptr<INode> CreateNode(std::string nodeClass, float x, float y) = 0;

    /*! Deletes the node with current id in graph
    @param id The node id of node which should be deleted
    */
    virtual bool DeleteNode(size_t id) = 0;

    /*! Returns node with current id
    @param id The node id which should be returned
    @return node object if node with this id exists, null otherwise
    */
	virtual INode* GetNode(size_t id) = 0;
	
    /*! Connects slot of node with other slot of node
    @param nodeIdFrom The node id which containes a first slot in connection
    @oaram slotIdFrom The slot id from which connection starts
    @param nodeIdTo The node id which containes a second slot in connection
    @param slotIdTo The slot id where connection ends
    @return true if connection is created, false otherwise
    */
    virtual bool Connect(size_t nodeIdFrom, size_t slotIdFrom,
        size_t nodeIdTo, size_t slotIdTo) = 0;

    /*! Disconnects slot of node with other slot of node
    @param nodeIdFrom The node id which containes a first slot in connection
    @oaram slotIdFrom The slot id from which connection starts
    @param nodeIdTo The node id which containes a second slot in connection
    @param slotIdTo The slot id where connection ends
    @return true if connection is disconnected, false otherwise
    */
    virtual bool Disconnect(size_t nodeIdFrom, size_t slotIdFrom,
        size_t nodeIdTo, size_t slotIdTo) = 0;
    
protected:

    /*! Is called by qml when node is needed to be created 
    */
    virtual void onCreateNode(int x, int y, std::string nodeClass) = 0;

    /*! Is called by qml when node is needed to be deleted
    */
    virtual void onDeleteNode(size_t nodeID) = 0;
    
    /*! Is called by qml when connection is needed to be created
    */
    virtual void onCreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo) = 0;

    /*! Is called by qml when connection is needed to be deleted
    */
    virtual void onDeleteConnection(size_t connectionId) = 0;

    /*! Gets current graph model
    */
    virtual const IListModel* GetGraphModel() const = 0;

	virtual void CreateGroup(Collection& collection,
	                         const Vector4& rectangle,
	                         const std::string& name,
	                         const Vector4& color) = 0;
};
} // end namespace wgt
#endif // __I_NODE_EDITOR_H__
