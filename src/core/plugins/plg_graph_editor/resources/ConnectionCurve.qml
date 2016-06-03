import QtQuick 2.0

Canvas
{
    anchors.fill: parent
    antialiasing : true

    property real _xScale
    property real _yScale
    property var _origin

    property var startPoint
    property var endPoint
    property color curveColor

    property bool isInteractive : true
    property var outputSlot
    property var inputSlot

    //onOutputSlotChanged: { requestPaint() }
    //onInputSlotChanged: { requestPaint() }

    onPaint:
    {
        var ctx = getContext('2d');
        ctx.clearRect(0, 0, width, height);

        var _startPoint = isInteractive ? startPoint : outputSlot.getGlobalCenter();
        var _endPoint = isInteractive ? endPoint : inputSlot.getGlobalCenter();

        if (startPoint == null || endPoint == null)
            return;

        ctx.lineWidth = 6.0
        ctx.lineCap = "round"
        ctx.strokeStyle = Qt.rgba(curveColor.r, curveColor.g, curveColor.b, 0.3)

        ctx.beginPath()
        ctx.moveTo(_startPoint.x, _startPoint.y)
        ctx.bezierCurveTo((_endPoint.x + _startPoint.x)/2,_startPoint.y,(_endPoint.x + _startPoint.x)/2,_endPoint.y,_endPoint.x,_endPoint.y)
        ctx.stroke();

        // -- Solid line
        ctx.lineWidth = 2.0
        ctx.lineCap = "round"
        ctx.strokeStyle = curveColor

        ctx.beginPath()
        ctx.moveTo(_startPoint.x, _startPoint.y)
        ctx.bezierCurveTo((_endPoint.x + _startPoint.x)/2,_startPoint.y,(_endPoint.x + _startPoint.x)/2,_endPoint.y,_endPoint.x,_endPoint.y)
        ctx.stroke();
    }
}
