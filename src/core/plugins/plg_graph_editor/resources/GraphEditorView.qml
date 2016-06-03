import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.2
import WGControls 1.0
import DAVA 1.0

Rectangle
{
    id: graphEditorComponent
    objectName: "GraphEditorView"
    color: palette.mainWindowColor

    property var title: "Graph Editor"
    property var layoutHints: { 'left': 0.5 }

    property var slotsIndex : []
    property var graphCanvasObject : graphCanvas
    property var objectsContainer : graphObject
    property var connectionStartSlot
    property var interactiveConnectionCurve : connectionCurve

    function connect(output, input)
    {
        createConnection(output, input)
    }

    function getNodeContextMenu()
    {
        return nodeMenuModel
    }

    function getSlotContextMenu()
    {
        return slotMenuModel
    }

    Rectangle
    {
        id: graphCanvas;
        anchors.fill: parent

        color: palette.lightPanelColor

        property var viewTransform: ViewTransform
        {
            container: graphCanvas
        }

        Image {
            anchors.fill: parent
            source: "grid_line.png"
            fillMode: Image.Tile
        }

        MouseArea {
            anchors.fill: parent;
            acceptedButtons: Qt.AllButtons
            z : graphCanvas.z

            property var mouseDragStart;

            onWheel:
            {
                var delta = 1 + wheel.angleDelta.y/120.0 * .1;
                // Zoom into the current mouse location
                var screenPos = Qt.point(wheel.x, wheel.y)
                var oldPos = graphCanvas.viewTransform.inverseTransform(screenPos);
                graphCanvas.viewTransform.xScale *= delta;
                graphCanvas.viewTransform.yScale *= delta;
                var newScreenPos = graphCanvas.viewTransform.transform(Qt.point(oldPos.x, oldPos.y));
                var shift = Qt.point(screenPos.x - newScreenPos.x, screenPos.y - newScreenPos.y)
                graphCanvas.viewTransform.shift(shift);
            }

            onPositionChanged:
            {
                if(mouseDragStart && (mouse.buttons & Qt.MiddleButton))
                {
                    var pos = Qt.point(mouse.x, mouse.y)
                    var delta = Qt.point(pos.x - mouseDragStart.x, pos.y - mouseDragStart.y)
                    mouseDragStart = pos
                    graphCanvas.viewTransform.shift(delta);
                }
            }

            onPressed:
            {
                mouseDragStart = Qt.point(mouse.x, mouse.y)
            }

            onReleased:
            {
                mouseDragStart = null;
            }
        }

        WGListModel
        {
            id : nodesModel
            source : nodes

            ValueExtension {}
        }

        WGListModel
        {
            id : connectors
            source : connectorsModel

            ValueExtension {}
        }

        Item
        {
            id : graphObject
            z : 50

            transform : [
            Scale
            {
                origin.x : 0
                origin.y : 0
                xScale : graphCanvasObject.viewTransform.xScale
                yScale : graphCanvasObject.viewTransform.xScale
            },
            Translate
            {
                x : graphCanvasObject.viewTransform.origin.x
                y : graphCanvasObject.viewTransform.origin.y
            }
            ]

            Repeater
            {
                model : connectors
                delegate : ConnectionItem
                {
                    outputSlot : slotsIndex[value.outputSlot]
                    inputSlot : slotsIndex[value.inputSlot]
                    uid : value.uid
                }
            }

            InteractiveConnectionItem
            {
                id : connectionCurve
                visible : false
            }

            Repeater
            {
                id: itemRepeater
                model: nodesModel
                delegate: NodeComponent
                {
                    z : graphCanvas.z + 10
                    node : value
                    nodeContextMenu : getNodeContextMenu()
                }
            }
        }

        ContextMenu
        {
            z: graphCanvas.z
            menuModel : contextMenuModel
        }
    }
}
