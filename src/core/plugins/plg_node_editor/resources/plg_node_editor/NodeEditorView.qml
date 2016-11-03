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
            id: graphView
            nodesModel : value.nodesModel
            connectionsModel: value.connectionsModel
            nodeClassesModel: value.nodeClassesModel
            groupModel: value.nodeGroupModel

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
}
