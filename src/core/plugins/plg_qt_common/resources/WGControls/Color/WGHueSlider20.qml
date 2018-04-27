import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Global 2.0
import WGControls.Private 2.0

/*!
 \Three WGColorValueSliders with color gradients representing Hue, Saturation and Value/Brightness/Lightness.

 The Saturation and Value sliders update their color gradients based on the current values.

Example:
\code{.js}
WGHueSlider {
    id: hslPicker
    Layout.fillWidth: true
    rgbaColor: myVec4Color

    onChangeColor: {
        if (myVec4Color != col)
        {
            myVec4Color = Qt.vector4d(col.x,col.y,col.z,col.w)
        }
    }
}
\endcode
*/

WGAbstractColor {
    id: hueSlider
    objectName: "WGHueSlider"
    WGComponent { type: "WGHueSlider20" }
    /*!
        The width of the number boxes on the right hand side.

        The default is 70.
    */
    property int numBoxWidth: 70

    /*!
        The maximum v value of the control
    */
    property real brightnessRange: 1.0

    /*!
        If the hue value should be shown as 0 to 360
    */
    property bool hue360: false

    altColorSpace: "HSV"

    ColumnLayout {
        anchors.fill: parent

        WGColorValueSlider {
            id: hSlider
            objectName: "hueColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            rgbaColor: hueSlider.rgbaColor
            altColor: hueSlider.altColor
            altColorSpace: hueSlider.altColorSpace
            useHDR: hueSlider.useHDR
            tonemap: hueSlider.tonemap
            decimalPlaces: hueSlider.decimalPlaces
            brightnessRange: 1.0
            numBoxWidth: hueSlider.numBoxWidth
            useHexValue: false

            primaryChannel: "x"

            sliderLabel: "H:"

            numberBoxData: hue360 ? hDc : sliderControl

            WGDataConversion {
                id: hDc
                dataTarget: hSlider.sliderControl
                dataToValue: function(val) {
                    return val * 360
                }
                valueToData: function(val) {
                    return val / 360
                }
            }

            onChangeColor: {
                if (col != hueSlider.rgbaColor)
                {
                    hueSlider.changeColor(col)
                }
            }
        }

        WGColorValueSlider {
            id: sSlider
            objectName: "satColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            rgbaColor: hueSlider.rgbaColor
            altColor: hueSlider.altColor
            altColorSpace: hueSlider.altColorSpace
            useHDR: hueSlider.useHDR
            tonemap: hueSlider.tonemap
            decimalPlaces: hueSlider.decimalPlaces
            brightnessRange: 1.0
            numBoxWidth: hueSlider.numBoxWidth
            useHexValue: false

            primaryChannel: "y"

            sliderLabel: "S:"

            onChangeColor: {
                if (col != hueSlider.rgbaColor)
                {
                    hueSlider.changeColor(col)
                }
            }
        }

        WGColorValueSlider {
            id: vSlider
            objectName: "valueColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            rgbaColor: hueSlider.rgbaColor
            altColor: hueSlider.altColor
            altColorSpace: hueSlider.altColorSpace
            useHDR: hueSlider.useHDR
            tonemap: hueSlider.tonemap
            decimalPlaces: hueSlider.decimalPlaces
            brightnessRange: hueSlider.brightnessRange
            numBoxWidth: hueSlider.numBoxWidth
            useHexValue: false

            primaryChannel: "z"

            sliderLabel: altColorSpace == "HSV" ? "V:" : "L:"

            onChangeColor: {
                if (col != hueSlider.rgbaColor)
                {
                    hueSlider.changeColor(col)
                }
            }
        }
    }
}
