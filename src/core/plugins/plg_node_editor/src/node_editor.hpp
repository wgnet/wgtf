#ifndef __DEFAULT_NODE_EDITOR_H__
#define __DEFAULT_NODE_EDITOR_H__

#include "core_dependency_system/depends.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_data_model/generic_list.hpp"

#include "interfaces/i_node_editor.hpp"

namespace wgt
{
class IComponentContext;
class IDefinitionManager;

class NodeEditor : public Implements<INodeEditor>
                   ,
                   public Depends<IDefinitionManager>
{
	DECLARE_REFLECTED
public:
	NodeEditor(IComponentContext& context);
	virtual ~NodeEditor()
	{
	}

	void SetGraph(std::shared_ptr<IGraph> graph) override;

	std::shared_ptr<INode> CreateNode(std::string nodeClass, float x, float y) override;
	INode* GetNode(size_t id) override;
	bool DeleteNode(size_t id) override;    

    bool Connect(size_t nodeIdFrom, size_t slotIdFrom,
        size_t nodeIdTo, size_t slotIdTo) override;
    bool Disconnect(size_t nodeIdFrom, size_t slotIdFrom,
        size_t nodeIdTo, size_t slotIdTo) override;

private:
    void onCreateNode(int x, int y, std::string nodeClass) override;
    void onDeleteNode(size_t nodeID) override;

    void onCreateConnection(size_t nodeIdFrom, size_t slotIdFrom, size_t nodeIdTo, size_t slotIdTo) override;
    void onDeleteConnection(size_t connectionId) override;

    const IListModel* GetGraphModel() const override { return &graphModel; }

	virtual void CreateGroup(Collection& collection,
	                         const Vector4& rectangle,
	                         const std::string& name,
	                         const Vector4& color) override;

private:
    GenericListT<ObjectHandleT<IGraph>> graphModel;
};
} // end namespace wgt
#endif // __DEFAULT_NODE_EDITOR_H__
