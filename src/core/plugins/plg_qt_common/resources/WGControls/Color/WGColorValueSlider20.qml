import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Global 2.0
import WGControls.Private 2.0
import WGControls.Styles 2.0

/*!
 \A single WGColorSlider with a numberBox setup to use WGAbstractColor

Example:
\code{.js}
WGColorValueSlider {
    id: rSlider
    Layout.fillWidth: true
    rgbaColor: myVec4Color

    sliderLabel: "R"

    primaryChannel: 0

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
    id: baseSlider
    objectName: "WGColorValueSlider"
    WGComponent { type: "WGColorValueSlider20" }

    /*!
        The property determines whether the number boxes will show values from 0 to 1 or 0 to 255.

        The default is false.
    */
    property bool useHexValue: false

    /*!
        The width of the number boxes on the right hand side.

        The default is 70.
    */
    property int numBoxWidth: 70

    /*!
        The maximum value of the control
    */
    property real brightnessRange: 1.0

    /*!
        The color channel of the altColor the slider will control.

        The default is "x" (R in RGB space)
    */
    property string primaryChannel: "x"

    /*!
        The value of the primaryChannel
    */
    property real pValue: primaryChannel == "w" ? rgbaColor.w : altColor[primaryChannel]

    /*!
        The text label next to the slider.

        The default is "R:"
    */
    property string sliderLabel: "R:"

    /*!
        Whether or not to show the text label.

        The default is true
    */
    property bool showLabel: true

    /*!
        Whether or not to show the value number box.

        The default is true.
    */
    property bool showValue: true

    property QtObject numberBoxData: sliderFrame
    property QtObject sliderData: sliderFrame

    property alias sliderControl: sliderFrame

    property real stepSize: 1 / Math.pow(10, decimalPlaces)

    /*!
        This signal is fired when a handle of the slider(index) has finished dragging
    */
    signal endDrag (int index)

    RowLayout {
        anchors.fill: parent

        spacing: showLabel || showValue ? defaultSpacing.standardMargin : 0

        WGLabel {
            text: sliderLabel
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: showLabel ? defaultSpacing.doubleMargin : 0
            visible: showLabel
        }

        WGSliderControl {
            id: sliderFrame
            minimumValue: 0
            maximumValue: brightnessRange

            Layout.fillWidth: true
            Layout.fillHeight: true

            value: pValue

            showValue: baseSlider.showValue

            decimals: decimalPlaces

            stepSize: baseSlider.stepSize

            sliderData: baseSlider.sliderData
            numberBoxData: baseSlider.numberBoxData

            valueBoxWidth: numBoxWidth

            onChangeValue: {
                if (round(val) != round(pValue))
                {
                    var xval = primaryChannel == "x" ? val : altColor.x
                    var yval = primaryChannel == "y" ? val : altColor.y
                    var zval = primaryChannel == "z" ? val : altColor.z
                    var wval = primaryChannel == "w" ? val : rgbaColor.w

                    if (primaryChannel == "w")
                    {
                        changeColor(Qt.vector4d(rgbaColor.x, rgbaColor.y, rgbaColor.z, wval))
                    }
                    else
                    {
                        changeAltColor(Qt.vector3d(xval, yval, zval))
                    }
                }
            }

            slider: WGColorSlider {
                id: colSlider
                objectName: "colorSlider"

                stepSize: typeof sliderFrame.sliderData.stepSize != "undefined" ? sliderFrame.sliderData.stepSize : sliderFrame.stepSize

                value: typeof sliderFrame.sliderData.value != "undefined" ? sliderFrame.sliderData.value : sliderFrame.value

                multipleValues: sliderFrame.multipleValues

                minimumValue: typeof sliderFrame.sliderData.minimumValue != "undefined" ? sliderFrame.sliderData.minimumValue : sliderFrame.minimumValue
                maximumValue: typeof sliderFrame.sliderData.maximumValue != "undefined" ? sliderFrame.sliderData.maximumValue : sliderFrame.maximumValue

                anchors.fill: parent
                Layout.preferredHeight: __horizontal ? Math.round(sliderFrame.height) : -1

                activeFocusOnPress: true

                orientation: sliderFrame.orientation

                tickmarksEnabled: sliderFrame.tickmarksEnabled
                tickmarkInterval: sliderFrame.tickmarkInterval
                showTickmarkLabels: sliderFrame.showTickmarkLabels
                customTickmarkLabels: sliderFrame.customTickmarkLabels
                customTickmarks: sliderFrame.customTickmarks

                onEndDrag: {
                    baseSlider.endDrag(index)
                }

                onValueChanged: {
                    if (typeof sliderFrame.sliderData.stepSize != "undefined")
                    {
                        if (round(sliderFrame.sliderData.value) != round(colSlider.value))
                        {
                            sliderFrame.sliderData.changeValue(colSlider.value)
                        }
                    }
                }

                states: [
                    State {
                        name: ""
                        when: sliderFrame.width < sliderValue.Layout.preferredWidth + sliderHandle.width
                        PropertyChanges {target: slider; opacity: 0}
                        PropertyChanges {target: sliderLayout; spacing: 0}
                        PropertyChanges {target: slider; visible: false}
                    },
                    State {
                        name: "HIDDENSLIDER"
                        when: sliderFrame.width >= sliderValue.Layout.preferredWidth + sliderHandle.width
                        PropertyChanges {target: slider; opacity: 1}
                        PropertyChanges {target: sliderLayout; spacing: defaultSpacing.rowSpacing}
                        PropertyChanges {target: slider; visible: true}
                    }
                ]

                transitions: [
                    Transition {
                        from: ""
                        to: "HIDDENSLIDER"
                        NumberAnimation { properties: "opacity"; duration: 200 }
                    },
                    Transition {
                        from: "HIDDENSLIDER"
                        to: ""
                        NumberAnimation { properties: "opacity"; duration: 200 }
                    }
                ]

                function calcColor(pos) {
                    var xval = primaryChannel == "x" ? pos * brightnessRange : altColor.x
                    var yval = primaryChannel == "y" ? pos * brightnessRange : altColor.y
                    var zval = primaryChannel == "z" ? pos * brightnessRange : altColor.z
                    var wval = primaryChannel == "w" ? pos * brightnessRange : rgbaColor.w

                    var newCol = Qt.vector3d(xval,yval,zval)

                    if (altColorSpace == "HSV")
                    {
                        newCol = WGColor.hsvToRgb(newCol.x,newCol.y,newCol.z)
                    }
                    else if (altColorSpace == "HSL")
                    {
                        newCol = WGColor.hslToRgb(newCol.x,newCol.y,newCol.z)
                    }

                    newCol = tonemap(Qt.vector3d(newCol.x,newCol.y,newCol.z))

                    if (primaryChannel == "w")
                    {
                        return Qt.rgba(newCol.x,newCol.y,newCol.z,wval)
                    }
                    else
                    {
                        return Qt.rgba(newCol.x,newCol.y,newCol.z,1)
                    }
                }

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: colSlider.calcColor(0)
                    }
                    GradientStop {
                        position: 0.1
                        color: colSlider.calcColor(0.1)
                    }
                    GradientStop {
                        position: 0.2
                        color: colSlider.calcColor(0.2)
                    }
                    GradientStop {
                        position: 0.3
                        color: colSlider.calcColor(0.3)
                    }
                    GradientStop {
                        position: 0.4
                        color: colSlider.calcColor(0.4)
                    }
                    GradientStop {
                        position: 0.5
                        color: colSlider.calcColor(0.5)
                    }
                    GradientStop {
                        position: 0.6
                        color: colSlider.calcColor(0.6)
                    }
                    GradientStop {
                        position: 0.7
                        color: colSlider.calcColor(0.7)
                    }
                    GradientStop {
                        position: 0.8
                        color: colSlider.calcColor(0.8)
                    }
                    GradientStop {
                        position: 0.9
                        color: colSlider.calcColor(0.9)
                    }
                    GradientStop {
                        position: 1
                        color: colSlider.calcColor(1)
                    }
                }
            }
        }
    }
}
