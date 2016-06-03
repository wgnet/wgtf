import QtQuick 2.0
import WGControls 1.0

Canvas
{
    id : connectorsLayer

    property color curveColor
    property var connectors

    function drawConnector(ctx, startPoint, endPoint)
    {
        ctx.lineWidth = 6.0
        ctx.lineCap = "round"
        ctx.strokeStyle = Qt.rgba(curveColor.r, curveColor.g, curveColor.b, 0.3)

        ctx.beginPath()
        ctx.moveTo(_startPoint.x, _startPoint.y)
        ctx.bezierCurveTo((_endPoint.x + _startPoint.x)/2,_startPoint.y,(_endPoint.x + _startPoint.x)/2,_endPoint.y,_endPoint.x,_endPoint.y)
        ctx.stroke();

        ctx.lineWidth = 2.0
        ctx.lineCap = "round"
        ctx.strokeStyle = curveColor

        ctx.beginPath()
        ctx.moveTo(_startPoint.x, _startPoint.y)
        ctx.bezierCurveTo((_endPoint.x + _startPoint.x)/2,_startPoint.y,(_endPoint.x + _startPoint.x)/2,_endPoint.y,_endPoint.x,_endPoint.y)
        ctx.stroke();
    }

    onPaint:
    {
        var ctx = getContext('2d');
        ctx.clearRect(0, 0, width, height);

        var connectorsIt = iterator(connectors)
        if(connectorsIt.moveNext())
        {
            do
            {
                var connector = connectorsIt.current
                var outputSlotPt = slotsIndex[connector.outputSlot].getGlobalCenter()
                var inputSlotPt = slotsIndex[connector.inputSlot].getGlobalCenter()
                drawConnector(ctx, outputSlotPt, inputSlotPt)

            } while(connectorsIt.moveNext())
        }
    }
}
