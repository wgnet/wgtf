import QtQuick 2.4
import QtQuick.Controls 1.2

Row {
    id: columns
    objectName: "WGDataModelHeaderRow"

    height: minimumRowHeight + topMargin + bottomMargin

    property real topMargin: 0
    property real bottomMargin: 0
    property int columnCount: 0
    property var columnWidthFunction: null
    property color backgroundColour: "transparent"
    property Component columnDelegate: null
    property var model: null
    property real minimumRowHeight: 0

    function dataChanged(fromColumn, toColumn)
    {
        toColumn = Math.min(columnRepeater.children.length - 1, toColumn);

        for (var i = fromColumn; i <= toColumn; ++i)
        {
            var child = columnRepeater.children[i];

            if (child !== null)
            {
                child.dataChanged();
            }
        }
    }

    Component.onCompleted: dataChanged(0, columnCount - 1)

    Repeater {
        id: columnRepeater
        model: columnCount

        Rectangle {
            id: columnDelegate
            y: topMargin
            height: columns.height - topMargin - bottomMargin
            width: columnWidthFunction(index);
            color: backgroundColour

            signal dataChanged;

            Loader {
                sourceComponent: columns.columnDelegate
                anchors.fill: parent

                property int columnIndex: index

                function getData(roleName)
                {
                    return columns.model.headerData(columnIndex, roleName);
                }

                onLoaded: {
                    var widthFunction = function() { return columnWidthFunction(index); }
                    width = Qt.binding(widthFunction);
                    columns.height = Math.max(height, minimumRowHeight) + topMargin + bottomMargin;
                    item.clip = true;

                    if (typeof(item.dataChanged) !== "undefined")
                    {
                        columnDelegate.dataChanged.connect(item.dataChanged);
                    }
                }
            }
        }
    }
}
