import QtQuick 2.0

/*!
 \brief Controls behaviour for resizable column widths and spacing.
 Adds the mouse handles between columns.
 */
Row {
    id: columnsFrame
    width: childrenRect.width

    property var columnWidths: []
    property real columnSpacing: 0

    property real availableWidth: 0

    onAvailableWidthChanged: {
        var columnCount = columnWidths.length
        if (columnCount == 0) {
            return
        }

        var currentWidth = 0
        for (var i = 0; i < columnCount; ++i) {
            currentWidth += columnWidths[i] + columnSpacing
        }
        if (__previousAvailableWidth == 0) {
            __previousAvailableWidth = currentWidth
        }
        var totalWidthDelta = availableWidth - __previousAvailableWidth
        if (totalWidthDelta < 0 && availableWidth >= currentWidth) {
            __previousAvailableWidth = availableWidth
            return
        }

        while (totalWidthDelta != 0) {
            var resizeCount = columnCount
            if (totalWidthDelta < 0) {
                for (var i = 0; i < columnCount; ++i) {
                    if (columnWidths[i] == 1) {
                        --resizeCount
                    }
                }
            }

            var desiredWidthDelta = totalWidthDelta / resizeCount
            for (var i = 0; i < columnCount; ++i) {
                var widthDelta = Math.max(1 - columnWidths[i], desiredWidthDelta)
                if (widthDelta == 0) {
                    continue
                }

                var handle = handles.itemAt(i).children[0]
                handle.x += widthDelta
                totalWidthDelta -= widthDelta
            }
        }
        __previousAvailableWidth = availableWidth
    }

    Component.onCompleted: {
        __initialColumnWidths = columnWidths
    }

    Repeater {
        id: handles
        model: __initialColumnWidths

        Item {
            width: handle.x + handle.width
            height: columnsFrame.height

            Item {
                id: handle

                x: __initialColumnWidths[index]
                width: columnSpacing
                height: columnsFrame.height

                /* MOVE INTO STYLE*/
                Rectangle {
                    anchors.fill: parent
                    color: palette.darkColor
                }
                /**/

                MouseArea {
                    anchors.verticalCenter: parent.verticalCenter
                    width: handle.width + 1
                    height: handle.height
                    cursorShape: Qt.SplitHCursor

                    drag.target: handle
                    drag.threshold: 0
                    drag.minimumX: 1
                    drag.axis: Drag.XAxis
                }

                onXChanged: {
                    var tmp = columnWidths
                    tmp[index] = x
                    columnWidths = tmp
                }
            }
        }
    }

    property var __initialColumnWidths: []
    property real  __previousAvailableWidth: 0
}

