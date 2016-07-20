import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import WGControls 2.0

/*!
 \brief A slider with a multiple keyframe handles and no bar that should really only be used in the Timeline Control

\code{.js}

WGTimelineFrameSlider {
    id: frameSlider
    minimumValue: 0
    maximumValue: totalFrames
    startFrame: 25
    endFrame: 75
    stepSize: 1
    enabled: true

    Repeater {
        model: keys

        WGTimelineFrameSliderHandle {
            id: frameSliderHandle
            minimumValue: frameSlider.minimumValue
            maximumValue: frameSlider.maximumValue
            showBar: false
            value: time * rootFrame.framesPerSecond
            frameType: type

            label: label
        }
    }
}

\endcode
*/

WGSlider {
    objectName: "slider"
    id: slider

    activeFocusOnPress: true

    style: WGTimelineFrameSliderStyle{}

    // initial handle values to apply deltas to
    property var initialValues: []

    property bool showLabel: false

    grooveClickable: false

    handleClamp: false

    clickAreaBuffer: 50

    allowMouseWheel: false

    signal dragSelectedHandles(real delta)

    onHandleClicked: {
        for (var i = 0; i < slider.__handlePosList.length; i++)
        {
            slider.initialValues[i] = slider.__handlePosList[i].value
        }
    }

    // tell the view a drag has started
    onBeginDrag: {
        __handlePosList[index].handleDragging = true
    }

    onEndDrag: {
        __handlePosList[index].handleDragging = false
        initialValues = []
    }
}
