import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Layouts 2.0

// Component for dislaying an entry in the history timeline
Item {
    id: timelineEntry
    objectName: "WGTimelineEntryDelegate"
    WGComponent { type: "WGTimelineEntryDelegate" }

    // Only access DisplayObject once because it is generated every time in C++
    property variant displayObject: itemData.value.DisplayObject
    property int parentColumnIndex: columnIndex
    property bool isCurrentItem: history.currentIndex == rowIndex

    // Width set by parent
    // Height determined by the batch command
    height: expandingTimelineRow.height

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
                visible: isCurrentItem || arrowButton.containsMouse
                opacity: arrowButton.containsMouse && !isCurrentItem ? 0.3 : 1
            }

            MouseArea {
                id: arrowButton
                objectName: "arrowButton"
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    var modelIndex = history.model.index(rowIndex);
                    history.rowDoubleClicked(mouse, modelIndex);
                }
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
        anchors.right: parent.right

        property bool isApplied: rowIndex <= history.currentIndex

        WGBatchCommand {
            id: timelineCommand

            Layout.fillWidth: true
            Layout.preferredHeight: height

            displayObject: timelineEntry.displayObject
            isCurrentItem: timelineEntry.isCurrentItem
            isApplied: expandingTimelineRow.isApplied
            columnIndex: parentColumnIndex
        }
    }
}

