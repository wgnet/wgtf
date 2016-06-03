
#include "graph_editor.h"
#include "metadata/graph_editor.mpp"

#include "connection_manager.h"

#include <core_dependency_system/i_interface.hpp>
#include <core_reflection/i_definition_manager.hpp>

namespace wgt
{
GraphEditor::GraphEditor()
{
    IDefinitionManager* defMng = Context::queryInterface<IDefinitionManager>();
    assert(defMng != nullptr);

    ConnectionManager& mng = ConnectionManager::Instance();
    mng.Initialize(defMng);
}

GraphEditor::~GraphEditor()
{
    ConnectionManager::Instance().Finilize();
}

IListModel* GraphEditor::GetConnectorsModel() const
{
    return ConnectionManager::Instance().GetConnectorsModel();
}

IListModel* GraphEditor::GetNodeModel() const
{
    return ConnectionManager::Instance().GetNodeModel();
}

IListModel* GraphEditor::GetRootContextMenuModel() const
{
    return ConnectionManager::Instance().GetRootContextMenuModel();
}

IListModel* GraphEditor::GetNodeContextMenuModel() const
{
    return ConnectionManager::Instance().GetNodeContextMenuModel();
}

IListModel* GraphEditor::GetSlotContextMenuModel() const
{
    return ConnectionManager::Instance().GetSlotContextMenuModel();
}

void GraphEditor::CreateConnection(size_t outputUID, size_t inputUID)
{
    ConnectionManager::Instance().CreateConnection(outputUID, inputUID);
}
} // end namespace wgt
