import QtQuick 2.0
import WGControls 2.0

Rectangle {
    id: handle
    visible: enabled;
    width: 7;
    height: width
    color: baseColor
    state: "unselected"

    x: viewTransform.transformX(handle.point.pos.x);
    y: viewTransform.transformY(handle.point.pos.y);

    onXChanged: { parentCurve.requestPaint(); updated(handle) }
    onYChanged: { parentCurve.requestPaint(); updated(handle) }

    property var _scaleX: viewTransform.xScale;
    property var _scaleY: viewTransform.yScale;
    property var _originX: viewTransform.origin.x;
    property var _originY: viewTransform.origin.y;
    property var _prevX;
    property var _prevY;
    property var _nextX;
    property var _nextY;
    property var parentCurve;
    property var prevPoint;
    property var nextPoint;
    property var point;
    property var pointIndex;
    property bool enabled;
    property color baseColor;
    property string prevState;
    property bool selected: false;
    property bool cp1Enabled: prevPoint !== undefined && parentCurve.showControlPoints;
    property bool cp2Enabled: nextPoint !== undefined && parentCurve.showControlPoints;
    property real minDistance: .001
    property var viewTransform: WGViewTransform{
        container: parent
    }

    signal positionChanged(var point, real xDelta, real yDelta)
    signal clicked(var point, var mouse)
    signal pressed(var point, var mouse)
    signal released(var point, var mouse)
    signal updated(var point)

    function setPosition(x, y) {
        if(x === handle.point.pos.x && y === handle.point.pos.y)
            return;
        // Constrain to adjacent points
        if(prevPoint)
            x = Math.max(prevPoint.pos.x + minDistance, x)
        if(nextPoint)
            x = Math.min(nextPoint.pos.x - minDistance, x)
        // Constrain to the timeline
        x = Math.max(0, x)
        x = Math.min(1, x)

        var xDelta = x - handle.point.pos.x
        var yDelta = y - handle.point.pos.y
        beginUndoFrame()
        handle.point.pos.x = x;
        handle.point.pos.y = y;
        parentCurve.constrainHandles(pointIndex);
        if(!Qt._updatingPosition)
        {
            Qt._updatingPosition = true
            positionChanged(handle, xDelta, yDelta);
            Qt._updatingPosition = false
        }
        endUndoFrame()
        handle.parent.requestPaint();
    }

    on_ScaleXChanged: { updateHandlePositions() }
    on_ScaleYChanged: { updateHandlePositions() }
    on_OriginXChanged: { updateHandlePositions() }
    on_OriginYChanged: { updateHandlePositions() }

    onSelectedChanged:{
        state = selected ? 'selected' : 'unselected';
    }

    transform: Translate{
        x: -width / 2;
        y: -height / 2;
    }

    states: [
        State {
            name: "selected";
            PropertyChanges { target: handle; color: Qt.lighter(handle.baseColor); }
            PropertyChanges { target: handle; width: 10; }
            PropertyChanges { target: leftHandle; visible: handle.cp1Enabled; }
            PropertyChanges { target: rightHandle; visible: handle.cp2Enabled; }
        },
        State {
            name: "unselected";
            PropertyChanges { target: handle; color: handle.baseColor; }
            PropertyChanges { target: handle; width: 7; }
            PropertyChanges { target: leftHandle; visible: false; }
            PropertyChanges { target: rightHandle; visible: false; }
        },
        State {
            name: "hovered"
            PropertyChanges { target: handle; color: Qt.lighter(handle.baseColor); }
            PropertyChanges { target: handle; width: 10; }
        }
    ]

    function updateHandlePositions(){
        leftHandle.x = handle.point.cp1.x * viewTransform.xScale;
        leftHandle.y = handle.point.cp1.y * viewTransform.yScale;
        rightHandle.x = handle.point.cp2.x * viewTransform.xScale;
        rightHandle.y = handle.point.cp2.y * viewTransform.yScale;
    }

    function constrainHandles()
    {
        if(prevPoint){
            var constrainedX = Math.max(prevPoint.pos.x - point.pos.x, handle.point.cp1.x)
            handle.point.cp1.y = constrainedX ? handle.point.cp1.y * constrainedX / handle.point.cp1.x : 0;
            handle.point.cp1.x = constrainedX;
            leftHandle.x = handle.point.cp1.x * viewTransform.xScale;
            leftHandle.y = handle.point.cp1.y * viewTransform.yScale;
        }
        if (nextPoint) {
            var constrainedX = Math.min(nextPoint.pos.x - point.pos.x, handle.point.cp2.x)
            handle.point.cp2.y = constrainedX ? handle.point.cp2.y * constrainedX / handle.point.cp2.x : 0;
            handle.point.cp2.x = constrainedX;
            rightHandle.x = handle.point.cp2.x * viewTransform.xScale;
            rightHandle.y = handle.point.cp2.y * viewTransform.yScale;
        }
    }

    MouseArea {
        id: handleDragArea
        property bool dragging: false
        property var startPos
        anchors.fill: parent
        drag.target: handle
        drag.minimumX: prevPoint ? viewTransform.transformX(prevPoint.pos.x + minDistance) : viewTransform.transformX(0)
        drag.maximumX: nextPoint ? viewTransform.transformX(nextPoint.pos.x - minDistance) : viewTransform.transformX(1)
        drag.maximumY: viewTransform.transformY(0)
        drag.filterChildren: true
        drag.threshold: 1
        hoverEnabled: true

        function constrainAxis(mouse)
        {
            var lockAxis = (mouse.modifiers & Qt.ShiftModifier);

            if(!lockAxis)
            {
                drag.axis = Drag.XAndYAxis;
                handleDragArea.startPos = Qt.point(mouse.x, mouse.y)
            }
            else if(drag.axis == Drag.XAndYAxis)
            {
                if(Math.abs(mouse.x - handleDragArea.startPos.x) > Math.abs(mouse.y - handleDragArea.startPos.y))
                    drag.axis = Drag.XAxis;
                else
                    drag.axis = Drag.YAxis;
            }
        }

        onPositionChanged: {
            if (drag.active) {
                constrainAxis(mouse)
                if(!dragging)
                {
                    dragging = true
                    beginUndoFrame();
                }
                state = handle.selected ? "selected" : "hovered"
                var pos = viewTransform.inverseTransform(Qt.point(handle.x, handle.y))
                setPosition(pos.x, pos.y);
            }
        }
        onEntered: {
            if (handle.state === "unselected") {
                prevState = handle.state;
                handle.state = "hovered"
            }
        }
        onExited: {
            if (handle.state === "hovered" && !dragging) {
                handle.state = prevState
            }
        }
        onClicked: {
            handle.clicked(handle, mouse);
            handle.selected = !handle.selected;
        }
        onPressed: {
            handle.pressed(handle, mouse);
            prevState = handle.state;
            handleDragArea.startPos = Qt.point(mouse.x,mouse.y)
        }
        onReleased: {
            if(dragging)
            {
                handle.released(handle, mouse);
                handle.state = handle.selected ? "selected" : "unselected";
                endUndoFrame();
                dragging = false
                drag.axis = Drag.XAndYAxis;
            }
        }
    }

    Rectangle {
        id: leftTangent
        color: handle.color
        x: handle.width/2
        y: handle.height/2
        visible: leftHandle.visible
        width: Qt.vector2d(leftHandle.x, leftHandle.y).length()
        height: 1
        transform: Rotation {
            angle: Math.acos(Qt.vector2d(leftHandle.x, leftHandle.y).normalized().dotProduct(Qt.vector2d(1,0))) * 180/Math.PI * ((leftHandle.y > 0) ? 1 : -1)
        }
    }

    Rectangle {
        id: leftHandle
        width: 20;
        height: width
        color: "transparent";
        visible: false;
        border.color: Qt.rgba(handle.color.r, handle.color.g, handle.color.b, .1)
        x: handle.point.cp1.x * viewTransform.xScale;
        y: handle.point.cp1.y * viewTransform.yScale;
        transform: Translate{
            x: -width/2 - 2; // Additional 2 pixels due to snapping
            y: -height/2 - 2;
        }

        Rectangle {
            anchors.fill: parent;
            anchors.margins: 8;
            color: handle.color;
        }

        MouseArea {
            property bool dragging: false;
            anchors.fill: parent
            drag.target: leftHandle
            drag.threshold: 0
            drag.minimumX: prevPoint ?
                viewTransform.xScale*(prevPoint.pos.x - point.pos.x) : -Infinity
            drag.maximumX: 0
            onPositionChanged: {
                if (drag.active) {
                    if(!dragging)
                    {
                        dragging = true;
                        beginUndoFrame();
                    }
                    // -- Move the tangent
                    if (!rightHandle.children[1].drag.active) {
                        handle.point.cp1.x = leftHandle.x / viewTransform.xScale;
                        handle.point.cp1.y = leftHandle.y / viewTransform.yScale;
                    }

                    // -- Force the opposite tangent to the inverse position
                    if (!(mouse.modifiers & Qt.ControlModifier) && nextPoint) {
                        handle.point.cp2.x = -handle.point.cp1.x;
                        handle.point.cp2.y = -handle.point.cp1.y;
                        constrainHandles();
                    }

                    handle.parent.requestPaint();
                }
            }
            onReleased: {
                if(dragging)
                {
                    endUndoFrame();
                    dragging = false;
                }
            }
        }
    }

    Rectangle {
        id: rightTangent
        color: handle.color
        x: handle.width/2
        y: handle.height/2
        visible: rightHandle.visible
        width: Qt.vector2d(rightHandle.x, rightHandle.y).length()
        height: 1
        transform: Rotation {
            angle: Math.acos(Qt.vector2d(rightHandle.x, rightHandle.y).normalized().dotProduct(Qt.vector2d(1,0))) * 180/Math.PI * ((rightHandle.y > 0) ? 1 : -1)
        }
    }

    Rectangle {
        id: rightHandle
        width: 20;
        height: width
        color: "transparent";
        visible: false;
        border.color: Qt.rgba(handle.color.r, handle.color.g, handle.color.b, .1)
        x: handle.point.cp2.x * viewTransform.xScale;
        y: handle.point.cp2.y * viewTransform.yScale;
        transform: Translate{
            x: -width/2 - 2; // Additional 2 pixels due to snapping
            y: -height/2 - 2;
        }

        Rectangle {
            anchors.fill: parent;
            anchors.margins: 8;
            color: handle.color;
        }

        MouseArea {
            property bool dragging: false;
            anchors.fill: parent
            drag.target: rightHandle
            drag.threshold: 0
            drag.minimumX: 0
            drag.maximumX: nextPoint ?
                viewTransform.xScale*(nextPoint.pos.x - point.pos.x) : Infinity
            onPositionChanged: {
                if (drag.active) {
                    if(!dragging)
                    {
                        dragging = true;
                        beginUndoFrame();
                    }
                    // -- Move the tangent
                    if (!leftHandle.children[1].drag.active) {
                        handle.point.cp2.x = rightHandle.x / viewTransform.xScale;
                        handle.point.cp2.y = rightHandle.y / viewTransform.yScale;
                    }

                    // -- Force the opposite tangent to the inverse position
                    if (!(mouse.modifiers & Qt.ControlModifier) && prevPoint) {
                        handle.point.cp1.x = -handle.point.cp2.x;
                        handle.point.cp1.y = -handle.point.cp2.y;
                        constrainHandles();
                    }

                    handle.parent.requestPaint();
                }
            }
            onReleased: {
                if(dragging)
                {
                    endUndoFrame();
                    dragging = false;
                }
            }
        }
    }
}

