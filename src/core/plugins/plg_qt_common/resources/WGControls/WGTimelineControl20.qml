import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1

import WGControls 2.0

/*!
    \brief A multitrack timeline control to show durations and keyframes of various tracks/objects/events.
*/

Rectangle {
    id: timelineFrame

    color: palette.mainWindowColor

    // is fps necessary?
    property int framesPerSecond: 30

    property int totalFrames: (framesPerSecond * timeScale)

    property real frameWidth: gridCanvas.canvasWidth / totalFrames

    property int topMargin: (defaultSpacing.minimumRowHeight * 4) + timelineToolbar.height

    property int currentFrame: 0

    property alias model: timelineView.model

    property alias timeScale: gridCanvas.timeScale

    property bool previewPlaying: false

    property bool showLabels: true

    signal yPositionChanged(var yPos)

    signal eventFired(var eventName, var eventAction, var eventValue)

    // Sets the yPosition. Useful for linking up another Flickable/ListView
    function setViewYPosition(yPos) {
        if (timelineView.contentY != yPos)
        {
            timelineView.contentY = yPos
        }
    }

    function pixelsToFrames(pixels) {
        return Math.round(pixels / frameWidth)
    }

    function framesToPixels(frames) {
        return frames * frameWidth
    }

    Component.onCompleted: {
        timelineFrame.currentFrame = 0
    }

    onFocusChanged: {
        if (focus === true) {
            gridCanvas.requestPaint();
        }
    }

    property alias setTimeDialog: timelineDialogs.setTimeDialog

    WGTimelineDialogs {
        id: timelineDialogs
    }


    // animates currentFrame by the timeScale (seconds)
    NumberAnimation on currentFrame {
        id: playbackAnim
        from: 0
        to: totalFrames
        running: false
        duration: (timeScale * 1000)
        loops: timelineToolbar.looping ? Animation.Infinite : 1
        onStopped: {
            if (currentFrame == totalFrames)
            {
                playbackAnim.stop()
                playbackAnim.paused = false
                timelineFrame.previewPlaying = false
                timelineFrame.currentFrame = 0
                timelineToolbar.playing = false
                timelineToolbar.paused = false
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // toolbar for holding useful buttons that affect the timeline
        WGTimelineToolbar {
            id: timelineToolbar
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin

            onPlayingChanged: {
                if (playing)
                {
                    playbackAnim.start()
                    timelineFrame.previewPlaying = true
                }
                else
                {
                    playbackAnim.stop()
                    timelineToolbar.paused = false
                    timelineFrame.previewPlaying = false
                    currentFrame = 0
                }
            }

            onPausedChanged: {
                if (paused)
                {
                    playbackAnim.pause()
                    timelineFrame.previewPlaying = false
                }
                else
                {
                    playbackAnim.resume()
                    timelineFrame.previewPlaying = true
                }
            }
        }

        WGGridCanvas {
            id: gridCanvas

            Layout.fillHeight: true
            Layout.fillWidth: true

            focus: true
            useAxis: xGrid
            showMouseLine: false
            showXGridLines: true
            showYGridLines: false

            z: 0

            // Trigger selection changes on dragging out a selection rectangle
            onPreviewSelectArea: {
                timelineView.selectionChanged()
            }

            // Reset all selection on a single click of the canvas
            onCanvasPressed: {
                timelineView.selectedBars = []
                timelineView.selectedHandles = []
                timelineView.selectionChanged()
            }

            // Draggable current time handle that moves automatically on playback.
            WGSlider {
                id: frameScrubber
                width: gridCanvas.canvasWidth
                height: defaultSpacing.minimumRowHeight
                x: gridCanvas.viewTransform.transformX(0)
                anchors.bottom: timelineView.top
                anchors.bottomMargin: defaultSpacing.standardMargin

                z: 5
                minimumValue: 0
                maximumValue: totalFrames
                stepSize: 1

                enabled: !timelineFrame.previewPlaying

                handleClamp: false

                onValueChanged: {
                    timelineFrame.currentFrame = value
                }

                Connections {
                    target: timelineFrame
                    onCurrentFrameChanged: {
                        frameScrubber.value = currentFrame
                    }
                }

                style: WGSliderStyle {
                    groove: Item {
                        implicitWidth: defaultSpacing.minimumRowHeight
                        implicitHeight: defaultSpacing.minimumRowHeight
                        WGTextBoxFrame {
                            radius: defaultSpacing.standardRadius
                            anchors.fill: parent
                        }
                    }
                    bar: null
                }

                // small chunky handle with a thin red line beneath it
                WGSliderHandle {
                    showBar: false
                    handleStyle: WGButtonFrame {
                        id: scrubberHandle
                        implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
                        implicitHeight: defaultSpacing.minimumRowHeight
                        color: palette.mainWindowColor
                        borderColor: frameScrubber.enabled ? palette.darkerShade : palette.darkShade
                        highlightColor: frameScrubber.hoveredHandle === handleIndex ? palette.lighterShade : "transparent"
                        innerBorderColor: frameScrubber.__activeHandle === handleIndex && frameScrubber.activeFocus ? palette.highlightShade : "transparent"

                        Rectangle {
                            anchors.top: parent.top
                            anchors.topMargin: -defaultSpacing.standardMargin
                            anchors.horizontalCenter: parent.horizontalCenter
                            height: gridCanvas.height - defaultSpacing.minimumRowHeight * 3 + defaultSpacing.doubleMargin
                            z: -1
                            width: 2
                            color: "#77FF0000"
                        }
                    }
                    onValueChanged: {
                        frameScrubber.value = value
                    }
                }
            }

            WGTimelineContextMenu {

            }

            // The list of bars, keyframes etc.
            ListView {
                id: timelineView
                model: timelineModel

                width: gridCanvas.canvasWidth
                height: gridCanvas.canvasHeight

                x: gridCanvas.viewTransform.transformX(0)
                y: defaultSpacing.minimumRowHeight * 4

                interactive: false

                spacing: 1
                z: 1

                property var selectedBars: []

                property var selectedHandles: []

                // if a bar or a handle is dragged
                property bool itemDragging: false

                // sent if the bar or handle selection is changed
                signal selectionChanged()

                // sent if a moveable item in the view is dragged.
                signal itemDragged(real delta, bool minHandle, bool maxHandle, bool bar)

                // sent to select all bars and points in the timeline
                signal selectAll()

                // sends any vertical movement of the content to the root (useful for linking other Flickables/ListViews)
                onContentYChanged: {
                    timelineFrame.yPositionChanged(contentY)
                }

                function changeTime(startHandle, endHandle)
                {
                    setTimeDialog.startHandle = startHandle

                    if (typeof endHandle != "undefined")
                    {
                        setTimeDialog.endTime = true
                        setTimeDialog.endHandle = endHandle
                    }
                    else
                    {
                        setTimeDialog.endTime = false
                    }

                    setTimeDialog.open()
                }

                // check if minPoint and maxPoint lie wholly within min and max
                // or if minPoint and maxPoint y's are wholly within AND minPoint and maxPoint x's are wholly without
                function checkSelection(min, max, minPoint, maxPoint)
                {
                    //y selection is pretty basic
                    if (minPoint.y >= min.y && maxPoint.y <= max.y)
                    {
                        //x selection is more picky. Could make this select any bar the selection crosses... but this may be too coarse.
                        if ((minPoint.x <= min.x && maxPoint.x >= max.x)
                                || (minPoint.x >= min.x && maxPoint.x <= max.x))
                        {
                            return true
                        }
                        else
                        {
                            return false
                        }
                    }
                }

                // this really doesn't seem like the ideal way to reflect on the model data... but will do for now.
                delegate: Loader {
                    id: rowDelegate
                    height: defaultSpacing.minimumRowHeight
                    anchors.left: parent.left
                    anchors.right: parent.right

                    property var view: rowDelegate.ListView.view

                    sourceComponent: {
                        if (type == "textBox")
                        {
                            return textObject
                        }
                        else if (type == "fullBar")
                        {
                            return fullBar
                        }
                        else if (type == "barSlider")
                        {
                            return barSlider
                        }
                        else if (type == "frameSlider")
                        {
                            return frameSlider
                        }
                    }

                    // a full width textbox, possibly for entering scripts? conditions? comments?
                    property Component textObject: WGTextBox {
                        id: textObject
                        text: model.text
                        onEditAccepted: {
                            model.text = text;
                        }
                    }

                    // a full width non interactive bar with a label. Useful for organisation mostly.
                    property Component fullBar: WGButtonFrame {
                        id: fullBar
                        color: model.barColor
                        WGLabel {
                            anchors.centerIn: parent
                            text: model.name
                        }
                    }

                    // coloured moveable and scalable bar
                    property Component barSlider: WGTimelineBarDelegate {
                        id: barSlider

                        minimumValue: 0
                        maximumValue: totalFrames
                        stepSize: 1
                        startFrame: model.startTime * timelineFrame.framesPerSecond
                        endFrame: model.endTime * timelineFrame.framesPerSecond
                        barColor: model.barColor

                        barIndex: index

                        view: timelineView
                        grid: gridCanvas
                        rootFrame: timelineFrame
                    }

                    // a multi handle slider of keyframes
                    property Component frameSlider: WGTimelineKeyframeDelegate {
                        id: frameSlider

                        minimumValue: 0
                        maximumValue: totalFrames
                        stepSize: 1

                        barColor: model.barColor

                        showLabel: showLabels

                        view: timelineView
                        grid: gridCanvas

                        //for some reason these have to be given different names or they cause errors
                        rootFrame: timelineFrame
                        keys: keyFrames
                    }
                }
            }
        }



        // Slideable depiction of total duration and visible duration.

        WGTimelineBarSlider {
            id: timelineArea

            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            handleClamp: false

            property real leftBound: Math.max(gridCanvas.viewTransform.inverseTransform(Qt.point(0,0)).x,0)
            property real rightBound: Math.min(gridCanvas.viewTransform.inverseTransform(Qt.point(gridCanvas.width,0)).x,1)

            property real barWidth: rightBound - leftBound

            property real initialTransformOrigin: 0

            barColor: "#77AAAAAA"

            startFrame: gridCanvas.width > 0 ? leftBound : 0
            endFrame: gridCanvas.width > 0 ? rightBound : 1

            grooveClickable: false

            stepSize: 0.001

            minimumValue: 0
            maximumValue: 1

            minimumBarWidth: 0

            style: WGSliderStyle {
                groove: WGTextBoxFrame {
                    radius: defaultSpacing.standardRadius
                    anchors.verticalCenter: parent.verticalCenter
                    implicitWidth: defaultSpacing.minimumRowHeight
                    implicitHeight: defaultSpacing.minimumRowHeight
                    color: control.enabled ? palette.textBoxColor : "transparent"
                }
                bar: Item {
                    Loader
                    {
                        sourceComponent: control.barContent
                        clip: true

                        anchors.fill: parent
                    }
                }
            }

            onChangeValue: {
                // TODO Make the zoomed area expand/contract based on stretching or shrinking the handles.
                // Maths too hard.
            }

            onBarPressed: {
                initialTransformOrigin = gridCanvas.viewTransform.origin.x
            }

            onBarDragging: {
                // TODO Make the bar not shrink in width when it goes past the bounds.
                gridCanvas.viewTransform.origin.x = initialTransformOrigin - delta * (gridCanvas.viewTransform.xScale / timelineArea.width)
                gridCanvas.requestPaint()
            }

            onLeftBoundChanged: {
                if (!__handleMoving)
                {
                    timelineArea.setHandleValue(leftBound, 0)
                }
            }

            onRightBoundChanged: {
                if (!__handleMoving)
                {
                    timelineArea.setHandleValue(rightBound, 1)
                }
            }
        }
    }
}
