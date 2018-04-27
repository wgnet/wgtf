import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \Three WGColorSliders with color gradients representing Red, Green and Blue color values. These values are always
 stored as real values from 0 to 1 but can be displayed as 0 to 255 hex values if useHexValue = true.

 The sliders update their color gradients based on the current values.

Example:
\code{.js}
WGRgbSlider {
    id: rgbPicker
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
    id: rgbSlider
    objectName: "WGRgbSlider"
    WGComponent { type: "WGRgbSlider20" }

    /*!
        The property determines whether the number boxes will show values from 0 to 1 or 0 to 255.

        The default is true.
    */
    property bool useHexValue: true

    /*!
        The width of the number boxes on the right hand side.

        The default is 70.
    */
    property int numBoxWidth: 70

    /*!
        The maximum r,g and b values of the control
    */
    property real brightnessRange: 1.0

    altColorSpace: "RGB"

    ColumnLayout {
        anchors.fill: parent

        WGColorValueSlider {
            id: rSlider
            objectName: "redColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            rgbaColor: rgbSlider.rgbaColor
            altColor: rgbSlider.altColor
            altColorSpace: rgbSlider.altColorSpace
            useHDR: rgbSlider.useHDR
            tonemap: rgbSlider.tonemap
            decimalPlaces: rgbSlider.decimalPlaces
            brightnessRange: rgbSlider.brightnessRange
            numBoxWidth: rgbSlider.numBoxWidth
            useHexValue: rgbSlider.useHexValue

            primaryChannel: "x"

            sliderLabel: "R:"

            numberBoxData: useHexValue ? rDc : sliderControl

            WGDataConversion {
                id: rDc
                dataTarget: rSlider.sliderControl
                decimals: 0
                stepSize: 1
                dataToValue: function(val) {
                    return Math.floor(val * 255)
                }
                valueToData: function(val) {
                    return val / 255
                }
            }

            onChangeColor: {
                if (col != rgbSlider.rgbaColor)
                {
                    rgbSlider.changeColor(col)
                }
            }
        }

        WGColorValueSlider {
            id: gSlider
            objectName: "greenColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            rgbaColor: rgbSlider.rgbaColor
            altColor: rgbSlider.altColor
            altColorSpace: rgbSlider.altColorSpace
            useHDR: rgbSlider.useHDR
            tonemap: rgbSlider.tonemap
            decimalPlaces: rgbSlider.decimalPlaces
            brightnessRange: rgbSlider.brightnessRange
            numBoxWidth: rgbSlider.numBoxWidth
            useHexValue: rgbSlider.useHexValue

            primaryChannel: "y"

            sliderLabel: "G:"

            numberBoxData: useHexValue ? gDc : sliderControl

            WGDataConversion {
                id: gDc
                dataTarget: gSlider.sliderControl
                decimals: 0
                stepSize: 1
                dataToValue: function(val) {
                    return Math.floor(val * 255)
                }
                valueToData: function(val) {
                    return val / 255
                }
            }

            onChangeColor: {
                if (col != rgbSlider.rgbaColor)
                {
                    rgbSlider.changeColor(col)
                }
            }
        }

        WGColorValueSlider {
            id: bSlider
            objectName: "blueColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            rgbaColor: rgbSlider.rgbaColor
            altColor: rgbSlider.altColor
            altColorSpace: rgbSlider.altColorSpace
            useHDR: rgbSlider.useHDR
            tonemap: rgbSlider.tonemap
            decimalPlaces: rgbSlider.decimalPlaces
            brightnessRange: rgbSlider.brightnessRange
            numBoxWidth: rgbSlider.numBoxWidth
            useHexValue: rgbSlider.useHexValue

            primaryChannel: "z"

            sliderLabel: "B:"

            numberBoxData: useHexValue ? bDc : sliderControl

            WGDataConversion {
                id: bDc
                dataTarget: bSlider.sliderControl
                decimals: 0
                stepSize: 1
                dataToValue: function(val) {
                    return Math.floor(val * 255)
                }
                valueToData: function(val) {
                    return val / 255
                }
            }

            onChangeColor: {
                if (col != rgbSlider.rgbaColor)
                {
                    rgbSlider.changeColor(col)
                }
            }
        }
    }
}
