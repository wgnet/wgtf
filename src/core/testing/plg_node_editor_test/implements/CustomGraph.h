#ifndef __CUSTOM_GRAPH_H__
#define __CUSTOM_GRAPH_H__

#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_object/managed_object.hpp"

#include "plugins/plg_node_editor/interfaces/i_graph.hpp"
#include "plugins/plg_node_editor/interfaces/i_node.hpp"
#include "plugins/plg_node_editor/interfaces/i_connection.hpp"

#include <vector>

namespace wgt
{
typedef std::map<std::string, std::function<ObjectHandleT<INode>()> > NodeClassesMap;

class CustomGraph : public Implements<IGraph>, Depends<IDefinitionManager>
{
    DECLARE_REFLECTED
public:
    CustomGraph();
    virtual ~CustomGraph()
    {}

	ObjectHandleT<INode> CreateNode(std::string nodeClass, float x = 0.0f, float y = 0.0f) override;
    void DeleteNode(size_t nodeId) override;
   
	ObjectHandleT<IConnection> CreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo) override;
    void DeleteConnection(size_t connectionId) override;
    
    bool Validate(std::string& errorMessage) override;
    void Save(std::string fileName) override;
    void Load(std::string fileName) override;

private:
    const AbstractListModel* GetNodesModel() const override { return &nodesModel_; }
    const AbstractListModel* GetConnectionsModel() const override { return &connectionsModel_; }
    const AbstractListModel* GetNodeClassesModel() const override { return  &nodeClassesModel_; }
	virtual const Collection& GetNodeGroupModel() const override;

private:
	std::vector<ManagedObject<INode>> ownedNodes_;
	std::vector<ManagedObject<IConnection>> ownedConnections_;

	std::vector<std::string> nodeClasses_;
	std::vector<ObjectHandleT<INode>> nodes_;
	std::vector<ObjectHandleT<IConnection>> connections_;
	std::vector<ObjectHandleT<IGroup>> groups_;

	CollectionModel nodeClassesModel_;
	CollectionModel nodesModel_;
	CollectionModel connectionsModel_;
	Collection groupModel_;

    NodeClassesMap m_nodeClasses;
};
} // end namespace wgt
#endif //__CUSTOM_GRAPH_H__
