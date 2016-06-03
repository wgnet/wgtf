import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 1.0 as WGOne
import WGControls 2.0


Rectangle {
    id: curveEditor
    property string title: "Curve Editor"
    property var layoutHints: { 'curveeditor': 1.0, 'bottom': 0.5 }

    // TODO: Enable unlocked curves
    property bool lockCurves: true

    property var selection: [];

    property bool showColorSlider: lockCurves && (curveRepeater.count == 3 || curveRepeater.count == 4)
    property bool alphaEnabled: true

    Layout.fillHeight: true
    color: palette.mainWindowColor

    onFocusChanged: {
        if ( curveEditor.focus === true )
        {
            timeline.requestPaint();
            repaintCurves();
        }
    }

    function addPointsToCurves(mouse)
    {
        var pos = timeline.viewTransform.inverseTransform(Qt.point(mouse.x, mouse.y))
        pos.x = Math.max(pos.x, 0)
        pos.x = Math.min(pos.x, 1)


        beginUndoFrame();
        var curveIt = iterator(curves)
        while(curveIt.moveNext()){
            curveIt.current.addAt( pos.x, true )
        }
        endUndoFrame();
    }

    function clearSelection()
    {
        for(var index = 0; index < curveRepeater.count; ++index)
        {
            var currentCurve = curveRepeater.itemAt(index);
            for(var i = 0; i < currentCurve.pointRepeater.count; ++i)
            {
                currentCurve.pointRepeater.itemAt(i).selected = false;
            }
        }
    }

    function curveRemoved(item)
    {
        var newSelection = []
        for(var i = 0; i < selection.length; ++i)
        {
            var point = selection[i]
            if(point.parentCurve != item && point.selected)
            {
                newSelection.push(point)
            }
        }
        selection = newSelection
    }

    function deletePointsAt(valuesToDelete)
    {
        if(valuesToDelete.length > 0)
        {
            // Can't group multi-selection deletions because commands get consolidated
            //beginUndoFrame();
            for(var i = 0; i < valuesToDelete.length; ++i)
            {
                var curveIt = iterator(curves)
                // TODO: Remove this grouping when methods no longer get grouped for undo
                beginUndoFrame();
                while(curveIt.moveNext()){
                    curveIt.current.removeAt( valuesToDelete[i], true );
                }
                endUndoFrame();
            }
            // endUndoFrame();
        }
    }

    function deleteSelected()
    {
        var curveIt = iterator(curves)
        var index = 0;
        var valuesToDelete = []
        while(curveIt.moveNext()){
            var currentCurve = curveRepeater.itemAt(index++);
            for(var i = 0; i < currentCurve.pointRepeater.count; ++i){
                var point = currentCurve.pointRepeater.itemAt(i);
                if(point.selected){
                    point.selected = false;
                    var valueIndex = binarySearch(valuesToDelete, point.point.pos.x)
                    if(valueIndex < 0)
                    {
                        valuesToDelete.splice(-valueIndex -1, 0, point.point.pos.x);
                    }
                }
            }
        }
        deletePointsAt(valuesToDelete);
    }

    function getColorAt(index)
    {
        return Qt.rgba(
            curveRepeater.itemAt(0).getPoint(index).pos.y,
            curveRepeater.itemAt(1).getPoint(index).pos.y,
            curveRepeater.itemAt(2).getPoint(index).pos.y,
            ((curveRepeater.count == 4) ? curveRepeater.itemAt(3).getPoint(index).pos.y : 1))
    }

    function getPositionAt(index)
    {
        return curveRepeater.itemAt(0).getPoint(index).pos.x
    }

    function pointSelectionChanged(point)
    {
        var newSelection = []
        for(var i = 0; i < selection.length; ++i)
        {
            if(selection[i].selected)
                newSelection.push(selection[i])
        }
        // Point is being selected, add it to our collection
        if(point.selected)
        {
            newSelection.push(point)
        }
        selection = newSelection;
    }

    function binarySearch(ar, el) {
        var m = 0;
        var n = ar.length - 1;
        while (m <= n) {
            var k = (n + m) >> 1;
            var cmp = el - ar[k];
            if (cmp > 0) {
                m = k + 1;
            } else if(cmp < 0) {
                n = k - 1;
            } else {
                return k;
            }
        }
        return -m - 1;
    }

    function updateLockedCurves(point)
    {
        if(lockCurves)
        {
            // Move the points that share the same index on the other curves
            for(var i = 0; i < curveRepeater.count; ++i)
            {
                var otherPoint = curveRepeater.itemAt(i).pointRepeater.itemAt(point.pointIndex)
                // Ignore the point that caused this modification
                // Also ignore selected points which we will modify later
                if(otherPoint !== point && !otherPoint.selected)
                {
                    otherPoint.setPosition(point.point.pos.x, otherPoint.point.pos.y)
                }
            }
        }
    }

    function pointPositionChanged(point, xDelta, yDelta)
    {
        var modifiedIndexes = [point.pointIndex]

        updateLockedCurves(point)

        // Shift the other selected points by the delta
        for(var i = 0; i < selection.length; ++i)
        {
            var selectedPoint = selection[i]
            if(selectedPoint !== point)
            {
                var newX = selectedPoint.point.pos.x + xDelta;
                var newY = selectedPoint.point.pos.y + yDelta;

                var index = binarySearch(modifiedIndexes, selectedPoint.pointIndex)
                if (index < 0)
                {
                    modifiedIndexes.splice(-index - 1, 0, selectedPoint.pointIndex)
                }

                selectedPoint.setPosition(newX, newY);
                updateLockedCurves(selectedPoint)
            }
        }

        // Update all the necessary handles.
        if (showColorSlider)
        {
            var gradHandleValues = []
            var gradHandleColors = []

            for(var i = 0; i < modifiedIndexes.length; ++i)
            {
                var pointIndex = modifiedIndexes[i]
                var newX = getPositionAt(pointIndex)
                var color = getColorAt(pointIndex)
                gradHandleValues.push(newX)
                gradHandleColors.push(color)
            }
            colorGradient.setHandleValue(gradHandleValues, modifiedIndexes);
            colorGradient.setHandleColor(gradHandleColors, modifiedIndexes);
        }

        repaintCurves()
    }

    function repaintCurves() {
        for(var i = 0; i < curveRepeater.count; ++i) {
            curveRepeater.itemAt(i).requestPaint()
        }
    }

    function toggleCurve(index) {
        var curve = curveRepeater.itemAt(index)
        if(curve) {
            curve.enabled = !curve.enabled;
            curve.requestPaint()
        }
    }

    function curveEnabled(index) {
        var curve = curveRepeater.itemAt(index)
        return curveRepeater.count > index && curve && curve.enabled;
    }

    ColumnLayout{
        id: contents
        spacing: 2
        anchors.fill: parent

        CurveEditorToolbar {
            id: toolbar

            title: subTitle
            time:  selection.length > 0 ? selection[selection.length-1].point.pos.x : 0
            value:  selection.length > 0 ? selection[selection.length-1].point.pos.y : 0
            timeScale: xScale
            valueScale: yScale
            editEnabled: selection.length > 0
            timeScaleEnabled: timeScaleEditEnabled

            onToggleX: toggleCurve(0)
            onToggleY: toggleCurve(1)
            onToggleZ: toggleCurve(2)
            onToggleW: toggleCurve(3)
            onTimeScaleChanged: xScale = timeScale;
            onValueScaleChanged: yScale = valueScale;

            onUnscaledTimeChanged:
            {
                if(selection.length > 0)
                {
                    var point = selection[selection.length-1];
                    point.setPosition(time, point.point.pos.y)
                }
            }

            onUnscaledValueChanged:
            {
                if(selection.length > 0)
                {
                    var point = selection[selection.length-1];
                    point.setPosition(point.point.pos.x, value)
                }
            }
        }

        WGGridCanvas {
            id:timeline
            focus: true
            timeScale: xScale
            valueScale: yScale
            Layout.fillHeight: true
            Layout.fillWidth: true

            // Zoom to the extents of the curve, always zooms the full X axis and zooms to the available y extremes
            function zoomExtents()
            {
                // Calculate the y extremes
                var yMin = undefined;
                var yMax = undefined;
                for(var index = 0; index < curveRepeater.count; ++index){
                    var currentCurve = curveRepeater.itemAt(index);
                    for(var i = 0; i < currentCurve.pointRepeater.count; ++i){
                        var point = currentCurve.pointRepeater.itemAt(i);
                        if(yMin === undefined || point.point.pos.y < yMin){
                            yMin = point.point.pos.y;
                        }
                        if(yMax === undefined || point.point.pos.y > yMax){
                            yMax = point.point.pos.y;
                        }
                    }
                }

                // If there were no points on any curves zoom to the full y extents
                if(yMin === undefined)
                {
                    yMin = 0;
                    yMax = 1;
                }

                // If the y extremes are the same make sure we have something to zoom to centering the points
                if(yMin === yMax)
                {
                    yMax += 0.5;
                    yMin -= 0.5;
                }

                var topMargin = .1
                var botMargin = .9
                var leftMargin = .05
                var rightMargin = .9

                // Here we solve for the yScale and origin.y necessary to zoom to our extents.
                // We want to have our yMax/yMin values a comfortable distance from the top/bottom of the timeline
                // This gives us two equations and two unknowns (yScale, origin.y)
                // yMax * yScale + origin.y = timeline.height * topMargin
                // yMin * yScale + origin.y = timeline.height * botMargin
                // Solve for origin.y in terms of yScale
                //  origin.y = (timeline.height * botMargin) - (yMin * yScale)
                // Now solve for yScale
                //  yMax * yScale = (timeline.height * topMargin) - origin.y
                //  yScale = ((timeline.height * topMargin) - origin.y) / yMax
                // Substitute origin.y
                //  yScale = ((timeline.height * topMargin) - ((timeline.height * botMargin) - (yMin * yScale))) / yMax
                //  yScale = ((timeline.height * topMargin) - (timeline.height * botMargin) + (yMin * yScale)) / yMax
                //  yScale = (timeline.height * topMargin)/yMax - (timeline.height * botMargin)/yMax + (yMin * yScale)/yMax
                //  yScale - (yMin * yScale)/yMax = (timeline.height/yMax) * (topMargin - botMargin)
                //  yScale * (1 - yMin/yMax) = (timeline.height/yMax) * (topMargin - botMargin)
                //  yScale = ((timeline.height/yMax) * (topMargin - botMargin)) / (1 - yMin/yMax)

                // Because we divide by yMax it must not be zero
                if(yMax === 0)
                    yMax = 0.001
                var yViewScale = ((topMargin - botMargin)*timeline.height/yMax) / (1 - yMin/yMax)
                // Use a margin so xMin/xMax are a comfortable distance from the left/right of the timeline
                timeline.viewTransform.origin.x = timeline.width * leftMargin;
                timeline.viewTransform.origin.y = (timeline.height * botMargin) - (yMin * yViewScale)
                timeline.viewTransform.xScale = timeline.width * rightMargin;
                timeline.viewTransform.yScale = yViewScale;
                timeline.requestPaint();
            }

            Keys.onPressed:{
                if(event.key === Qt.Key_E && event.modifiers & Qt.ControlModifier){
                    timeline.zoomExtents();
                }
                else if(event.key === Qt.Key_Delete)
                {
                    curveEditor.deleteSelected();
                }
                else if(event.key === Qt.Key_X)
                {
                    toggleCurve(0)
                }
                else if(event.key === Qt.Key_Y)
                {
                    toggleCurve(1)
                }
                else if(event.key === Qt.Key_Z)
                {
                    toggleCurve(2)
                }
                else if(event.key === Qt.Key_W)
                {
                    toggleCurve(3)
                }
            }

            // Data model coming from C++
            WGOne.WGListModel
            {
                id: curvesModel
                source: curves
                WGOne.ValueExtension {}
            }

            WGSelectionArea
            {
                onSelectArea:{
                    min = timeline.viewTransform.inverseTransform(min)
                    max = timeline.viewTransform.inverseTransform(max)
                    var addToSelection = (mouse.modifiers & Qt.ControlModifier)
                    for(var index = 0; index < curveRepeater.count; ++index)
                    {
                        var currentCurve = curveRepeater.itemAt(index);
                        if(!currentCurve.enabled)
                            continue;
                        for(var i = 0; i < currentCurve.pointRepeater.count; ++i)
                        {
                            var point = currentCurve.pointRepeater.itemAt(i)
                            var pos = point.point.pos
                            // Invert y to account for window coordinates
                            var contained = pos.x >= min.x && pos.x <= max.x && pos.y <= min.y && pos.y >= max.y;
                            if(contained)
                                point.selected = true;
                            else if(!addToSelection)
                                point.selected = false;
                        }
                    }
                }
            }

            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.AllButtons
                onPressed:{
                    // Focus the timeline for keyboard shortcuts
                    timeline.forceActiveFocus(Qt.MouseFocusReason);
                    mouse.accepted = false;

                    if(mouse.button == Qt.LeftButton)
                    {
                        if(mouse.modifiers & Qt.AltModifier)
                        {
                            addPointsToCurves(mouse)
                            mouse.accepted = true;
                        }
                        else if(!(mouse.modifiers & Qt.ControlModifier))
                        {
                            clearSelection()
                        }
                    }
                }
            }

            Repeater
            {
                id: curveRepeater
                model: curvesModel

                onCountChanged: colorGradient.syncHandles()

                delegate: Curve{
                    objectName: index
                    curveIndex: index
                    points: value.points
                    curveModel: value
                    viewTransform: timeline.viewTransform;
                    Component.onCompleted:{
                        // Assign and don't bind the color, otherwise we get qml errors when the curvesModel changes
                        // Should we update colors when the curvesModel Changes?
                        color = ["#b45b4e", "#99dc74", "#92cfdd", "#808080"][index%4]
                    }
                    onPointSelectionChanged:{
                        curveEditor.pointSelectionChanged(point);
                    }
                    onPointPositionChanged:{
                        curveEditor.pointPositionChanged(point, xDelta, yDelta)
                    }
                    onPointPressed:{
                        if(point.selected === false && mouse.modifiers !== Qt.ControlModifier)
                        {
                            clearSelection();
                        }
                    }
                    onPointAdded:{
                        colorGradient.syncHandles()
                    }
                    onPointRemoved:{
                        colorGradient.syncHandles()
                    }
                    onPointClicked:{
                        if(mouse.modifiers !== Qt.ControlModifier)
                        {
                            clearSelection();
                        }
                    }
                    onPointUpdated:{
                        updateGradientTimer.restart()
                    }
                }

                onItemRemoved:
                {
                    curveEditor.curveRemoved(item);
                }
            }

            CurveEditorContextMenu {}

            // Commenting this out until multiple gradient stops work correctly
            // Also need signals to enable handling changes to update the curve data
            WGGradientSlider {
                id: colorGradient
                visible: showColorSlider
                anchors.bottom: timeline.bottom
                x: timeline.viewTransform.transformX(0)
                width: timeline.viewTransform.transformX(1) - timeline.viewTransform.transformX(0)
                height: defaultSpacing.minimumRowHeight
                minimumValue: 0
                maximumValue: 1.0
                stepSize: .001

                onChangeValue: {
                    if(!Qt._updatingPosition && !Qt._updatingCurveGradient)
                    {
                        var red = curveRepeater.itemAt(0).getPoint(index);
                        var green = curveRepeater.itemAt(1).getPoint(index);
                        var blue = curveRepeater.itemAt(2).getPoint(index);
                        var alpha = curveRepeater.count == 4 ?
                                    curveRepeater.itemAt(3).getPoint(index) : null
                        beginUndoFrame()
                        red.pos.x = val
                        green.pos.x = val
                        blue.pos.x = val
                        if(alpha){
                            alpha.pos.x = val
                        }
                        endUndoFrame()
                        repaintCurves()
                    }
                }

                onColorModified: {
                    if(!Qt._updatingCurveGradient)
                    {
                        beginUndoFrame()
                        var red = curveRepeater.itemAt(0).getPoint(index);
                        var green = curveRepeater.itemAt(1).getPoint(index);
                        var blue = curveRepeater.itemAt(2).getPoint(index);
                        var alpha = curveRepeater.count == 4 ?
                                    curveRepeater.itemAt(3).getPoint(index) : null

                        red.pos.y = color.r
                        green.pos.y = color.g
                        blue.pos.y = color.b
                        if(alpha){
                            alpha.pos.y = color.a
                        }
                        endUndoFrame()
                        repaintCurves()
                    }
                }

                onHandleAdded: {
                    console.assert(curveRepeater.count > 0)
                    if(!Qt._updatingCurveGradient)
                    {
                        // Prevent adding new points if the addition of the point created this handle
                        if(colorGradient.__handleCount === curveRepeater.itemAt(0).pointRepeater.count)
                            return
                        var color = colorGradient.getHandleColor(index)
                        var relPos = colorGradient.getHandleValue(index)
                        var mousePos = timeline.viewTransform.transformX(relPos)
                        beginUndoFrame()
                        curveEditor.addPointsToCurves(Qt.point(mousePos,0))
                        var red = curveRepeater.itemAt(0).getPoint(index);
                        var green = curveRepeater.itemAt(1).getPoint(index);
                        var blue = curveRepeater.itemAt(2).getPoint(index);
                        var alpha = curveRepeater.count == 4 ?
                                    curveRepeater.itemAt(3).getPoint(index) : null

                        red.pos.y = color.r
                        green.pos.y = color.g
                        blue.pos.y = color.b
                        if(alpha){
                            alpha.pos.y = color.a
                        }
                        endUndoFrame()
                    }
                }

                onHandleRemoved: {
                    console.assert(curveRepeater.count > 0)
                    if(!Qt._updatingCurveGradient)
                    {
                        var red = curveRepeater.itemAt(0).getPoint(index);
                        curveEditor.deletePointsAt([red.pos.x]);
                    }
                }

                function syncHandles()
                {
                    if(!visible)
                        return
                    // Ensure all curves have equal points
                    var curveIndex = curveRepeater.count - 1
                    var pointCount = curveRepeater.itemAt(curveIndex).pointRepeater.count
                    while(--curveIndex >= 0)
                    {
                        if(pointCount !== curveRepeater.itemAt(curveIndex).pointRepeater.count)
                        {
                            return
                        }
                    }

                    if(!Qt._updatingCurveGradient)
                    {
                        Qt._updatingCurveGradient = true

                        // Remove surplus handles
                        while(__handleCount > pointCount)
                        {
                            colorGradient.removeHandle(__handleCount-1)
                        }

                        // Update existing handles
                        var indexes = []
                        var gradHandleValues = []
                        var gradHandleColors = []
                        for(var i = 0; i < __handleCount; ++i)
                        {
                            indexes.push(i)
                            gradHandleValues.push(curveEditor.getPositionAt(i))
                            gradHandleColors.push(curveEditor.getColorAt(i))
                        }

                        colorGradient.setHandleValue(gradHandleValues, indexes);
                        colorGradient.setHandleColor(gradHandleColors, indexes);

                        // Create handles for all missing values
                        while(__handleCount < pointCount)
                        {
                            var index = __handleCount
                            var point = curveRepeater.itemAt(curveRepeater.count - 1).pointRepeater.itemAt(index)

                            var newColor = curveEditor.getColorAt(index)

                            colorGradient.createColorHandle(point.point.pos.x, handleStyle, __handlePosList.length, newColor)
                        }
                        Qt._updatingCurveGradient = false
                    }
                }
            }
        }
    }

    // Support Undo/Redo and update the gradient slider when the points are updated
    // Only run on update once pooling all requests into a single update
    Timer
    {
        id: updateGradientTimer
        interval: 1
        onTriggered: { colorGradient.syncHandles() }
    }
}
