import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import WGControls 2.0

Row
{
    id: slot
    objectName: "Slot"
    WGComponent { type: "Slot" }
    
    property bool isInput
    property var slotObj

    /*! The Node (Qml object) that this slot is part of */
    property var parentNode

    property bool connected

    // TODO: Make this more robust? Not so dependent on slot 'color'?
    property bool validSlot: {
        if (canvasContainer.creatingConnection && canvasContainer.currentConnectionSlot !== slotObj)
        {
            if (canvasContainer.creatingFromInput !== isInput && canvasContainer.creatingColor == slotObj.color && slot.parentNode != canvasContainer.creatingNode)
            {
                return true
            }
            else
            {
                return false
            }
        }
        else
        {
            return true
        }
    }

    property color slotColor: validSlot ? slotObj.color : "#999999"

    property string slotLabel: slotObj.label

    property var style: SlotStyle{
        parentSlot: slot
    }

    signal removeConnection()

    Connections {
        target: canvasContainer
        onUpdateConnections: {
            slot.connected = slotObj.isConnected
        }
    }

    spacing: 2
    height: defaultSpacing.minimumRowHeight
    width: slotIcon.width + slotText.width
    layoutDirection: (isInput) ? Qt.LeftToRight : Qt.RightToLeft

    function getSlotAnchor()
    {
        if (nodeContainer.nodeIsExpanded)
        {
            return mapToItem(graphView, slotIcon.x + slotIcon.width / 2, slotIcon.y + slotIcon.height / 2);
        }
        else
        {
            if (isInput)
                return nodeHeader.mapToItem(graphView, nodeHeader.x - defaultSpacing.standardBorderSize, nodeHeader.y + nodeHeader.height / 2);
            else
                return nodeHeader.mapToItem(graphView, nodeHeader.x + nodeHeader.width, nodeHeader.y + nodeHeader.height / 2)
        }
    }

    Loader
    {
        id: slotIcon
        height: parent.height - defaultSpacing.standardMargin
        width: parent.height - defaultSpacing.standardMargin
        anchors.verticalCenter: parent.verticalCenter

        sourceComponent: style.connector
        asynchronous: true

        opacity: slot.validSlot ? 1.0 : 0.5

        MouseArea
        {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed:
            {
                if (mouse.button == Qt.LeftButton)
                {
                    mouse.accepted = true;
                    canvasContainer.startCreatingNewConnection(slotObj, parentNode);
                }
                else if (mouse.button == Qt.RightButton)
                {
                    mouse.accepted = true;
					if (!canvasContainer.creatingConnection)
					{
						removeConnection();
					}
                }
            }

            onPositionChanged:
            {
                if (canvasContainer.creatingConnection)
                {
                    mouse.accepted = true;
                    canvasContainer.currentConnection.endPos = mapToItem(graphView, mouse.x, mouse.y);
                }
            }

            onReleased:
            {
                if (mouse.button == Qt.LeftButton)
                {
                    canvasContainer.finishCreatingNewConnection(mapToItem(graphView, mouse.x, mouse.y));
                }
            }
        }
    }

    Loader
    {
        id: slotText

        sourceComponent: style.label
        asynchronous: true

        anchors.verticalCenter: parent.verticalCenter

        opacity: slot.validSlot ? 1.0 : 0.5
    }
}
