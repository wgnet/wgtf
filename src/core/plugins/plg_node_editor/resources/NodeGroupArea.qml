import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2
import WGControls 1.0
import WGControls 2.0

Item
{
    id: groupItem
    objectName: "groupItem"

    property var globalMinPosition: mapToItem(graphView, x, y)
    property var globalMaxPosition: mapToItem(graphView, x + width, y + height)

    property bool isDragActive: nodeGroupMouseArea.drag.active

    property string groupTitle

    property color groupColor

    property bool selected: false

    property bool locked: false

    signal setPosition(var xPos, var yPos)

    signal deleteNode()

    signal changeColor()

    signal changeTitle(var title)

    onIsDragActiveChanged:
    {
        dragStateChanged(this)
        if (!isDragActive){
            setPosition(globalMinPosition.x,globalMinPosition.y)
        }
    }

    signal dragStateChanged(var group);

    // stops bad connections
    function getSlotViewBySlotObj(slotObj)
    {
        return null
    }

    function getSlotByPos(Pos)
    {
        return null;
    }

    ContextMenu
    {
        id: contextArea
        menuPath: "NodeEditor.group"

        onAboutToShow: {
            contextObject = groupItem
        }
    }

    // editable header at the top of the group box
    WGTextBox {
        id: titleBox
        parent: groupFrame.frameBorder

        useContextMenu: false

        anchors.bottom: parent.top
        anchors.left: parent.left
        anchors.margins: defaultSpacing.standardMargin

        // increases title size when zooming out up to a limit
        property real scaleFactor: canvasContainer.viewTransform.xScale >= 1.0 ? 1.0 : Math.max(canvasContainer.viewTransform.xScale, 0.2)

        height: (defaultSpacing.minimumRowHeight + defaultSpacing.standardMargin) / scaleFactor

        width: parent.width

        enabled: !locked

        text: groupTitle

        font.bold: true
        font.pointSize: 10 / scaleFactor

        onEditingFinished: {
            changeTitle(text)
        }

        style: WGTextBoxStyle {
            background: Item {}
            textColor: Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.7)
        }

        // focus is not dropping because nothing else in node editor has focus items. Bit of a hack to stop editing state.
        Connections {
            target: canvasContainer
            onCanvasPressed: {
                titleBox.focus = false
            }
        }
        Connections {
            target: groupItem
            onIsDragActiveChanged: {
                titleBox.focus = false
            }
            onSelectedChanged: {
                titleBox.focus = false
            }
        }
    }

    Text {
        id: lockedText
        parent: groupFrame.frameBorder

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: defaultSpacing.doubleMargin
        height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin

        opacity: 0.5
        horizontalAlignment: Text.AlignRight
        text: "(locked)"
        visible: locked
        color: groupColor
    }

    WGResizeableFrame {
        id: groupFrame

        locked: groupItem.locked

        height: parent.height
        width: parent.width

        //visual appearance of the actual group box
        frameStyle: Rectangle {
            color: selected ? Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.3) : Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.15)
            border.width: defaultSpacing.doubleBorderSize
            border.color: selected ? groupColor : Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.5)
            radius: defaultSpacing.standardRadius
        }
    }

    MouseArea
    {
        id: nodeGroupMouseArea
        anchors.fill: parent
        anchors.margins: defaultSpacing.doubleMargin
        drag.target: groupItem
        drag.axis: Drag.XAndYAxis
        acceptedButtons: Qt.LeftButton
        preventStealing: true

        enabled: !locked

        onPositionChanged:
        {
            if (drag.active && !groupItem.selected)
            {
                graphView.resetNodesSelection();
                graphView.selectNode(groupItem);
            }
        }

        onPressed: {
            if (!(mouse.modifiers & Qt.ControlModifier))
            {
                graphView.resetNodesSelection();
                graphView.selectNode(groupItem);

                canvasContainer.selectArea(canvasContainer.mapFromItem(canvasItem, groupItem.x, groupItem.y),
                                           canvasContainer.mapFromItem(canvasItem, groupItem.x + groupItem.width, groupItem.y + groupItem.height), mouse.modifiers & Qt.ShiftModifier)
                return;
            }
        }
        onReleased: {
            graphView.resetNodesSelection();
            graphView.unselectNode(groupItem);
        }
    }
}
