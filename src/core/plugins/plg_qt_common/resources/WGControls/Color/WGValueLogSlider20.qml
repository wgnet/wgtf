import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \One WGColorValueSlider with a color gradient representing the value in a logarithmic scale.

 The slider updates its color gradient based on the current values.

Example:
\code{.js}
WGValueLogSlider {
    id: alphaSlider
    Layout.fillWidth: true
    rgbaColor: myVec4Color
}
\endcode
*/


WGAbstractColor {
    id: valSlider
    objectName: "WGValueLogSlider"
    WGComponent { type: "WGValueLogSlider20" }
    altColorSpace: "HSV"

    property real brightnessRange: 1.0

    signal changeBrightness (var br)

    ColumnLayout {
        anchors.fill: parent

        WGSliderControl {
            id: valueSlider

            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            minimumValue: 0
            maximumValue: 100
            stepSize: 0.1
            decimals: 3
            value: altColor.z

            sliderData: vDc

            WGDataConversion {
                id: vDc
                dataTarget: valueSlider
                stepSize: 0.1
                decimals: 1
                minimumValue: 0
                maximumValue: 10000

                round: function round(num) {
                    return Number(Math.round(num+'e'+decimals)+'e-'+decimals);
                }

                dataToValue: function(val) {
                    var result = 0
                    if (val <= 1)
                    {
                        result =  val * 3333
                    }
                    else if (val > 1 && val <= 2)
                    {
                        result = (val * 666.6) + 2666.4
                    }
                    else if (val > 2 && val <= 10)
                    {
                        result = val * 333.3 + 3333
                    }
                    else if (val > 10 && val <= 20)
                    {
                        result = (val * 66.66) + 5999.4
                    }
                    else
                    {
                        result = val*33.34 + 6666
                    }
                    return round(result)
                }
                valueToData: function(val) {
                    var result = 0
                    if (val <= 3333)
                    {
                        result = val / 3333
                    }
                    else if (val > 3333 && val <= 3333 + 333.3)
                    {
                        result = (val - 2666.4) / 666.6
                    }
                    else if (val > 3333 + 333.3 && val <= 6666)
                    {
                        result = (val-3333)/333.3
                    }
                    else if (val > 6666 && val <= 6666 + 666.6)
                    {
                        result = (val - 5999.4) / 66.66
                    }
                    else
                    {
                        result = (val-6666)/33.34
                    }
                    return round(result)
                }
            }

            customTickmarks: [0, 3333, 6666, 10000]
            customTickmarkLabels: ["0", "1", "10", "100"]
            showTickmarkLabels: true
            stickyValues: [0, 3333, 6666, 10000]
            stickyMargin: 10

            onChangeValue: {
                if (valueSlider.round(altColor.z) != valueSlider.round(val))
                {
                    valSlider.changeAltColor(Qt.vector3d(altColor.x, altColor.y, val))
                    if (val <= 1.0)
                    {
                        changeBrightness(1.0)
                    }
                    else if ((val > 1.0 && val <= 10.0))
                    {
                        changeBrightness(10.0)
                    }
                    else
                    {
                        changeBrightness(100.0)
                    }
                }
            }
        }
    }
}
