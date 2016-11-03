import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGColorPicker 2.0

/*!
 \Three WGColorSliders with color gradients representing Hue, Saturation and Lightness.

 The Saturation and Lightness sliders update their color gradients based on the current values.

Example:
\code{.js}
WGHslSlider {
    id: hslPicker
    Layout.fillWidth: true
    myColor = Qt.hsla(hueVal, satVal, lightVal, 1.0)
}
\endcode
*/

ColumnLayout {
    id: hslSlider
    WGComponent { type: "WGHslSlider" }

    /*!
        The value of the hue slider from 0 to 1.0
    */
    property real hueVal

    /*!
        The value of the saturation slider from 0 to 1.0
    */
    property real satVal

    /*!
        The value of the lightness slider from 0 to 1.0
    */
    property real lightVal

    /*!
        The width of the number boxes on the right hand side.

        The default is 70.
    */
    property int numBoxWidth: 70

    onHueValChanged: {
        hSlider.changeValue(hslSlider.hueVal, 0)
        hBox.value = hslSlider.hueVal
    }
    onSatValChanged: {
        sSlider.changeValue(hslSlider.satVal, 0)
        sBox.value = hslSlider.satVal
    }
    onLightValChanged: {
        lSlider.changeValue(hslSlider.lightVal, 0)
        lBox.value = hslSlider.lightVal
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "H:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: hSlider
            objectName: "hueColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: Qt.rgba(1,0,0,1)
                }
                GradientStop {
                    position: 0.167
                    color: Qt.rgba(1,1,0,1)
                }
                GradientStop {
                    position: 0.333
                    color: Qt.rgba(0,1,0,1)
                }
                GradientStop {
                    position: 0.5
                    color: Qt.rgba(0,1,1,1)
                }
                GradientStop {
                    position: 0.667
                    color: Qt.rgba(0,0,1,1)
                }
                GradientStop {
                    position: 0.833
                    color: Qt.rgba(1,0,1,1)
                }
                GradientStop {
                    position: 1
                    color: Qt.rgba(1,0,0,1)
                }
            }

            value: hslSlider.hueVal

            onValueChanged: {
                if (value != hslSlider.hueVal)
                {
                    hslSlider.hueVal = value
                }
            }
        }

        WGNumberBox {
            id: hBox
            objectName: "hueColorValue"
            Layout.preferredWidth: numBoxWidth
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.01
            decimals: 5
            value: hslSlider.hueVal
            onValueChanged: {
                if (value != hslSlider.hueVal)
                {
                    hslSlider.hueVal = value
                }
            }
        }
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "S:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: sSlider
            objectName: "satColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: Qt.hsla(hueValue,0,lightVal,1)
                }
                GradientStop {
                    position: 1
                    color: Qt.hsla(hueValue,1,lightVal,1)
                }
            }

            value: hslSlider.satVal

            onValueChanged: {
                if (value != hslSlider.satVal)
                {
                    hslSlider.satVal = value
                }
            }
        }

        WGNumberBox {
            id: sBox
            objectName: "satColorValue"
            Layout.preferredWidth: numBoxWidth
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.01
            decimals: 5
            value: hslSlider.satVal
            onValueChanged: {
                if (value != hslSlider.satVal)
                {
                    hslSlider.satVal = value
                }
            }
        }
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "L:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: lSlider
            objectName: "lightColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            value: hslSlider.lightVal

            onValueChanged: {
                if (value != hslSlider.lightVal)
                {
                    hslSlider.lightVal = value
                }
            }
        }

        WGNumberBox {
            id: lBox
            objectName: "lightColorValue"
            Layout.preferredWidth: numBoxWidth
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.01
            decimals: 5
            value: hslSlider.lightVal
            onValueChanged: {
                if (value != hslSlider.lightVal)
                {
                    hslSlider.lightVal = value
                }
            }
        }
    }

}
