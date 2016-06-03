
#include "connection_manager.h"
#include "connection_slot.h"

#include "BaseModel.h"

#include "action.h"
#include "graph_node.h"
#include "connection_slot.h"
#include "connector.h"
#include "quick_items_manager.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>

namespace wgt
{
namespace
{
typedef std::function<ObjectHandleT<ConnectionSlot>(ObjectHandleT<GraphNode> const&,
                                                                  const std::string&,
                                                                  const std::string&)> TCreateSlotFn;

const char* greenSlotIcon = "greenSlot.png";
const char* bllueSlotIcon = "blueSlot.png";

void InitNodeType1(ObjectHandleT<GraphNode> const& node, const std::string& nodeTypeId, TCreateSlotFn const& createFn)
{
    GraphNode::Params params;
    params.typeId = nodeTypeId;

    params.inputSlots.push_back(createFn(node, "Exec", greenSlotIcon));
    params.inputSlots.push_back(createFn(node, "Value", greenSlotIcon));
    params.outputSlots.push_back(createFn(node, "Flow", bllueSlotIcon));

    node->SetTitle("Real node from C++");
    node->Init(std::move(params));
}

void InitNodeType2(ObjectHandleT<GraphNode> const& node, const std::string& nodeTypeId, TCreateSlotFn const& createFn)
{
    GraphNode::Params params;
    params.typeId = nodeTypeId;

    params.outputSlots.push_back(createFn(node, "Angle", bllueSlotIcon));
    params.outputSlots.push_back(createFn(node, "Type", bllueSlotIcon));

    node->SetTitle("Second node");
    node->Init(std::move(params));
}
}

class ConnectionManager::GraphNodeModel : public BaseModel<GraphNode>
{
};
class ConnectionManager::ConnectorsModel : public BaseModel<Connector>
{
};
class ConnectionManager::ContextMenuModel : public BaseModel<Action>
{
};

ConnectionManager& ConnectionManager::Instance()
{
    static ConnectionManager manager;
    return manager;
}

ConnectionManager::ConnectionManager()
	: defMng( nullptr )
{
}

ConnectionManager::~ConnectionManager()
{
    assert(graphNodeModel == nullptr);
    assert(connectorsModel == nullptr);
    assert(rootContextMenuModel == nullptr);
    assert(nodeContextMenuModel == nullptr);
    assert(slotContextMenuModel == nullptr);

    assert(nodeToSlots.empty());
    assert(slotToConnector.empty());
    assert(uidToSlot.empty());
}

void ConnectionManager::Initialize(IDefinitionManager* defMng_)
{
    assert(defMng == nullptr);
    defMng = defMng_;

    graphNodeModel.reset(new GraphNodeModel());
    connectorsModel.reset(new ConnectorsModel());

    InitRootContextMenu();
    InitNodeContextMenu();
    InitSlotContextMenu();
}

void ConnectionManager::Finilize()
{
    graphNodeModel.reset();
    connectorsModel.reset();
    rootContextMenuModel.reset();
    nodeContextMenuModel.reset();
    slotContextMenuModel.reset();

    slotToConnector.clear();
    uidToSlot.clear();
    nodeToSlots.clear();

    defMng = nullptr;
}

ObjectHandleT<GraphNode> ConnectionManager::CreateNode(const std::string& nodeTypeId)
{
    using namespace std::placeholders;

    return CreateNode(nodeTypeId, std::bind(&ConnectionManager::CreateSlot, this, _1, _2, _3));
}

ConnectionManager::TNodePtr ConnectionManager::CreateNode(const std::string& nodeTypeId, const TCreateSlotFn& fn)
{
    TNodePtr node = GetDefinitionManager().create<GraphNode>(true);
    node->Changed.connect(std::bind(&ConnectionManager::GraphNodeChanged, this, std::placeholders::_1));
    node->MoveNodes.connect(std::bind(&ConnectionManager::MoveHalf, this, std::placeholders::_1, std::placeholders::_2));

    if (nodeTypeId == "type1")
        InitNodeType1(node, nodeTypeId, fn);
    else
        InitNodeType2(node, nodeTypeId, fn);

    graphNodeModel->AddItem(node);

    return node;
}

void ConnectionManager::DeleteNode(ObjectHandleT<GraphNode> node)
{
    TNodeToSlotsMap::iterator nodeSlots = nodeToSlots.find(node);
    assert(nodeSlots != nodeToSlots.end());

    TSlots& slotCollection = nodeSlots->second;
    for (TSlotPtr& slot : slotCollection)
    {
        TSlotToConnectorsMap::iterator slotConnectors = slotToConnector.find(slot);
        if (slotConnectors != slotToConnector.end())
        {
            TConnectors& connectorCollection = slotConnectors->second;
            for (TConnectorPtr& connector : connectorCollection)
                connectorsModel->RemoveItem(connector);

            connectorCollection.clear();
            slotToConnector.erase(slotConnectors);
        }
    }

    slotCollection.clear();
    graphNodeModel->RemoveItem(node);
    nodeToSlots.erase(nodeSlots);
}

void ConnectionManager::DeleteNode(size_t nodeUid)
{
    auto iter = std::find_if(nodeToSlots.begin(), nodeToSlots.end(), [nodeUid](TNodeToSlotsMap::value_type const& node) {
        return node.first->GetUID() == nodeUid;
    });

    assert(iter != nodeToSlots.end());
    DeleteNode(iter->first);
}

ObjectHandleT<Connector> ConnectionManager::CreateConnection(size_t outputslotId, size_t inputSlotId)
{
    TConnectorPtr connector = GetDefinitionManager().create<Connector>(true);

    connector->Init(outputslotId, inputSlotId);
    slotToConnector[GetSlot(outputslotId)].push_back(connector);
    slotToConnector[GetSlot(inputSlotId)].push_back(connector);
    connectorsModel->AddItem(connector);

    return connector;
}

void ConnectionManager::DeleteConnection(Connector* connection)
{
    assert(false);
}

void ConnectionManager::Disconnect(size_t slotId)
{
    assert(false);
}

IListModel* ConnectionManager::GetNodeModel() const
{
    return graphNodeModel.get();
}

IListModel* ConnectionManager::GetConnectorsModel() const
{
    return connectorsModel.get();
}

IListModel* ConnectionManager::GetRootContextMenuModel() const
{
    return rootContextMenuModel.get();
}

IListModel* ConnectionManager::GetNodeContextMenuModel() const
{
    return nodeContextMenuModel.get();
}

IListModel* ConnectionManager::GetSlotContextMenuModel() const
{
    return slotContextMenuModel.get();
}

ConnectionManager::TSlotPtr ConnectionManager::CreateSlot(TNodePtr const& parentNode, const std::string& title,
                                                          const std::string& icon)
{
    TSlotPtr slot = GetDefinitionManager().create<ConnectionSlot>(true);

    ConnectionSlot::Params params(title, icon, parentNode);
    slot->Init(std::move(params));
    nodeToSlots[parentNode].push_back(slot);
    uidToSlot[slot->GetUID()] = slot;

    return slot;
}

IDefinitionManager& ConnectionManager::GetDefinitionManager()
{
    assert(defMng != nullptr);
    return *defMng;
}

void ConnectionManager::InitRootContextMenu()
{
    rootContextMenuModel.reset(new ContextMenuModel());

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    IDefinitionManager& mng = GetDefinitionManager();
    ObjectHandleT<Action> saveAction = mng.create<Action>(true);
    saveAction->SetParams("Save", std::bind(&ConnectionManager::SaveGraph, this, _1, _2, _3));
    ObjectHandleT<Action> loadAction = mng.create<Action>(true);
    loadAction->SetParams("Load", std::bind(&ConnectionManager::LoadGraph, this, _1, _2, _3));

    ObjectHandleT<Action> createType1 = mng.create<Action>(true);
    createType1->SetParams("Type 1", std::bind(&ConnectionManager::CreateNodeHelper, this, _1, _2, _3, "type1"));
    ObjectHandleT<Action> createType2 = mng.create<Action>(true);
    createType2->SetParams("Type 2", std::bind(&ConnectionManager::CreateNodeHelper, this, _1, _2, _3, "type2"));

    rootContextMenuModel->AddItem(saveAction);
    rootContextMenuModel->AddItem(loadAction);
    rootContextMenuModel->AddItem(createType1);
    rootContextMenuModel->AddItem(createType2);
}

void ConnectionManager::InitNodeContextMenu()
{
    nodeContextMenuModel.reset(new ContextMenuModel());

    IDefinitionManager& mng = GetDefinitionManager();
    ObjectHandleT<Action> deleteNode = mng.create<Action>(true);
    deleteNode->SetParams("Delete", [this](float x, float y, size_t objectUid) { DeleteNode(objectUid); });

    nodeContextMenuModel->AddItem(deleteNode);
}

void ConnectionManager::InitSlotContextMenu()
{
    slotContextMenuModel.reset(new ContextMenuModel());

    IDefinitionManager& mng = GetDefinitionManager();
    ObjectHandleT<Action> deleteNode = mng.create<Action>(true);
    deleteNode->SetParams("I don't know", [this](float x, float y, size_t objectUid) {});

    slotContextMenuModel->AddItem(deleteNode);
}

void ConnectionManager::SaveGraph(float x, float y, size_t objectUid)
{
    QString graphFile = QFileDialog::getSaveFileName(nullptr, "Save Graph", QString(), "Graph (*.gr)");
    if (graphFile.isEmpty())
    {
        return;
    }

    if (!graphFile.endsWith(".gr"))
    {
        graphFile += ".gr";
    }

    QFile file(graphFile);
    if (file.open(QIODevice::WriteOnly) == false)
    {
        assert(false);
        return;
    }

    QTextStream stream(&file);
    stream << nodeToSlots.size() << "\n";
    for (TNodeToSlotsMap::value_type const& mapNode : nodeToSlots)
    {
        TNodePtr node = mapNode.first;
        stream << QString::fromStdString(node->GetType()) << " " << node->GetPosX() << " " << node->GetPosY() << " " << node->GetUID() << "\n";

        TSlots const& slotCollection = mapNode.second;
        stream << slotCollection.size() << "\n";
        for (TSlotPtr const& slot : slotCollection)
        {
            stream << QString::fromStdString(slot->GetTitle()) << " " << slot->GetUID() << "\n";
        }
    }

    std::unordered_set<TConnectorPtr, ObjectHandleHash<Connector>> connectorSet;

    for (TSlotToConnectorsMap::value_type const& mapNode : slotToConnector)
    {
        connectorSet.insert(mapNode.second.begin(), mapNode.second.end());
    }

    stream << connectorSet.size() << "\n";
    for (TConnectorPtr const& connector : connectorSet)
    {
        stream << connector->GetOutputSlotId() << " " << connector->GetInputSlotId() << "\n";
    }

    stream.flush();
    file.close();
}

void ConnectionManager::LoadGraph(float x, float y, size_t objectUid)
{
    QString graphFile = QFileDialog::getOpenFileName(nullptr, "Open Graph", QString(), "Graph (*.gr)");
    if (graphFile.isEmpty())
        return;

    ClearModel();

    QFile file(graphFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        assert(false);
        return;
    }

    std::map<size_t, size_t> slotMapping;

    QTextStream stream(&file);
    size_t nodeCount = 0;
    stream >> nodeCount;
    for (size_t i = 0; i < nodeCount; ++i)
    {
        QString typeId;
        QPointF pos;
        size_t uid;
        stream >> typeId >> pos.rx() >> pos.ry() >> uid;

        std::map<std::string, size_t> slotMap;

        size_t slotCount = 0;
        stream >> slotCount;
        for (size_t j = 0; j < slotCount; ++j)
        {
            QString name;
            size_t slotUid;
            stream >> name >> slotUid;
            slotMap[name.toStdString()] = slotUid;
        }

        TNodePtr node = CreateNode(typeId.toStdString(), [&](const TNodePtr& node, const std::string& title, const std::string& icon) {
            TSlotPtr slot = CreateSlot(node, title, icon);
            auto iter = slotMap.find(slot->GetTitle());
            assert(iter != slotMap.end());

            slotMapping[iter->second] = slot->GetUID();

            return slot;
        });
        node->SetPosX(pos.x());
        node->SetPosY(pos.y());
    }

    size_t connectionCount = 0;
    stream >> connectionCount;
    for (size_t i = 0; i < connectionCount; ++i)
    {
        size_t output, input;
        stream >> output >> input;
        CreateConnection(slotMapping[output], slotMapping[input]);
    }
}

void ConnectionManager::CreateNodeHelper(float x, float y, size_t objectUid, const std::string& typeId)
{
    TNodePtr node = CreateNode(typeId);
    node->SetPosX(x);
    node->SetPosY(y);
}

void ConnectionManager::ClearModel()
{
    std::unordered_set<TConnectorPtr, ObjectHandleHash<Connector>> connectorSet;

    for_each(slotToConnector.begin(), slotToConnector.end(), [&connectorSet](TSlotToConnectorsMap::value_type const& mapNode) {
        connectorSet.insert(mapNode.second.begin(), mapNode.second.end());
    });

    for_each(connectorSet.begin(), connectorSet.end(), [this](TConnectorPtr const& connector) {
        connectorsModel->RemoveItem(connector);
    });

    for_each(nodeToSlots.begin(), nodeToSlots.end(), [this](TNodeToSlotsMap::value_type const& mapNode) {
        graphNodeModel->RemoveItem(mapNode.first);
    });

    slotToConnector.clear();
    uidToSlot.clear();
    nodeToSlots.clear();
}

void ConnectionManager::GraphNodeChanged(GraphNode* node)
{
    QuickItemsManager& mng = QuickItemsManager::Instance();

    TNodePtr nodePtr(node);

    auto nodeIter = nodeToSlots.find(nodePtr);
    assert(nodeIter != nodeToSlots.end());

    for (TSlotPtr& slot : nodeIter->second)
    {
        auto slotIter = slotToConnector.find(slot);
        if (slotIter != slotToConnector.end())
        {
            for (TConnectorPtr& connector : slotIter->second)
                mng.RepaintItem(connector->GetUID());
        }
    }
}

void ConnectionManager::MoveHalf(float x, float y)
{
    size_t count = nodeToSlots.size() >> 2;
    count = std::min(count, static_cast<size_t>(100));
    size_t counter = 0;
    auto iter = nodeToSlots.begin();
    while (counter < count)
    {
        iter->first->ShiftImpl(x, y);
        iter++;
        counter++;
    }
}

ObjectHandleT<ConnectionSlot> ConnectionManager::GetSlot(size_t slotUID)
{
    auto iter = uidToSlot.find(slotUID);
    if (iter != uidToSlot.end())
        return iter->second;

    return TSlotPtr();
}
} // end namespace wgt
