import QtQuick 2.0

/*!
 \brief WGColumnsFrame is used as a frame for listView and treeView columns
*/


Item {
    id: columnsFrame
    objectName: "WGColumnsFrame"

    property int columnCount: 0
    property real firstColumnIndentation: 0
    property real handleWidth: 2
    property var initialColumnWidths: []
    property real defaultInitialColumnWidth: 100
    property real dragWidthIncrease: 2
    property bool resizable: true
    property bool resizableColumns: true
    property bool drawHandles: true
    property real minimumColumnSize: 0
    property real maximumColumnSize: 10000
    property var idealColumnSizeFunction: null

    signal columnsChanged(var columnWidths);

    function resizeColumnToIdealSize(index)
    {
        if (idealColumnSizeFunction === null)
        {
            return;
        }

        if (columns.children.length < 2)
        {
            return;
        }

        columns.fixHandles();
        var column = columns.children[index];
        var handle = column.children[0];
        var minimumSize = (index === 0 ? firstColumnIndentation + minimumColumnSize : minimumColumnSize);
        handle.x = Math.max(minimumSize, idealColumnSizeFunction(index));
    }

    visible: columnCount > 0

    Row {
        id: columns
        objectName: "columns"
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: 0

        function fixHandles()
        {
            var column = null;

            for (var i = 0; i < columns.children.length; ++i)
            {
                column = columns.children[i];
                var handle = column.children[0];
                if (typeof(handle) !== "undefined") 
                {
                    handle.x = handle.x;
                }
            }
        }

        function updateWidths()
        {
            var columnWidths = [];
            var column = null;
            var columnWidth = 0;
            var totalWidth = 0;

            for (var i = 0; i < columns.children.length; ++i)
            {
                column = columns.children[i];
                columnWidth = column.width - handleWidth;
                columnWidths.push(columnWidth);
                totalWidth += columnWidth + handleWidth;
            }

            width = totalWidth;

            if (resizable)
            {
                columnsFrame.width = width;
            }

            columnsChanged(columnWidths);
        }

        function calculateMaxColumnSize(x, index)
        {
            var maxSize = width - x;

            for (var i = index + 1; i < columnWidths.length; ++i)
            {
                maxSize -= (handleWidth + columnWidths[i]);
            }

            return maxSize;
        }

        Repeater {
            id: columnSeries
            model: columnCount
            anchors.fill: parent

            Rectangle {
                id: column
                color: "transparent"
                height: columns.height
                width: handle.x + handle.width
                onWidthChanged: columns.updateWidths()

                Rectangle {
                    id: handle
                    objectName: "columnFrameHandle"
                    property real initialWidth: initialColumnWidths.length > index ? initialColumnWidths[index] : defaultInitialColumnWidth
                    x: initialWidth
                    width: handleWidth
                    y: 0
                    height: parent.height
                    color: drawHandles ? palette.darkColor : "transparent"

                    MouseArea {
                        id: columnHandleMouseArea
                        enabled: resizableColumns
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width + dragWidthIncrease
                        height: parent.height
                        cursorShape: drawHandles ? Qt.SplitHCursor : Qt.ArrowCursor

                        drag.target: handle
                        drag.threshold: 0
                        drag.axis: Drag.XAxis
                        drag.minimumX: index === 0 ? firstColumnIndentation + minimumColumnSize : minimumColumnSize
                        drag.maximumX: resizable ? maximumColumnSize : columns.calculateMaxColumnSize(column.x, index)
                        
                        onPositionChanged: {
                            columns.fixHandles();
                        }
                        
                        onDoubleClicked: {
                            if (idealColumnSizeFunction !== null)
                            {
                                columns.fixHandles();
                                parent.x = Math.max(drag.minimumX, idealColumnSizeFunction(index));
                            }
                        }
                    }

                    Rectangle {
                        id: innerShade
                        color: drawHandles ? palette.mainWindowColor : "transparent"
                        visible: drawHandles && width > 0
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.left: handle.left
                        anchors.right: rightSideShade.right
                        anchors.leftMargin: 1
                        anchors.rightMargin: 1
                    }

                    Rectangle {
                        id: rightSideShade
                        color: drawHandles ? palette.midLightColor : "transparent"
                        visible: drawHandles && handleWidth > 1
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        width: 1
                    }
                }
            }
        }
    }
}

