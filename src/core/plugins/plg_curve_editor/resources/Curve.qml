import QtQuick 2.0
import WGControls 1.0 as WGOne
import WGControls 2.0

Canvas {
    id: curve
    anchors.fill: parent
    // Bezier Point Data has 3 points, the position and two control points relative to the position
    // point{
    //      point pos;
    //      point cp1;
    //      point cp2;
    // }
    property alias pointRepeater: pointRepeater;
    property var points;
    property var curveModel;
    property var curveIndex;
    property color color;
    property bool enabled: true;
    property bool showControlPoints: enabled && curveModel.showControlPoints
    property var _scaleX: viewTransform.xScale;
    property var _scaleY: viewTransform.yScale;
    property var _originX: viewTransform.origin.x;
    property var _originY: viewTransform.origin.y;
    property var viewTransform: WGViewTransform{
        container: curve
    }

    signal pointSelectionChanged(Point point)
    signal pointPositionChanged(Point point, real xDelta, real yDelta)
    signal pointPressed(Point point, var mouse)
    signal pointReleased(Point point, var mouse)
    signal pointClicked(Point point, var mouse)
    signal pointAdded(int index, var point)
    signal pointRemoved(int index, var point)
    signal pointUpdated(Point point)

    on_OriginXChanged: { requestPaint() }
    on_OriginYChanged: { requestPaint() }
    on_ScaleXChanged: { requestPaint() }
    on_ScaleYChanged: { requestPaint() }

    onEnabledChanged: {
        if(!enabled)
        {
            for(var i = 0; i < pointRepeater.count; ++i){
                pointRepeater.itemAt(i).selected = false;
            }
        }
    }

    function addPoint(index, point)
    {
        if(index > 0)
        {
            var prevPoint = curve.pointRepeater.itemAt(index - 1);
            prevPoint.nextPoint = point.point;
            point.prevPoint = prevPoint.point;
            prevPoint.updateHandlePositions();
        }

        if(index + 1 < curve.pointRepeater.count)
        {
            var nextPoint = curve.pointRepeater.itemAt(index + 1);
            if(nextPoint)
            {
                point.nextPoint = nextPoint.point;
                nextPoint.prevPoint = point.point;
                nextPoint.updateHandlePositions();
            }
        }
        point.updateHandlePositions();

        curve.pointAdded(index, point)

        curve.requestPaint()
    }

    function removePoint(index, point)
    {
        if(index > 0)
        {
            var prevPoint = curve.pointRepeater.itemAt(index - 1);
            prevPoint.nextPoint = point.nextPoint;
            prevPoint.updateHandlePositions();
        }

        if(index < curve.pointRepeater.count)
        {
            var nextPoint = curve.pointRepeater.itemAt(index);
            nextPoint.prevPoint = point.prevPoint;
            nextPoint.updateHandlePositions();
        }

        curve.pointRemoved(index, point)
        curve.requestPaint()
    }

    function numPoints()
    {
        var pointIt = iterator(curve.points)
        var count = 0;
        while(pointIt.moveNext()){++count;}
        return count;
    }

    function getPoint(index)
    {
        if(index === -1 || index >= pointRepeater.count)
            return null;
        return pointRepeater.itemAt(index).point
    }

    function constrainHandles(pointIndex)
    {
        if(pointIndex > 0)
        {
            pointRepeater.itemAt(pointIndex-1).constrainHandles()
        }
        pointRepeater.itemAt(pointIndex).constrainHandles()
        if((pointIndex + 1) < pointRepeater.count)
        {
            pointRepeater.itemAt(pointIndex+1).constrainHandles()
        }
    }

    WGOne.WGListModel
    {
        id: pointModel
        source: points

        WGOne.ValueExtension {}
    }

    Repeater
    {
        id: pointRepeater
        model: pointModel
        delegate: Point{
            objectName: index
            point: value;
            parentCurve: curve;
            baseColor: curve.color;
            enabled: curve.enabled;
            viewTransform: curve.viewTransform;
            pointIndex: index
            onSelectedChanged:{
                pointSelectionChanged(this)
            }
            onPositionChanged: pointPositionChanged(point, xDelta, yDelta)
            onPressed: pointPressed(point, mouse)
            onReleased: pointReleased(point, mouse)
            onClicked: pointClicked(point, mouse)
            onUpdated: pointUpdated(point)
        }
        onItemAdded:
        {
            curve.addPoint(index, item)
        }
        onItemRemoved:
        {
            item.selected = false;
            curve.removePoint(index, item)
        }
    }

    onPaint: {
        var ctx = getContext('2d');
        ctx.clearRect(0, 0, width, height);

        var curveIt = iterator(curve.points)
        if(curveIt.moveNext()){
            // -- Glow line
            ctx.lineWidth = 4.0;
            ctx.strokeStyle = Qt.rgba(color.r, color.g, color.b, 0.3)
            //ctx.strokeStyle = enabled ? color : Qt.darker(strokeStyle);
            ctx.clearRect(0,0,width,height)

            ctx.beginPath();
            var point = curveIt.current;
            var c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
            c1 = viewTransform.transform(c1);
            var pos = viewTransform.transform(point.pos);
            ctx.moveTo(viewTransform.transformX(0), pos.y);
            ctx.lineTo(pos.x, pos.y);
            var c2;
            do{
                point = curveIt.current;
                pos = viewTransform.transform(point.pos);
                c2 = Qt.point(point.pos.x + point.cp1.x, point.pos.y + point.cp1.y);
                c2 = viewTransform.transform(c2);

                ctx.bezierCurveTo(c1.x, c1.y, c2.x, c2.y, pos.x, pos.y);

                c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
                c1 = viewTransform.transform(c1);
            } while(curveIt.moveNext())
            ctx.lineTo(viewTransform.transformX(1), pos.y);
            ctx.stroke();
        }
        var curveIt = iterator(curve.points)
        // Until we can get the length property we iterate the collection to count
        var count = 0;
        if(curveIt.moveNext()){
            // -- Solid line
            ctx.lineWidth = 1.0;
            ctx.strokeStyle = enabled ? color : Qt.darker(color);

            ctx.beginPath();
            var point = curveIt.current;
            var c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
            c1 = viewTransform.transform(c1);
            var pos = viewTransform.transform(point.pos);
            ctx.moveTo(viewTransform.transformX(0), pos.y);
            ctx.lineTo(pos.x, pos.y);
            do{
                point = curveIt.current;
                pos = viewTransform.transform(point.pos);
                var c2 = Qt.point(point.pos.x + point.cp1.x, point.pos.y + point.cp1.y);
                c2 = viewTransform.transform(c2);

                ctx.bezierCurveTo(c1.x, c1.y, c2.x, c2.y, pos.x, pos.y);

                c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
                c1 = viewTransform.transform(c1);
                ++count;
            } while(curveIt.moveNext())
            ctx.lineTo(viewTransform.transformX(1), pos.y);
            ctx.stroke();
        }
    }

    WGDataChangeNotifier
    {
        source: curveModel.notifyDirty
        onDataChanged:
        {
            // HACK: force the points to re-evaluate their positions by emitting the viewTransform changed signal
            viewTransformChanged();
        }
    }
}
