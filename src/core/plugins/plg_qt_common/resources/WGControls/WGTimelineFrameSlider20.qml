import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import WGControls 2.0

/*!
 \brief Slider with a multiple handles that should really only be used in the Timeline Control

\code{.js}

**** TODO **** {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    lowerValue: 25
    upperValue: 75
    stepSize: 1
    enabled: true
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
