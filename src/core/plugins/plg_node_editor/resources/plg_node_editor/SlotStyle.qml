import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import WGControls 2.0

Item
{
    id: slotStyle
    WGComponent { type: "SlotStyle" }

    property QtObject parentSlot: parent

    property Component connector: Rectangle
    {
        id: connector
        height: parentSlot.height - defaultSpacing.standardMargin
        width: parentSlot.height - defaultSpacing.standardMargin
        anchors.verticalCenter: parent.verticalCenter

        radius: width / 2

        color: parentSlot.enabled ? parentSlot.slotColor : "#999999"

        Rectangle {
            id: slotSocket
            anchors.fill: parent
            anchors.margins: defaultSpacing.doubleBorderSize
            radius: width / 2

            color: parentSlot.connected ? "#33FFFFFF" : "#66000000"
        }
    }

    property Component label: WGLabel
    {
        id: slotLabel
        text: parentSlot.slotLabel
    }
}
