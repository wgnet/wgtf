import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle
{
    id: nodeEditorView
    color: palette.mainWindowColor

    property var title: "Node Editor"

    WGListModel
    {
        id: graphListModel
        source: graphModel

        ValueExtension {}
    }

    Repeater
    {
        id: graphRender
        model: graphListModel
        delegate: GraphView
        {
            nodesModel : value.nodesModel
            connectionsModel: value.connectionsModel
            nodeClassesModel: value.nodeClassesModel
        }
    }
}
