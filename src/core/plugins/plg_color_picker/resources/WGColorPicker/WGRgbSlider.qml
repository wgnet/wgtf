import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGControls 2.0
import WGColorPicker 1.0

/*!
 \Three WGColorSliders with color gradients representing Red, Green and Blue color values. These values are always
 stored as real values from 0 to 1 but can be displayed as 0 to 255 hex values if useHexValue = true.

 The sliders update their color gradients based on the current values.

Example:
\code{.js}
WGRgbSlider {
    id: rgbPicker
    Layout.fillWidth: true
    myColor = Qt.rgba(redVal, greenVal, blueVal, 1.0)
}
\endcode
*/

ColumnLayout {
    id: rgbSlider

    /*!
        The value of the red slider from 0 to 1
    */
    property real redVal

    /*!
        The value of the green slider from 0 to 1
    */
    property real greenVal

    /*!
        The value of the blue slider from 0 to 1
    */
    property real blueVal

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

    onRedValChanged: {
        if (useHexValue)
        {
            rSlider.changeValue(rgbSlider.redVal * 255, 0)
            rBox.value = rgbSlider.redVal * 255
        }
        else
        {
            rSlider.changeValue(rgbSlider.redVal, 0)
            rBox.value = rgbSlider.redVal
        }
    }
    onGreenValChanged: {
        if (useHexValue)
        {
            gSlider.changeValue(rgbSlider.greenVal * 255, 0)
            gBox.value = rgbSlider.greenVal * 255
        }
        else
        {
            gSlider.changeValue(rgbSlider.greenVal, 0)
            gBox.value = rgbSlider.greenVal
        }
    }
    onBlueValChanged: {
        if (useHexValue)
        {
            bSlider.changeValue(rgbSlider.blueVal * 255, 0)
            bBox.value = rgbSlider.blueVal * 255
        }
        else
        {
            bSlider.changeValue(rgbSlider.blueVal, 0)
            bBox.value = rgbSlider.blueVal
        }
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "R:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: rSlider
            objectName: "redColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: useHexValue ? 255 : 1
            stepSize: useHexValue ? 1 : 0.001

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: Qt.rgba(0,rgbSlider.greenVal,rgbSlider.blueVal,1)
                }
                GradientStop {
                    position: 1
                    color: Qt.rgba(1,rgbSlider.greenVal,rgbSlider.blueVal,1)
                }
            }

            value: useHexValue ? rgbSlider.redVal * 255 : rgbSlider.redVal

            onValueChanged: {
                if (useHexValue)
                {
                    if (value != rgbSlider.redVal * 255)
                    {
                        rgbSlider.redVal = value / 255
                    }
                }
                else
                {
                    if (value != rgbSlider.redVal)
                    {
                        rgbSlider.redVal = value
                    }
                }
            }
        }

        WGNumberBox {
            id: rBox
            objectName: "redColorValue"
            Layout.preferredWidth: numBoxWidth
            minimumValue: 0
            maximumValue: useHexValue ? 255 : 1
            stepSize: useHexValue ? 1 : 0.01
            decimals: useHexValue ? 0 : 5
            value: useHexValue ? rgbSlider.redVal * 255 : rgbSlider.redVal
            onValueChanged: {
                if (useHexValue)
                {
                    if (value != rgbSlider.redVal * 255)
                    {
                        rgbSlider.redVal = value / 255
                    }
                }
                else
                {
                    if (value != rgbSlider.redVal)
                    {
                        rgbSlider.redVal = value
                    }
                }
            }
        }
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "G:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: gSlider
            objectName: "greenColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: useHexValue ? 255 : 1
            stepSize: useHexValue ? 1 : 0.001

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: Qt.rgba(rgbSlider.redVal,0,rgbSlider.blueVal,1)
                }
                GradientStop {
                    position: 1
                    color:Qt.rgba(rgbSlider.redVal,1,rgbSlider.blueVal,1)
                }
            }

            value: useHexValue ? rgbSlider.greenVal * 255 : rgbSlider.greenVal

            onValueChanged: {
                if (useHexValue)
                {
                    if (value != rgbSlider.greenVal * 255)
                    {
                        rgbSlider.greenVal = value / 255
                    }
                }
                else
                {
                    if (value != rgbSlider.greenVal)
                    {
                        rgbSlider.greenVal = value
                    }
                }
            }
        }

        WGNumberBox {
            id: gBox
            objectName: "greenColorValue"
            Layout.preferredWidth: numBoxWidth
            minimumValue: 0
            maximumValue: useHexValue ? 255 : 1
            stepSize: useHexValue ? 1 : 0.01
            decimals: useHexValue ? 0 : 5
            value: useHexValue ? rgbSlider.greenVal * 255 : rgbSlider.greenVal
            onValueChanged: {
                if (useHexValue)
                {
                    if (value != rgbSlider.greenVal * 255)
                    {
                        rgbSlider.greenVal = value / 255
                    }
                }
                else
                {
                    if (value != rgbSlider.greenVal)
                    {
                        rgbSlider.greenVal = value
                    }
                }
            }
        }
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "B:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: bSlider
            objectName: "blueColorSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: useHexValue ? 255 : 1
            stepSize: useHexValue ? 1 : 0.001

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: Qt.rgba(rgbSlider.redVal,rgbSlider.greenVal,0,1)
                }
                GradientStop {
                    position: 1
                    color: Qt.rgba(rgbSlider.redVal,rgbSlider.greenVal,1,1)
                }
            }

            value: useHexValue ? rgbSlider.blueVal * 255 : rgbSlider.blueVal

            onValueChanged: {
                if (useHexValue)
                {
                    if (value != rgbSlider.blueVal * 255)
                    {
                        rgbSlider.blueVal = value / 255
                    }
                }
                else
                {
                    if (value != rgbSlider.blueVal)
                    {
                        rgbSlider.blueVal = value
                    }
                }
            }
        }

        WGNumberBox {
            id: bBox
            objectName: "blueColorValue"
            Layout.preferredWidth: numBoxWidth
            minimumValue: 0
            maximumValue: useHexValue ? 255 : 1
            stepSize: useHexValue ? 1 : 0.01
            decimals: useHexValue ? 0 : 5
            value: useHexValue ? rgbSlider.blueVal * 255 : rgbSlider.blueVal
            onValueChanged: {
                if (useHexValue)
                {
                    if (value != rgbSlider.blueVal * 255)
                    {
                        rgbSlider.blueVal = value / 255
                    }
                }
                else
                {
                    if (value != rgbSlider.blueVal)
                    {
                        rgbSlider.blueVal = value
                    }
                }
            }
        }
    }

}
