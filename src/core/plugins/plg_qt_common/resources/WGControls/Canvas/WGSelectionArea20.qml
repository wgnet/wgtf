import QtQuick 2.5

import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
*/
MouseArea
{
    id:selectionMouseArea
    WGComponent { type: "WGSelectionArea20" }
    
    anchors.fill: parent

    property point _startPoint: Qt.point(0,0)

    acceptedButtons: Qt.LeftButton

    signal selectArea(point min, point max, var mouse)
    signal previewSelectArea(point min, point max, var mouse)

    onPressed:{
        _startPoint.x = draggedArea.x = mouse.x
        _startPoint.y = draggedArea.y = mouse.y
        draggedArea.width = draggedArea.height = 0
    }

    onPositionChanged:
    {
        draggedArea.x = Math.min(_startPoint.x, mouse.x)
        draggedArea.y = Math.min(_startPoint.y, mouse.y)
        draggedArea.width = Math.abs(_startPoint.x - mouse.x)
        draggedArea.height = Math.abs(_startPoint.y - mouse.y)
        var minPoint = Qt.point(draggedArea.x, draggedArea.y)
        var maxPoint = Qt.point(draggedArea.x + draggedArea.width, draggedArea.y + draggedArea.height)
        previewSelectArea(minPoint, maxPoint, mouse)
    }

    onReleased: {
        var minPoint = Qt.point(draggedArea.x, draggedArea.y)
        var maxPoint = Qt.point(draggedArea.x + draggedArea.width, draggedArea.y + draggedArea.height)
        selectArea(minPoint, maxPoint, mouse)
        draggedArea.width = draggedArea.height = 0
    }

    Rectangle
    {
        id: draggedArea
        border.width: 1
        border.color: palette.highlightShade
        color: Qt.rgba(palette.highlightColor.r,palette.highlightColor.g,palette.highlightColor.b,0.25)
    }
}
