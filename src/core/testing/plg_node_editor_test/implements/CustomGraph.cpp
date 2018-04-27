#include "CustomGraph.h"
#include "CustomConnection.h"

#include "IntToStringNode.h"
#include "AddIntegerNode.h"
#include "PrintNode.h"

#include "core_logging/logging.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
CustomGraph::CustomGraph() : groupModel_(groups_)
{
	nodeClasses_.push_back("IntToString");
	m_nodeClasses.insert(NodeClassesMap::value_type("IntToString", [&]() -> ObjectHandleT<INode> { 
		ownedNodes_.emplace_back(std::unique_ptr<INode>( new IntToStringNode("IntToString")));
		ownedNodes_.back()->Init();
		return ownedNodes_.back().getHandleT();
	}));

	nodeClasses_.push_back("AddInteger");
	m_nodeClasses.insert(NodeClassesMap::value_type("AddInteger", [&]() -> ObjectHandleT<INode> {
		ownedNodes_.emplace_back(std::unique_ptr<INode>(new AddIntegerNode("AddInteger")));
		ownedNodes_.back()->Init();
		return ownedNodes_.back().getHandleT();
	}));

	nodeClasses_.push_back("Print");
	m_nodeClasses.insert(NodeClassesMap::value_type("Print", [&]() -> ObjectHandleT<INode> {
		ownedNodes_.emplace_back(std::unique_ptr<INode>(new PrintNode("Print")));
		ownedNodes_.back()->Init();
		return ownedNodes_.back().getHandleT();
	}));

	nodeClassesModel_.setSource(Collection(nodeClasses_));
	nodesModel_.setSource(Collection(nodes_));
	connectionsModel_.setSource(Collection(connections_));
}

ObjectHandleT<INode> CustomGraph::CreateNode(std::string nodeClass, float x, float y)
{
	auto nodeCreator = m_nodeClasses.at(nodeClass);
	auto node(nodeCreator());
	node->SetPos(x, y);

	Collection& nodes = nodesModel_.getSource();
	nodes.insertValue(nodes.size(), node);

	return node;
}

void CustomGraph::DeleteNode(size_t nodeId)
{
	Collection& nodes = nodesModel_.getSource();
	auto callback = [nodeId](const Variant& nodeVariant) {
		auto node = nodeVariant.value<ObjectHandleT<INode>>();
		return nodeId == node->Id();
	};

	auto nodeIter = std::find_if(nodes.begin(), nodes.end(), callback);

	if (nodeIter == nodes.end())
	{
		NGT_ERROR_MSG("Failed to get node with id: %d\n", nodeId);
		return;
	}

	Variant nodeVariant = *nodeIter;
	auto node = nodeVariant.value<ObjectHandleT<INode>>();
	auto inputSlots = node->GetInputSlots()->getSource();
	auto outputSlots = node->GetOutputSlots()->getSource();

	for (const auto& slotVariant : inputSlots)
	{
		auto slot = slotVariant.value<ObjectHandleT<ISlot>>();

		if (!slot->IsConnected())
			continue;

		auto connections = *slot->GetConnectionIds();
		for (auto& connectionId : connections)
		{
			DeleteConnection(connectionId);
		}
	}

	for (const auto& slotVariant : outputSlots)
	{
		auto slot = slotVariant.value<ObjectHandleT<ISlot>>();

		if (!slot->IsConnected())
			continue;

		auto connections = *slot->GetConnectionIds();
		for (auto& connectionId : connections)
		{
			DeleteConnection(connectionId);
		}
	}

	auto index = std::distance(nodes.begin(), nodeIter);
	auto ownedNodesIter = ownedNodes_.begin();
	std::advance(ownedNodesIter, index);

	nodes.erase(nodeIter);
	ownedNodes_.erase(ownedNodesIter);
}

ObjectHandleT<IConnection> CustomGraph::CreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo,
                                                         size_t slotIdTo)
{
	ObjectHandleT<INode> nodeFrom;
	ObjectHandleT<ISlot> slotFrom;
	ObjectHandleT<INode> nodeTo;
	ObjectHandleT<ISlot> slotTo;

	bool result = false;
	while (true)
	{
		auto callbackFrom = [nodeIdFrom](const Variant& nodeVariant) {
			auto node = nodeVariant.value<ObjectHandleT<INode>>();
			return nodeIdFrom == node->Id();
		};

		auto nodeIterFrom = std::find_if(nodes_.begin(), nodes_.end(), callbackFrom);
		if (nodeIterFrom == nodes_.end())
		{
			NGT_ERROR_MSG("Failed to get node with id: %d\n", nodeIdFrom);
			break;
		}

		nodeFrom = *nodeIterFrom;
		slotFrom = nodeFrom->GetSlotById(slotIdFrom);
		if (slotFrom == nullptr)
		{
			NGT_ERROR_MSG("Failed to get slot with id: %d\n", slotIdFrom);
			break;
		}

		auto callbackTo = [nodeIdTo](const Variant& nodeVariant) {
			auto node = nodeVariant.value<ObjectHandleT<INode>>();
			return nodeIdTo == node->Id();
		};

		auto nodeIterTo = std::find_if(nodes_.begin(), nodes_.end(), callbackTo);
		if (nodeIterTo == nodes_.end())
		{
			NGT_ERROR_MSG("Failed to get node with id: %d\n", nodeIdTo);
			break;
		}

		nodeTo = *nodeIterTo;
		slotTo = nodeTo->GetSlotById(slotIdTo);
		if (slotTo == nullptr)
		{
			NGT_ERROR_MSG("Failed to get slot with id: %d\n", slotIdTo);
			break;
		}

		result = true;
		break;
	}

	if (!result)
	{
		return nullptr;
	}

	ownedConnections_.emplace_back(std::unique_ptr<IConnection>( new CustomConnection()));
	auto connection = ownedConnections_.back().getHandleT();
	result = connection->Bind(slotFrom, slotTo);

	if (result)
	{
		Collection& connections = connectionsModel_.getSource();
		connections.insertValue(connections.size(), connection);
		return connection;
	}

	return nullptr;
}

void CustomGraph::DeleteConnection(size_t connectionId)
{
	Collection& connections = connectionsModel_.getSource();
	auto callback = [connectionId](const Variant& connectionVariant) {
		auto connection = connectionVariant.value<ObjectHandleT<IConnection>>();
		return connectionId == connection->Id();
	};

	auto connectionPos = std::find_if(connections.begin(), connections.end(), callback);

	if (connectionPos == connections.end())
	{
		NGT_ERROR_MSG("Failed to get connection with ID: %d\n", connectionId);
		return;
	}

	Variant connectionVariant = *connectionPos;
	auto connection = connectionVariant.value<ObjectHandleT<IConnection>>();
	if (!connection->UnBind())
	{
		NGT_ERROR_MSG("Failed to unbind slots\n");
	}

	auto index = std::distance(connections.begin(), connectionPos);
	auto ownedConnnectionsIter = ownedConnections_.begin();
	std::advance(ownedConnnectionsIter, index);

	connections.erase(connectionPos);
	ownedConnections_.erase(ownedConnnectionsIter);
}

bool CustomGraph::Validate(std::string& errorMessage)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return true;
}

void CustomGraph::Save(std::string fileName)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}

void CustomGraph::Load(std::string fileName)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
}

const Collection& CustomGraph::GetNodeGroupModel() const /* override */
{
	return groupModel_;
}

} // end namespace wgt
