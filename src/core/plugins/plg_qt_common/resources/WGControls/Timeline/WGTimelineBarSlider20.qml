import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls.Styles 2.0
import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
 \brief Slider with a large bar and invisible handles that should really only be used in the Timeline Control

\code{.js}

WGTimelineBarSlider {
    minimumValue: 0
    maximumValue: totalFrames
    startFrame: 25
    endFrame: 75
    stepSize: 1
    enabled: true

    barColor: "blue"
}

\endcode
*/


WGSlider {
    objectName: "slider"
    id: slider
    WGComponent { type: "WGTimelineBarSlider20" }

    activeFocusOnPress: true

    style: WGTimelineBarSliderStyle{}

    property real startFrame: 0
    property real endFrame: 1

    // initial handle values to apply deltas to
    property var initialValues: []

    property point mouseInitialPoint: Qt.point(0,0)

    property int barIndex: -1

    // the minimum width the slider bar can be. Should be within the minimum and maximumum values.
    property real minimumBarWidth: 1

    // this is set to the mouseArea inside the colored bar.
    property QtObject barArea

    grooveClickable: false

    handleClamp: false

    clickAreaBuffer: 50

    allowMouseWheel: false

    onHandleClicked: {
        initialValues[0] = __handlePosList[0].value
        initialValues[1] = __handlePosList[1].value
    }

    onBeginDrag: {
        __handlePosList[index].handleDragging = true
    }

    onEndDrag: {
        __handlePosList[index].handleDragging = false
        initialValues = []
    }

    signal updateSelection (bool barSelected)

    signal barDragging (real delta)

    signal barEndDragging ()

    signal barPressed (int modifiers)

    property Component barContent: Rectangle {
        id: barContent
        color: slider.barColor

        border.width: 1
        border.color: Qt.darker(color, 1.5)

        // change the styling if bar is selected
        Connections {
            target: slider
            onUpdateSelection: {
                barContent.border.color = barSelected ? Qt.darker(color, 2.0) : Qt.darker(color, 1.5)
                barSelection.visible = barSelected
            }
        }

        // opaque white selection rectangle. Initially invisible
        Rectangle {
            id: barSelection
            anchors.fill: parent
            anchors.margins: defaultSpacing.doubleBorderSize
            visible: false
            color: "#99FFFFFF"
        }

        // Mousearea for dragging the bar around. Will automatically drag the handles and tell the view something is being dragged
        MouseArea {
            id: barMouseArea
            anchors.fill: parent

            // buffer so the handle and bar mouseareas don't overlap
            anchors.leftMargin: 3
            anchors.rightMargin: 3

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            // give the view the current mouse delta (only the X really matters)
            onPositionChanged: {
                var currentPos = barMouseArea.mapToItem(slider,mouse.x,mouse.y).x

                //clamp it to the min or max values or weird things happen when the mouse strays too far from the current window
                currentPos = Math.max(0, Math.min(slider.width, currentPos))

                slider.barDragging(currentPos - slider.mouseInitialPoint.x)
            }

            onPressed: {
                // populate the initial mouse values in order to calculate delta

                initialValues[0] = __handlePosList[0].value
                initialValues[1] = __handlePosList[1].value

                slider.mouseInitialPoint = barMouseArea.mapToItem(slider,mouse.x,mouse.y)

                slider.barPressed(mouse.modifiers)

                beginUndoFrame();

                preventStealing = true
                __handleMoving = true
            }

            onReleased: {
                endUndoFrame();
                barEndDragging();
                preventStealing = false
                __handleMoving = false
            }
        }
    }

    WGTimelineBarSliderHandle {
        id: sliderMinHandle
        minimumValue: slider.minimumValue
        maximumValue: sliderMaxHandle.value
        showBar: false
        rangePartnerHandle: sliderMaxHandle
        value: slider.startFrame
        maxHandle: false

        Connections {
            target: slider
            onStartFrameChanged: {
                sliderMinHandle.value = startFrame
            }
        }

        onValueChanged: {
            model.startTime = value / timelineFrame.framesPerSecond;
        }

        // need to set max value here or the value might be clamped before the value is valid
        Component.onCompleted: {
            maximumValue = Qt.binding(function() { return sliderMaxHandle.value - minimumBarWidth })
        }
    }

    WGTimelineBarSliderHandle {
        id: sliderMaxHandle
        minimumValue: sliderMinHandle.value
        maximumValue: slider.maximumValue
        showBar: true
        barMinPos: (sliderMinHandle.value * (parentSlider.__clampedLength / (parentSlider.maximumValue - parentSlider.minimumValue))) + parentSlider.__visualMinPos
        rangePartnerHandle: sliderMinHandle
        value: slider.endFrame
        maxHandle: true

        Connections {
            target: slider
            onEndFrameChanged: {
                sliderMaxHandle.value = endFrame
            }
        }

        onValueChanged: {
            model.endTime = value / timelineFrame.framesPerSecond;
        }

        // need to set min value here or the value might be clamped before the value is valid
        Component.onCompleted: {
            minimumValue = Qt.binding(function() { return sliderMinHandle.value + minimumBarWidth })
        }
    }
}
