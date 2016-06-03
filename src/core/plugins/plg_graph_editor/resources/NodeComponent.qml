import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import WGControls 1.0

Rectangle {
    id : root
    property var node
    property var nodeContextMenu

    //TODO link to data
    property color nodeColor: "red"

    property string nodeTitle: node.title

    //TODO link to data
    property string nodeSubTitle: "Example sub title"

    //TODO link to data
    property string nodeIcon: "model_16x16.png"

    property bool nodeIsExpanded: true

    x : node.nodePosX
    y : node.nodePosY

    color: palette.mainWindowColor

    radius: defaultSpacing.standardRadius

    width : mainLayout.width + defaultSpacing.doubleBorderSize + defaultSpacing.doubleMargin
    height : mainLayout.height + defaultSpacing.doubleBorderSize + defaultSpacing.doubleMargin

    onNodeIsExpandedChanged: {
        //Hack to redraw the canvas so the lines update
        //graphCanvasObject.viewTransform.shift(0, 0)
    }
    Rectangle {
        color: palette.darkestShade

        height: parent.height
        width: parent.width

        x: 1
        y: 1

        radius: defaultSpacing.standardRadius
        z: -1
    }

    Rectangle {
        color: nodeColor

        anchors.centerIn: parent

        height: parent.height - defaultSpacing.doubleMargin
        width: parent.width + defaultSpacing.doubleMargin - defaultSpacing.doubleBorderSize

        visible: !nodeIsExpanded

        radius: defaultSpacing.halfRadius
        z: -2
    }

    MouseArea
    {
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

        // Expands/contracts the node
        onDoubleClicked: {
            nodeIsExpanded = !nodeIsExpanded
        }
    }

    ContextMenu
    {
        id : contextMenu
        z : root.z
        menuModel : nodeContextMenu
        contextObjectUid : node.uid
    }

    ColumnLayout
    {
        id : mainLayout
        anchors.centerIn: parent

        WGExpandingRowLayout {
            id: headerBox
            Layout.fillWidth: true

            Image
            {
                source: nodeIcon
                visible: nodeIcon != ""
            }

            WGLabel
            {
                id : header
                Layout.fillHeight : true
                Layout.alignment : Qt.AlignHCenter | Qt.AlignVCenter
                text : nodeTitle

                // Text.QtRendering looks a bit fuzzier sometimes but is much better for arbitrary scaling
                renderType: Text.QtRendering
            }

            Item {
                Layout.fillWidth: true
            }

            // Expand/Contract Icon/Button
            WGToolButton
            {
                Layout.preferredHeight: 20
                Layout.preferredWidth: 20
                iconSource: nodeIsExpanded ? "arrow_down_small_16x16.png" : "arrow_right_small_16x16.png"
                onClicked: {
                    nodeIsExpanded = !nodeIsExpanded
                }
            }


        }
        // Placeholder subtitle
        WGMultiLineText
        {
            id : subHeader
            Layout.fillHeight : true
            Layout.preferredWidth: header.width
            Layout.alignment : Qt.AlignLeft | Qt.AlignVCenter
            text : nodeSubTitle
            color: palette.disabledTextColor

            visible: nodeIsExpanded && nodeSubTitle != ""

            // Text.QtRendering looks a bit fuzzier but is much better for arbitrary scaling
            renderType: Text.QtRendering
        }

        Rectangle
        {
            Layout.fillWidth : true
            Layout.preferredHeight : defaultSpacing.separatorWidth
            color: nodeColor
            visible: nodeIsExpanded
        }

        // Feels like a slot should be one object with an input and/or output and potentially an edit control in the middle rather than two separate slots.
        // This would make it easier to line everything up into three columns as well.

        WGExpandingRowLayout {
            id: nodes
            visible: nodeIsExpanded

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
            // TODO Loader for an edit control?
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
