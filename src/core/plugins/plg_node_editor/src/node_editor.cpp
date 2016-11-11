#include "node_editor.hpp"
#include "group.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/type_class_definition.hpp"

namespace wgt
{
NodeEditor::NodeEditor(IComponentContext& context) : Implements<INodeEditor>(), Depends<IDefinitionManager>(context)
{
}

void NodeEditor::SetGraph(std::shared_ptr<IGraph> graph)
{
	if (graph == nullptr)
	{
		NGT_ERROR_MSG("Graph is Null");
		return;
	}

	graphModel.clear();
	graphModel.push_back(graph);
}

std::shared_ptr<INode> NodeEditor::CreateNode(std::string nodeClass, float x, float y)
{
	assert(!graphModel.empty());
	return graphModel.back()->CreateNode(nodeClass, x, y);
}

void NodeEditor::onCreateNode(int x, int y, std::string nodeClass)
{
	// TODO: Unify the x, y type between CreateNode() and onCreateNode()
	std::shared_ptr<INode> node =
	graphModel.back()->CreateNode(nodeClass, static_cast<float>(x), static_cast<float>(y));
}

void NodeEditor::onDeleteNode(size_t nodeID)
{
	graphModel.back()->DeleteNode(nodeID);
}

void NodeEditor::onCreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo)
{
	graphModel.back()->CreateConnection(nodeIdFrom, slotIdFrom, nodeIdTo, slotIdTo);
}

void NodeEditor::onDeleteConnection(size_t connectionId)
{
	graphModel.back()->DeleteConnection(connectionId);
}

void NodeEditor::CreateGroup(Collection& collection, const Vector4& rectangle, const std::string& name,
                             const Vector4& color)
{
	auto pDefinitionManager = this->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return;
	}
	auto pDefinition = pDefinitionManager->getDefinition<IGroup>();
	if (pDefinition == nullptr)
	{
		return;
	}

	ObjectHandleT<IGroup> group(std::unique_ptr<IGroup>(new Group(rectangle, name, color)), pDefinition);
	collection.insertValue(collection.size(), group);
}

INode* NodeEditor::GetNode(size_t id)
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
