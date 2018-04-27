#include "node_editor.hpp"
#include "group.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
NodeEditor::NodeEditor()
{
}

void NodeEditor::SetGraph(std::shared_ptr<IGraph> graph)
{
	if (graph == nullptr)
	{
		NGT_ERROR_MSG("Graph is Null");
		return;
	}

	graph_ = graph;
}

ObjectHandleT<INode> NodeEditor::CreateNode(std::string nodeClass, float x, float y)
{
	return graph_->CreateNode(nodeClass, x, y);
}

void NodeEditor::onCreateNode(int x, int y, std::string nodeClass)
{
	// TODO: Unify the x, y type between CreateNode() and onCreateNode()
	auto node = graph_->CreateNode(nodeClass, static_cast<float>(x), static_cast<float>(y));
}

void NodeEditor::onDeleteNode(size_t nodeID)
{
	graph_->DeleteNode(nodeID);
}

void NodeEditor::onCreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo)
{
	graph_->CreateConnection(nodeIdFrom, slotIdFrom, nodeIdTo, slotIdTo);
}

void NodeEditor::onDeleteConnection(size_t connectionId)
{
	graph_->DeleteConnection(connectionId);
}

void NodeEditor::CreateGroup(Collection& collection, const Vector4& rectangle, const std::string& name,
                             const Vector4& color)
{
	auto pDefinitionManager = this->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return;
	}
	auto pDefinition = pDefinitionManager->getDefinition<Group>();
	if (pDefinition == nullptr)
	{
		return;
	}

	auto group = reflectedCast<IGroup>(pDefinition->createShared(), *pDefinitionManager);
	group->setColor(color);
	group->setName(name);
	group->setRectangle(rectangle);
	collection.insertValue(collection.size(), group);
}

ObjectHandleT<INode> NodeEditor::GetNode(size_t id)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return nullptr;
}

bool NodeEditor::DeleteNode(size_t id)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return false;
}

bool NodeEditor::Connect(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return false;
}

bool NodeEditor::Disconnect(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo)
{
	NGT_ERROR_MSG("METHOD IS NOT IMPLEMENTED\n");
	return false;
}

} // end namespace wgt
