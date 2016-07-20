import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import WGControls 1.0

Item
{
    id: nodeContainer
    objectName: "Node"
    property var nodeObj
    property var nodeID
    property var nodeTitle
    property var inputSlotsModel
    property var outputSlotsModel
    property var globalPosition: mapToItem(graphView, x, y)
    property bool isDragActive: nodeMouseArea.drag.active

    property bool nodeIsExpanded: true

    property bool selected: false

    property string nodeSubTitle: nodeObj.subtitle
    property color nodeColor: nodeObj.color
    property string nodeIcon: nodeObj.icon

    property var style: NodeStyle{
        node: nodeContainer
    }

    Component.onCompleted: { console.log(nodeObj, nodeObj.color); }

    width: nodeFrame.width
    height: nodeFrame.height

    onGlobalPositionChanged: nodeObj.setPos(globalPosition.x, globalPosition.y)
    onIsDragActiveChanged:
    {
        dragStateChanged(this)
    }

    signal dragStateChanged(var node);

    function getSlotViewBySlotObj(slotObj)
    {
        var isInput = slotObj.isInput;
        var listModel = (isInput) ? inputSlotsRepeater : outputSlotsRepeater;
        var slotView = null;

        var size = listModel.count;
        for (var i = 0; i < size; ++i)
        {
            slotView = listModel.itemAt(i);
            if (slotView.slotObj.id == slotObj.id)
            {
                return slotView;
            }
        }

        return null;
    }

    function getSlotByPos(Pos)
    {
        var slotPos = mapToItem(inputSlotsLayout, Pos.x, Pos.y);
        var slot = inputSlotsLayout.childAt(slotPos.x, slotPos.y);

        if (!slot)
        {
            slotPos = mapToItem(outputSlotsLayout, Pos.x, Pos.y);
            slot = outputSlotsLayout.childAt(slotPos.x, slotPos.y);
        }

        if (slot && slot.objectName == "Slot")
        {
            return slot;
        }

        return null;
    }

    Loader {
        id: nodeFrame
        sourceComponent: style.background
        asynchronous: true

        width : mainLayout.width + defaultSpacing.doubleBorderSize + defaultSpacing.doubleMargin
        height : mainLayout.height + defaultSpacing.doubleBorderSize + defaultSpacing.doubleBorderSize

        Loader  {

            id: nodeHeader
            sourceComponent: style.titleBar
            asynchronous: true

            anchors.top: parent.top
            anchors.margins: defaultSpacing.standardBorderSize
            anchors.left: parent.left
            anchors.right: parent.right
            height: defaultSpacing.minimumRowHeight
        }

        MouseArea
        {
            id: nodeMouseArea
            anchors.fill: parent
            drag.target: nodeContainer
            drag.axis: Drag.XAndYAxis
            acceptedButtons: Qt.LeftButton
            preventStealing: true


            onPositionChanged:
            {
                if (drag.active && !nodeContainer.selected)
                {
                    graphView.resetNodesSelection();
                    graphView.selectNode(nodeContainer);
                }
            }

            onClicked: {
                if (!(mouse.modifiers & Qt.ControlModifier))
                {
                    graphView.resetNodesSelection();
                    graphView.selectNode(nodeContainer);
                    return;
                }

                if (selected)
                    graphView.unselectNode(nodeContainer);
                else
                    graphView.selectNode(nodeContainer);
            }
            onDoubleClicked: {
                nodeIsExpanded = !nodeIsExpanded
            }
        }

        ContextMenu
        {
            id: contextArea
            menuPath: "NodeEditor.node"

            onAboutToShow: {
                contextObject = nodeContainer
            }
        }

        ColumnLayout
        {
            id : mainLayout
            anchors.centerIn: parent

            Loader {
                id: headerBox
                sourceComponent: style.label
                asynchronous: true
                Layout.fillWidth: true
            }

            Loader
            {
                id : subHeader
                Layout.fillHeight : true
                Layout.preferredWidth: headerBox.width
                Layout.alignment : Qt.AlignLeft | Qt.AlignVCenter
                visible: nodeIsExpanded && nodeSubTitle != ""

                sourceComponent: style.subHeader
                asynchronous: true
            }

            // Feels like a slot should be one object with an input and/or output and potentially an edit control in the middle rather than two separate slots.
            // This would make it easier to line everything up into three columns as well.

            WGExpandingRowLayout {
                id: nodes
                visible: nodeIsExpanded

                ColumnLayout
                {
                    id: inputSlotsLayout
                    Layout.alignment : Qt.AlignTop | Qt.AlignLeft

                    WGListModel
                    {
                        id : inputSlotsListModel
                        source : inputSlotsModel

                        ValueExtension {}
                    }

                    Repeater
                    {
                        id : inputSlotsRepeater
                        model : inputSlotsListModel
                        delegate : Slot
                        {
                            z : nodeContainer.z + 10
                            slotObj: value
                            isInput: value.isInput
                            connected: value.isConnected
                            parentNode: nodeContainer
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
                    id: outputSlotsLayout
                    Layout.alignment : Qt.AlignTop | Qt.AlignRight

                    WGListModel
                    {
                        id: outputSlotsListModel
                        source: outputSlotsModel

                        ValueExtension {}
                    }


                    Repeater
                    {
                        id : outputSlotsRepeater
                        model : outputSlotsListModel
                        Layout.alignment : Qt.AlignTop | Qt.AlignRight
                        delegate : Slot
                        {
                            z : nodeContainer.z + 10
                            slotObj: value
                            isInput: value.isInput
                            connected: value.isConnected
                            parentNode: nodeContainer
                            Layout.alignment : Qt.AlignTop | Qt.AlignRight
                        }
                    }
                }
            }
        }
    }
}
