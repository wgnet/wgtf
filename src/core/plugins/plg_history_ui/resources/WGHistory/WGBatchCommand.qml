import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Layouts 2.0


Item {
    id: batchCommand
    objectName: "batchCommand"
    WGComponent { type: "WGBatchCommand" }
    height: defaultSpacing.minimumRowHeight


    property var historyItem: null
    property bool currentItem: false
    property bool applied: false
    property int columnIndex: 0


    Item {
        height: parent.height
        width: childCommand.width + batchSummary.width
        WGCommandInstance {
            id: childCommand
            anchors.left: parent.left
            height: parent.height
            historyItem: batchCommand.historyItem
            currentItem: batchCommand.currentItem
            applied: batchCommand.applied
            columnIndex: batchCommand.columnIndex
            batchCommandParent: batchChildList.count > 0
            hovered: batchArea.containsMouse || batchPopup.visible
        }

        MouseArea {
            id: batchArea
            height: parent.height
            width: parent.width + batchSummary.width + defaultSpacing.doubleMargin
            hoverEnabled: batchChildList.count > 0
            acceptedButtons: Qt.NoButton
            propagateComposedEvents: true
            enabled: batchChildList.count > 0
            cursorShape: Qt.PointingHandCursor

            WGPopup {
                id: batchPopup
                openPolicy: openOnHover
                width: batchCommand.parent.width - defaultSpacing.doubleMargin
                x: defaultSpacing.doubleMargin
                Column {
                    id: batchChildColumn

                    Repeater {
                        id: batchChildList
                        model: typeof historyItem === "undefined" ? null : historyItem.Children

                        delegate: WGCommandInstance {
                            id: batchChild
                            historyItem: model.value
                            currentItem: batchCommand.currentItem
                            applied: batchCommand.applied
                            columnIndex: batchCommand.columnIndex
                        }
                    }
                }
            }
        }
        Item {
            id: batchSummary
            anchors.left: childCommand.right
            anchors.leftMargin: defaultSpacing.doubleMargin
            height: parent.height
            width: childrenRect.width

            Repeater {
                model: typeof historyItem === "undefined" ? null : historyItem.Children

                WGCommandInstance {
                    id: summaryChild
                    anchors.verticalCenter: parent.verticalCenter
                    historyItem: model.value
                    currentItem: batchCommand.currentItem
                    applied: batchCommand.applied
                    columnIndex: batchCommand.columnIndex
                    visible: index == 0
                    opacity: batchPopup.visible ? 0.2 : 0.8
                }
            }
        }
    }
}

