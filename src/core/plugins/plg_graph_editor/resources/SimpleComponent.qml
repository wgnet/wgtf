import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import WGControls 1.0

Rectangle
{
    id : root
    property var node
    property var nodeContextMenu
    opacity: 1.0

    color: palette.mainWindowColor

    radius: defaultSpacing.standardRadius

    width : mainLayout.width + defaultSpacing.doubleBorderSize + defaultSpacing.doubleMargin
    height : mainLayout.height + defaultSpacing.doubleBorderSize + defaultSpacing.doubleMargin

    Rectangle {
        color: palette.darkestShade

        height: parent.height
        width: parent.width

        x: 1
        y: 1

        radius: defaultSpacing.standardRadius
        z: -1
    }

    MouseArea {
        id: dragHandle
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        preventStealing: true

        property var mouseDragStart

        onPositionChanged:
        {
            if(mouseDragStart && (mouse.buttons & Qt.LeftButton))
            {
                var pos = mapToItem(graphCanvasObject, mouse.x, mouse.y)
                var delta = Qt.point(pos.x - mouseDragStart.x, pos.y - mouseDragStart.y)
                node.shiftNode(delta.x, delta.y)
                mouseDragStart = pos
            }
        }

        onPressed :
        {
            mouseDragStart = mapToItem(graphCanvasObject, mouse.x, mouse.y)
        }

        onReleased :
        {
            mouseDragStart = null
        }
    }

    ContextMenu {
        id : contextMenu
        z : root.z
        menuModel : nodeContextMenu
        contextObjectUid : node.uid
    }

    ColumnLayout {
        id : mainLayout
        anchors.centerIn: parent
        Text
        {
            id : header
            Layout.fillHeight : true
            Layout.alignment : Qt.AlignHCenter | Qt.AlignVCenter
            text : node.title
            color: palette.textColor
        }

        WGSeparator
        {
            Layout.fillWidth : true
            Layout.preferredHeight : defaultSpacing.separatorWidth
            vertical: false
        }

        WGExpandingRowLayout
        {
            ColumnLayout
            {
                Layout.alignment : Qt.AlignTop | Qt.AlignLeft

                WGListModel
                {
                    id : inputSlotsModel
                    source : node.inputSlots
                    ValueExtension {}
                }

                Repeater
                {
                    id : inputSlotsRepeater
                    model : inputSlotsModel
                    delegate : InputSlot
                    {
                        z : root.z + 10
                        nodeObject : root
                        slotObject : value

                    }
                }
            }

            Item
            {
                Layout.fillWidth: true
            }

            ColumnLayout
            {
                Layout.alignment : Qt.AlignTop | Qt.AlignRight
                WGListModel
                {
                    id : outputSlotsModel
                    source : node.outputSlots
                    ValueExtension {}
                }

                Repeater
                {
                    id : outputSlotsRepeater
                    model : outputSlotsModel
                    Layout.alignment : Qt.AlignTop | Qt.AlignRight
                    delegate : OutputSlot
                    {
                        z : root.z + 10
                        nodeObject : root
                        slotObject : value
                        Layout.alignment : Qt.AlignTop | Qt.AlignRight
                    }
                }
            }
        }
    }
}
