import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Layouts 2.0


Item {
    id: timelineEntry
    objectName: "WGTimelineEntryDelegate"
    WGComponent { type: "WGTimelineEntryDelegate" }
    // Width set by parent, height determined by the batch command
    implicitHeight: expandingTimelineRow.height

    property var view
    property var historyItem
    property int columnIndex
    property bool currentItem
    property bool applied

    // Keep timeline arrow a fixed size
    Row {
        id: fixedTimelineRow
        objectName: "timelineArrow"
        anchors.left: parent.left
        anchors.top: expandingTimelineRow.top
        anchors.bottom: expandingTimelineRow.bottom

        //Current item column & arrow
        Item {
            id: timeline
            width: defaultSpacing.minimumRowHeight
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Image {
                id: currentArrow
                objectName: "arrowImage"
                anchors.centerIn: parent
                source: "icons/16/marker_right_16x16.png"
                visible: currentItem || arrowButton.containsMouse
                opacity: !currentItem && arrowButton.containsMouse ? 0.3 : 1
            }

            MouseArea {
                id: arrowButton
                objectName: "arrowButton"
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: view.itemDoubleClicked(mouse, itemModelIndex, itemModelIndex)
            }
        }

        WGSeparator {
            id: timelineSeparator
            vertical: true
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }
    }

    RowLayout {
        id: expandingTimelineRow
        objectName: "expandingTimelineRow"
        anchors.left: fixedTimelineRow.right
        anchors.right: timelineEntry.right
        height: timelineCommand.height

        WGBatchCommand {
            id: timelineCommand
            Layout.fillWidth: true
            Layout.preferredHeight: height
            historyItem: timelineEntry.historyItem
            currentItem: timelineEntry.currentItem
            applied: timelineEntry.applied
            columnIndex: timelineEntry.columnIndex
        }
    }
}

