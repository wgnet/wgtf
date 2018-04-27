import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2

import WGControls 2.0
import WGControls.Styles 2.0

Item
{
    id: groupItem
    objectName: "groupItem"
    WGComponent { type: componentType }
    property var componentType: "NodeGroupArea"

    property var globalMinPosition: mapToItem(graphView, x, y)
    property var globalMaxPosition: mapToItem(graphView, x + width, y + height)

    property bool isDragActive: nodeGroupMouseArea.drag.active

    property string groupTitle

    property vector4d groupColor

    property bool selected: false

    property bool locked: false

    signal setPosition(var xPos, var yPos)
    signal setDimensions(var newX, var newY, var newWidth, var newHeight)

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
            textColor: Qt.rgba(groupColor.x, groupColor.y, groupColor.z, 0.7)
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
        color: Qt.rgba(groupColor.x, groupColor.y, groupColor.z, groupColor.w)
    }

    WGResizeableFrame {
        id: groupFrame

        targetItem: groupItem
        locked: groupItem.locked

        height: parent.height
        width: parent.width

        //visual appearance of the actual group box
        frameStyle: Rectangle {
            color: selected ? Qt.rgba(groupColor.x, groupColor.y, groupColor.z, 0.3) : Qt.rgba(groupColor.x, groupColor.y, groupColor.z, 0.15)
            border.width: defaultSpacing.doubleBorderSize
            border.color: selected ? Qt.rgba(groupColor.x, groupColor.y, groupColor.z, groupColor.w) : Qt.rgba(groupColor.x, groupColor.y, groupColor.z, 0.5)
            radius: defaultSpacing.standardRadius
        }

        onSetDimensions: {
            parent.setDimensions(newX, newY, newWidth, newHeight);
        }
    }

    MouseArea
    {
        id: contextMenuMouseArea
        anchors.fill: parent
        anchors.margins: defaultSpacing.doubleMargin
        acceptedButtons: Qt.RightButton
        preventStealing: true

        onClicked: {
            if(!groupItem.selected) {
                graphView.resetNodesSelection();
                graphView.selectNode(groupItem);
            }

            var coord = mapToItem(graphView, mouse.x, mouse.y);
            graphView.contextMenu.popupPointX = coord.x;
            graphView.contextMenu.popupPointY = coord.y;
            graphView.contextMenu.contextObject = groupItem;
            graphView.contextMenu.popup();
        }
    }

    MouseArea
    {
        id: nodeGroupMouseArea
        anchors.fill: parent
        anchors.margins: defaultSpacing.doubleMargin
        drag.target: groupItem.locked ? null : groupItem
        drag.axis: Drag.XAndYAxis
        acceptedButtons: Qt.LeftButton
        preventStealing: true
        propagateComposedEvents: true

        onPositionChanged: {
            if (!locked && drag.active && !groupItem.selected) {
                graphView.resetNodesSelection();
                graphView.selectNode(groupItem);
            }
        }

        onPressed: {
            if (!locked && !(mouse.modifiers & Qt.ControlModifier)) {
                graphView.resetNodesSelection();
                graphView.selectNode(groupItem);

                canvasContainer.selectArea(
                    canvasContainer.mapFromItem(canvasItem, groupItem.x, groupItem.y),
                    canvasContainer.mapFromItem(canvasItem, groupItem.x + groupItem.width, groupItem.y + groupItem.height),
                    mouse.modifiers & Qt.ShiftModifier)
            }
        }

        onReleased: {
            if(groupItem.selected) {
                graphView.resetNodesSelection();
                graphView.unselectNode(groupItem);
            }
        }
    }
}
