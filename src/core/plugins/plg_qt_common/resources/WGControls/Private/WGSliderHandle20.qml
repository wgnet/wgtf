import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import WGControls 2.0

/*!
 \ingroup wgcontrols
 \brief A Slider handle intended for the WGSlider Control

Example:
\code{.js}
WGSlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    stepSize: 1.0

    WGSliderHandle {
        value: 50

        handleStyle: Rectangle {
            color: "grey"
            implicitHeight: 20
            implicitWidth: 20
        }
    }
}
\endcode
*/

Item {
    id: sliderHandle
    objectName: "SliderHandle"

    /*!
        The parent slider object
    */
    property QtObject parentSlider

    property alias range: range

    /*!
        The internal color of the handle
    */
    property color handleColor: palette.lightPanelColor

    /*!
        The color of the bar attached to the handle
    */
    property color barColor: parentSlider.barColor

    /*!
        Allows a slider bar's minimum point to be attached to another position (usually another handle for range sliders).
    */
    property int barMinPos: 0

    /*!
        This property determines whether the coloured bar will be shown or not.
        The default value is \c true
    */

    property bool showBar: true

    /*!
        This property holds the minimum value of the handle.
        The default value is \c{0.0}.
    */

    property alias minimumValue: range.minimumValue

    /*!
        This property holds the maximum value of the handle.
        The default value is \c{1.0}.
    */
    property alias maximumValue: range.maximumValue

    /*!
        This property holds the current value of the handle.
        The default value is \c{0.0}.
    */
    property alias value: range.value

    /*!
        The offset of the handle on the slider. This is useful if you want the handle offset from the value.
        (For example if you want the handles to resize the internal bar without obscuring it)

        The default value is parentSlider.__handleWidth / 2 which centers the handle on the exact value.
    */
    property int handleOffset: - parentSlider.__handleWidth / 2

    /*!
        The mouse cursor when the handle is hovered.

        The default value is the standard arrow cursor.
    */
    property var hoverCursor: Qt.ArrowCursor

    /*!
        The mouse cursor when the handle is dragged.

        The default value is hoverCursor
    */
    property var dragCursor: hoverCursor

    /*!
        This is the Component for the handle style.

        This can be any Item based component.
    */
    property Component handleStyle: WGButtonFrame{
        color: parentSlider.enabled ? handleColor : palette.mainWindowColor
        borderColor: parentSlider.enabled ? palette.darkerShade : palette.darkShade
        highlightColor: parentSlider.hoveredHandle === handleIndex ? palette.lighterShade : "transparent"
        innerBorderColor: parentSlider.__activeHandle === handleIndex && parentSlider.activeFocus ? palette.highlightShade : "transparent"
        implicitWidth: handleWidth
        implicitHeight: handleHeight
    }

    property int handleWidth: Math.round(defaultSpacing.minimumRowHeight * 0.75)

    property int handleHeight: Math.round(defaultSpacing.minimumRowHeight * 0.75)

    /*! \internal */
    property bool __horizontal: parentSlider.__horizontal

    function updatePos() {
        if (parentSlider.__handleMoving)
        {
            var newValue = range.valueForPosition(sliderHandle.x)
            setValueHelper(sliderHandle, "value", newValue);
        }
    }

    RangeModel {
        objectName: "SliderRange"
        id: range
        stepSize: parentSlider.stepSize

        minimumValue: parentSlider.minimumValue
        maximumValue: parentSlider.maximumValue

        positionAtMinimum:  {
            if(sliderHandle.minimumValue == parentSlider.minimumValue)
            {
                parentSlider.__visualMinPos
            }
            else if (sliderHandle.minimumValue > parentSlider.minimumValue)
            {
                ((sliderHandle.minimumValue - parentSlider.minimumValue) * (parentSlider.__clampedLength / (parentSlider.maximumValue - parentSlider.minimumValue)))
            }
        }

        positionAtMaximum: {
            if(sliderHandle.maximumValue == parentSlider.maximumValue)
            {
                parentSlider.__visualMaxPos
            }
            else if (sliderHandle.maximumValue < parentSlider.maximumValue)
            {
                ((sliderHandle.maximumValue - parentSlider.minimumValue) * (parentSlider.__clampedLength / (parentSlider.maximumValue - parentSlider.minimumValue)))
            }
        }
    }
}

