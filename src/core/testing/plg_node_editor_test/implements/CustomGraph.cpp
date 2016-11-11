#include "CustomGraph.h"
#include "CustomConnection.h"

#include "IntToStringNode.h"
#include "AddIntegerNode.h"
#include "PrintNode.h"

#include "core_logging/logging.hpp"

namespace wgt
{
CustomGraph::CustomGraph() : m_nodeGroupModel(m_groupsStorage)
{
	m_nodeClassesModel.push_back("IntToString");
	m_nodeClasses.insert(
	NodeClassesMap::value_type("IntToString", []() -> INode* { return new IntToStringNode("IntToString"); }));

	m_nodeClassesModel.push_back("AddInteger");
	m_nodeClasses.insert(
	NodeClassesMap::value_type("AddInteger", []() -> INode* { return new AddIntegerNode("AddInteger"); }));

	m_nodeClassesModel.push_back("Print");
	m_nodeClasses.insert(NodeClassesMap::value_type("Print", []() -> INode* { return new PrintNode("Print"); }));
}

std::shared_ptr<INode> CustomGraph::CreateNode(std::string nodeClass, float x, float y)
{
	auto nodeCreator = m_nodeClasses.at(nodeClass);
	std::shared_ptr<INode> node(nodeCreator());

	node->SetPos(x, y);
	m_nodesModel.push_back(node);

	return node;
}

void CustomGraph::DeleteNode(size_t nodeId)
{
	auto nodeIter = std::find_if(m_nodesModel.begin(), m_nodesModel.end(),
	                             [nodeId](const ObjectHandleT<INode>& node) { return nodeId == node->Id(); });

	if (nodeIter == m_nodesModel.end())
	{
		NGT_ERROR_MSG("Failed to get node with id: %d\n", nodeId);
		return;
	}

	ObjectHandleT<INode> node = *nodeIter;
	auto inputSlots = node->GetInputSlots();
	auto outputSlots = node->GetOutputSlots();

	for (const auto& slot : *inputSlots)
	{
		if (!slot->IsConnected())
			continue;

		auto connections = *slot->GetConnectionIds();
		for (auto& connectionId : connections)
		{
			DeleteConnection(connectionId);
		}
	}

	for (const auto& slot : *outputSlots)
	{
		if (!slot->IsConnected())
			continue;

		auto connections = *slot->GetConnectionIds();
		for (auto& connectionId : connections)
		{
			DeleteConnection(connectionId);
		}
	}
	m_nodesModel.erase(nodeIter);
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
		auto nodeIterFrom =
		std::find_if(m_nodesModel.begin(), m_nodesModel.end(),
		             [nodeIdFrom](const ObjectHandleT<INode>& node) { return nodeIdFrom == node->Id(); });
		if (nodeIterFrom == m_nodesModel.end())
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

		auto nodeIterTo = std::find_if(m_nodesModel.begin(), m_nodesModel.end(),
		                               [nodeIdTo](const ObjectHandleT<INode>& node) { return nodeIdTo == node->Id(); });
		if (nodeIterTo == m_nodesModel.end())
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

	ObjectHandleT<IConnection> connection(new CustomConnection());
	result = connection->Bind(slotFrom, slotTo);

	if (result)
	{
		m_connectionsModel.push_back(connection);
		return connection;
	}

	return nullptr;
}

void CustomGraph::DeleteConnection(size_t connectionId)
{
	auto connectionPos =
	std::find_if(m_connectionsModel.begin(), m_connectionsModel.end(),
	             [connectionId](ObjectHandleT<IConnection> connection) { return connectionId == connection->Id(); });

	if (connectionPos == m_connectionsModel.end())
	{
		NGT_ERROR_MSG("Failed to get connection with ID: %d\n", connectionId);
		return;
	}

	ObjectHandleT<IConnection> connection = *connectionPos;
	if (!connection->UnBind())
	{
		NGT_ERROR_MSG("Failed to unbind slots\n");
	}
	m_connectionsModel.erase(connectionPos);
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
	return m_nodeGroupModel;
}

} // end namespace wgt
