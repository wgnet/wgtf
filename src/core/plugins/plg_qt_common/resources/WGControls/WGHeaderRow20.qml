import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0


Item {
    id: itemRow
    objectName: "WGHeaderRow"

    width: childrenRect.width != 0 ? childrenRect.width : 1024
    height: childrenRect.height != 0 ? childrenRect.height : 1024

    property var columnDelegates: []
    property var columnSequence: []
    property var columnWidths: []
    property var headerData: []
    property var sortIndicators: []
    property alias columnSpacing: row.spacing

    /* MOVE INTO STYLE*/
    Rectangle {
        id: backgroundArea
        anchors.fill: row
        color: palette.midDarkColor
        opacity: 1
        visible: true
    }
    /**/

    Row {
        id: row
        Repeater {
            model: columnCount()

            Item {
                width: columnWidths[index]
                height: childrenRect.height
                clip: true
                MouseArea {
                    width: columnWidths[index]
                    height: row.height
                    acceptedButtons: Qt.RightButton | Qt.LeftButton;

                    onClicked: {
                        //TODO handle sort
                    }
                }

                Column {
                    id: columnLayoutRow

                    // sort indicator componenet.
                    Loader {
                        id: sortIndicatorLoader
                        width: itemRow.columnWidths[index]
                        property var headerSortIndex: index;
                        sourceComponent: itemRow.sortIndicators[index]
                    }

                    // header component
                    Loader {
                        id: columnDelegateLoader
                        property var headerData: itemRow.columnSequence.length <= index ? itemRow.headerData[index] :  itemRow.headerData[itemRow.columnSequence[index]]
                        property var headerWidth: itemRow.columnWidths[index] - x
                        sourceComponent: itemRow.columnDelegates[index]
                    }
                }
            }
        }
    }
}

