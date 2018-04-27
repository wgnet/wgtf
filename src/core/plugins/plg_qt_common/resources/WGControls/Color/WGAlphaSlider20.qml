import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \One WGColorValueSlider with a color gradient representing the alpha value. This value is always
 stored as a real value from 0 to 1.

 The slider updates its color gradient based on the current values.

Example:
\code{.js}
WGAlphaSlider {
    id: alphaSlider
    Layout.fillWidth: true
    rgbaColor: myVec4Color
}
\endcode
*/

WGAbstractColor {
    id: alphaSlider
    objectName: "WGAlphaSlider"
    WGComponent { type: "WGAlphaSlider20" }

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
            id: aSlider
            objectName: "alphaColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            rgbaColor: alphaSlider.rgbaColor
            altColor: alphaSlider.altColor
            altColorSpace: alphaSlider.altColorSpace
            useHDR: alphaSlider.useHDR
            tonemap: alphaSlider.tonemap
            decimalPlaces: alphaSlider.decimalPlaces
            brightnessRange: 1.0
            numBoxWidth: alphaSlider.numBoxWidth
            useHexValue: false

            primaryChannel: "w"

            sliderLabel: "A:"

            onChangeColor: {
                if (col != alphaSlider.rgbaColor)
                {
                    alphaSlider.changeColor(col)
                }
            }
        }
    }
}
