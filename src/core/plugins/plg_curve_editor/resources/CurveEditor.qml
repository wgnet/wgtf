import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Canvas 2.0


Rectangle {
    id: curveEditor
    WGComponent { type: "CurveEditor" }
    
    property string title: "Curve Editor"
    property var layoutHints: { 'curveeditor': 1.0, 'bottom': 0.5 }

    // TODO: Enable unlocked curves
    property bool lockCurves: true

    property var selection: [];

    property bool showColorSlider: lockCurves && (curveRepeater.count == 3 || curveRepeater.count == 4)
    property bool alphaEnabled: true
    property alias initialized: timeline.isInitialized

    /* If true, shows non-linear gradients in the Gradient Slider (based off an approximation of the bezier curves)

    The default depends on whether any curves have showControlPoints == true */
    property bool showSubGradients: showColorSlider && (curveRepeater.itemAt(0).showControlPoints ||
                                     curveRepeater.itemAt(1).showControlPoints || curveRepeater.itemAt(2).showControlPoints ||
                                     (curveRepeater.count == 4 && curveRepeater.itemAt(3).showControlPoints))

    /* How many subdivisions will be used to calculate the linear approximation for the color gradient

    The default is 10*/
    property int subGradientDetail: 10

    /* Shows the linear approximation points for the gradient at a clicked point. (DEBUG ONLY)*/
    property bool showDebugPoints: false

    Layout.fillHeight: true
    color: palette.mainWindowColor

    onFocusChanged: {
        if ( curveEditor.focus === true )
        {
            timeline.requestPaint();
            repaintCurves();
        }
    }

    QtObject
    {
        id:_
        property var addedPointIndexes: []
        property var removedPointIndexes: []
        property var updatedPointIndexes: []
        property var handleIndexes: []
        property bool resetAll: false

        function clear()
        {
            addedPointIndexes = []
            removedPointIndexes = []
            updatedPointIndexes = []
            handleIndexes = []
        }

        function addPoint(index)
        {
            // Ignore add requests if the handle has already been added
            if(binarySearch(handleIndexes, index) < 0)
            {
                addIndex(addedPointIndexes, index)
            }
            else
            {
                addIndex(updatedPointIndexes, index)
            }
        }

        function updatePoint(index)
        {
            addIndex(updatedPointIndexes, index)
        }

        function removePoint(index)
        {
            // Ignore remove requests if the handle has already been removed
            if(binarySearch(handleIndexes, index) < 0)
            {
                addIndex(removedPointIndexes, index)
            }
            else
            {
                addIndex(updatedPointIndexes, index-1)
            }
        }

        function handleModified(index)
        {
            addIndex(handleIndexes, index)
        }

        function addIndex(collection, index)
        {
            if(index >= 0)
            {
                var valueIndex = binarySearch(collection, index)
                if(valueIndex < 0)
                {
                    collection.splice(-valueIndex-1, 0, index)
                }
            }
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
            beginUndoFrame();
            // Delete largest values first to enable easy indexing for undo
            for(var i = valuesToDelete.length-1; i >= 0; --i)
            {
                var curveIt = iterator(curves)
                while(curveIt.moveNext()){
                    curveIt.current.removeAt( valuesToDelete[i], true );
                }
            }
            endUndoFrame();
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
            curveRepeater.itemAt(0).getPoint(index).point.pos.y,
            curveRepeater.itemAt(1).getPoint(index).point.pos.y,
            curveRepeater.itemAt(2).getPoint(index).point.pos.y,
            ((curveRepeater.count == 4) ? curveRepeater.itemAt(3).getPoint(index).point.pos.y : 1))
    }

    function getPositionAt(index)
    {
        return curveRepeater.itemAt(0).getPoint(index).point.pos.x
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
        Item {
            id: toolbarFrame
            Layout.fillWidth: true
            Layout.preferredHeight: childrenRect.height + defaultSpacing.standardMargin

            CurveEditorToolbar {
                id: toolbar

                title: subTitle
                time:  selection.length > 0 ? selection[selection.length-1].point.pos.x : 0
                value:  selection.length > 0 ? selection[selection.length-1].point.pos.y : 0
                timeScale: xScale
                valueScale: yScale
                editEnabled: selection.length > 0
                timeScaleEnabled: timeScaleEditEnabled

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: defaultSpacing.standardMargin

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
        }

        WGGridCanvas {
            id:timeline
            focus: true
            timeScale: xScale
            valueScale: yScale
            Layout.fillHeight: true
            Layout.fillWidth: true
            useAxisScaleLimit: yScaleLimit
            maxYScaleFactor: -1000

            property var isInitialized: false
            property int oldWidth: timeline.width
            property int oldHeight: timeline.height

            onWidthChanged: {
                if(oldWidth > 0 && oldHeight > 0)
                {
                    var leftMargin = timeline.viewTransform.origin.x / oldWidth
                    var rightMargin = timeline.viewTransform.xScale / oldWidth

                    timeline.viewTransform.origin.x = timeline.width * leftMargin;
                    timeline.viewTransform.xScale = timeline.width * rightMargin;

                    timeline.requestPaint();
                    curveEditor.repaintCurves();
                }
                if( !isInitialized && width > 0 && height > 0 )
                {
                    timeline.zoomExtents();
                    isInitialized = true;
                }
                oldWidth = timeline.width
            }

            onHeightChanged: {
                if(oldWidth > 0 && oldHeight > 0)
                {
                    var topMargin = timeline.viewTransform.origin.y / oldHeight
                    var botMargin = timeline.viewTransform.yScale / oldHeight

                    timeline.viewTransform.origin.y = timeline.height * topMargin;
                    timeline.viewTransform.yScale = timeline.height * botMargin;

                    timeline.requestPaint();
                    curveEditor.repaintCurves();
                }
                if( !isInitialized && width > 0 && height > 0 )
                {
                    timeline.zoomExtents();
                    isInitialized = true;
                }
                oldHeight = timeline.height
            }

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
            WGListModel
            {
                id: curvesModel
                source: curves
                ValueExtension {}
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

                onCountChanged: {
                    _.resetAll = true
                    updateGradientTimer.restart()
                }

                delegate: Curve{
                    objectName: "curve" + index
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
                        _.updatePoint(point.pointIndex)
                        // Also update the previous point's sub-gradiant
                        _.updatePoint(point.pointIndex-1)
                        curveEditor.pointPositionChanged(point, xDelta, yDelta)
                    }
                    onPointPressed:{
                        if(point.selected === false && mouse.modifiers !== Qt.ControlModifier)
                        {
                            clearSelection();
                        }
                        timeline.debugLine = curveIndex
                    }
                    onPointAdded:{
                        _.addPoint(index)
                        updateGradientTimer.restart()
                    }
                    onPointRemoved:{
                        _.removePoint(index)
                        updateGradientTimer.restart()
                    }
                    onPointClicked:{
                        if(mouse.modifiers !== Qt.ControlModifier)
                        {
                            clearSelection();
                        }
                    }
                    onPointUpdated:{
                        _.updatePoint(point.pointIndex)
                        // Also update the previous point's sub-gradiant
                        _.updatePoint(point.pointIndex-1)
                        updateGradientTimer.restart()
                    }
                }

                onItemRemoved:
                {
                    curveEditor.curveRemoved(item);
                }
            }

            // *** DEBUG CODE ***
            // An array of points represented by squares to show linear approximation ***
            // Enabled by making showDebugPoints >= 0

            signal updateDebug()

            property var debugPoints: []
            property int debugCount: timeline.debugPoints.length
            property int debugLine: 0

            onUpdateDebug: {
                timeline.debugCount = timeline.debugPoints.length
            }

            Repeater {
                id: debugSquareRepeater
                model: showDebugPoints ? timeline.debugCount : 0

                delegate: Rectangle {
                    height: 4
                    width: 4
                    color: "transparent"
                    border.color: curveRepeater.itemAt(timeline.debugLine).color
                    border.width: 1
                    visible: index < timeline.debugPoints.length
                    x: index < timeline.debugPoints.length ? timeline.debugPoints[index].x - 2 : 0
                    y: index < timeline.debugPoints.length ? timeline.debugPoints[index].y - 2 : 0
                }
            }

            // *** END DEBUG CODE ***

            CurveEditorContextMenu {}

            WGGradientSlider {
                id: colorGradient
                visible: showColorSlider
                anchors.bottom: timeline.bottom
                x: Math.round(timeline.viewTransform.transformX(0))
                width: Math.round(timeline.viewTransform.transformX(1) - timeline.viewTransform.transformX(0))
                height: defaultSpacing.minimumRowHeight
                minimumValue: 0
                maximumValue: 1.0
                stepSize: .0001
                onVisibleChanged: updateGradientTimer.restart()

                onChangeValue: {
                    if(!Qt._updatingPosition && !Qt._updatingCurveGradient)
                    {
                        var red = curveRepeater.itemAt(0).getPoint(index);
                        var green = curveRepeater.itemAt(1).getPoint(index);
                        var blue = curveRepeater.itemAt(2).getPoint(index);
                        var alpha = curveRepeater.count == 4 ?
                                    curveRepeater.itemAt(3).getPoint(index) : null
                        beginUndoFrame()
                        red.setPosition(val, red.point.pos.y)
                        green.setPosition(val, green.point.pos.y)
                        blue.setPosition(val, blue.point.pos.y)
                        if(alpha){
                            alpha.setPosition(val, alpha.point.pos.y)
                        }
                        endUndoFrame()
                        repaintCurves()
                    }
                }

                onColorModified: {
                    if(!Qt._updatingCurveGradient)
                    {
                        var red = curveRepeater.itemAt(0).getPoint(index);
                        var green = curveRepeater.itemAt(1).getPoint(index);
                        var blue = curveRepeater.itemAt(2).getPoint(index);
                        var alpha = curveRepeater.count == 4 ?
                                    curveRepeater.itemAt(3).getPoint(index) : null
                        beginUndoFrame()
                        red.setPosition(red.point.pos.x, color.r)
                        green.setPosition(green.point.pos.x, color.g)
                        blue.setPosition(blue.point.pos.x, color.b)
                        if(alpha){
                            alpha.setPosition(alpha.point.pos.x, color.a)
                        }
                        endUndoFrame()
                    }
                }

                onHandleAdded: {
                    console.assert(curveRepeater.count > 0)
                    if(!Qt._updatingCurveGradient)
                    {
                        // Prevent adding new points if the addition of the point created this handle
                        if(colorGradient.__handleCount === curveRepeater.itemAt(0).pointRepeater.count)
                            return
                        var relPos = colorGradient.getHandleValue(index)
                        var mousePos = timeline.viewTransform.transformX(relPos)
                        beginUndoFrame()
                        _.handleModified(index)
                        curveEditor.addPointsToCurves(Qt.point(mousePos,0))
                        endUndoFrame()
                    }
                }

                onHandleRemoved: {
                    console.assert(curveRepeater.count > 0)
                    if(!Qt._updatingCurveGradient)
                    {
                        // Prevent deleting points multiple times if the deletion of the point removed this handle
                        if(colorGradient.__handleCount === curveRepeater.itemAt(0).pointRepeater.count)
                            return
                        var red = curveRepeater.itemAt(0).getPoint(index).point;
                        _.handleModified(index)
                        curveEditor.deletePointsAt([red.pos.x]);
                    }
                }

                onSliderDoubleClicked: {
                    if (useColorPicker)
                    {
                        clearSelection();
                    }
                }

                function updateSubGradients (index)
                {
                    var newDebugArray = []

                    var indexStart = index >= 0 ? Math.max(index, 0) : 0
                    var indexEnd = index >= 0 ? Math.min(index + 1, colorGradient.__handleCount - 1) : colorGradient.__handleCount - 1

                    // last handle never needs an approximated gradient (it's always a single color)

                    // generates linear approximations of the curves to create a gradient
                    for (var i = indexStart; i < indexEnd; i++)
                    {
                        var subCurves = []
                        for (var c = 0; c < curveRepeater.count; c++)
                        {
                            //gets a linear array of points approximating the curve from point(i)
                            subCurves.push(curveRepeater.itemAt(c).getLinearGradient(i, subGradientDetail))
                        }

                        var subCols = []
                        var subVals = []

                        // create a new gradient based off the linear approximations above
                        // has to be done via a string of QML as gradients are very restrictive and un-dynamic

                        var gradString = "import QtQuick 2.4; Gradient { "

                        for (var j = 0; j <= subGradientDetail+1; j++)
                        {
                            //adds points to array to show approximated linear curve
                            if (showDebugPoints && timeline.debugLine >= 0 && timeline.debugLine <= curveRepeater.count)
                            {
                                newDebugArray.push(timeline.viewTransform.transform(subCurves[timeline.debugLine][j]));
                            }

                            var t = (1 / (subGradientDetail + 1)) * j
                            var newR = subCurves.length >= 1 ? subCurves[0][j].y : 1
                            var newG = subCurves.length >= 2 ? subCurves[1][j].y : 1
                            var newB = subCurves.length >= 3 ? subCurves[2][j].y : 1
                            var newA = subCurves.length == 4 ? subCurves[3][j].y : 1
                            var newColor = Qt.rgba(newR, newG, newB, newA)

                            subCols[j] = newColor
                            subVals[j] = t

                            gradString += "GradientStop { position: " + subVals[j] + "; color: '" + subCols[j] + "' } "
                        }

                        gradString += " }"

                        var newGradient = Qt.createQmlObject(gradString,colorGradient)

                        colorGradient.setHandleGradient(i + 1, newGradient)
                    }
                    // update debug repeater
                    if (showDebugPoints)
                    {
                        timeline.debugPoints = newDebugArray
                        timeline.updateDebug()
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

                        // When the curve count changes clear out the previous gradient and recreate it
                        if(_.resetAll)
                        {
                            _.resetAll = false
                            _.clear()

                            while(colorGradient.__handleCount > 0)
                            {
                                colorGradient.removeHandle(colorGradient.__handleCount - 1)
                            }
                            for(var index = 0; index < pointCount; ++index)
                            {
                                _.addPoint(index)
                            }
                        }

                        // Update existing handles
                        var gradHandleValues = []
                        var gradHandleColors = []
                        for(var i = 0; i < _.updatedPointIndexes.length; ++i)
                        {
                            gradHandleValues.push(curveEditor.getPositionAt(_.updatedPointIndexes[i]))
                            gradHandleColors.push(curveEditor.getColorAt(_.updatedPointIndexes[i]))
                        }

                        colorGradient.setHandleValue(gradHandleValues, _.updatedPointIndexes);
                        colorGradient.setHandleColor(gradHandleColors, _.updatedPointIndexes);

                        // Remove handles corresponding to removed points, highest index first
                        for(var r = _.removedPointIndexes.length-1; r >= 0; --r)
                        {
                            var index = _.removedPointIndexes[r]
                            colorGradient.removeHandle(index)
                            // Update the index accounting for all handles being removed
                            index = index - r
                            _.updatePoint(index-1)
                        }

                        // Create new handles for new points
                        for(var a = 0; a < _.addedPointIndexes.length; ++a)
                        {
                            var index = _.addedPointIndexes[a]
                            var point = curveRepeater.itemAt(curveRepeater.count - 1).pointRepeater.itemAt(index)

                            var newColor = curveEditor.getColorAt(index)

                            _.updatePoint(index)
                            // Also update the previous point
                            if(index > 0)
                            {
                                _.updatePoint(index-1)
                            }
                            // Calculate the gradient index based on the number of points added
                            colorGradient.createColorHandle(point.point.pos.x, handleStyle, index, newColor)
                        }

                        for(var i = 0; i < _.updatedPointIndexes.length; ++i)
                        {
                            colorGradient.updateSubGradients(_.updatedPointIndexes[i])
                        }

                        _.clear()

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
        interval: 0

        onTriggered: { colorGradient.syncHandles() }
    }
}
