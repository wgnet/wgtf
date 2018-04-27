import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0

WGPanel
{
    id: nodeEditorView
    WGComponent { type: "NodeEditorView" }
    
    color: palette.mainWindowColor
    objectName: "NodeEditorView"
    title: "Node Editor"
    layoutHints: { 'nodeeditor': 1.0 }

    GraphView
    {
        id: graphView
        nodesModel : graph.nodesModel
        connectionsModel: graph.connectionsModel
        nodeClassesModel: graph.nodeClassesModel
        groupModel: graph.nodeGroupModel

        Connections {
            target: nodeEditorView

            onFocusChanged: {
                if ( nodeEditorView.focus === true )
                {
                    graphView.redrawGraph();
                }
            }
        }
    }
}
