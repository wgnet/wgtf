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
WGKelvinSlider {
    id: kSlider
    useHDR: true
    rgbaColor: kelvinColor // (R,G,B,K)
    Layout.preferredHeight: defaultSpacing.minimumRowHeight
    Layout.fillWidth: true

    onChangeColor: {
        kelvinColor = col // (R,G,B,K)
    }
}
\endcode
*/

Item {
    id: baseSlider
    objectName: "WGKelvinSlider"
    WGComponent { type: "WGKelvinSlider20" }

    /*!
        The width of the number boxes on the right hand side.

        The default is 70.
    */
    property int numBoxWidth: 70

    /*!
        The maximum r,g and b values of the control
    */
    property real brightnessRange: 1.0

    property bool showLabel: true
    property bool showValue: true

    property int value: 1000

    property alias sliderControl: sliderFrame

    property real minimumValue: 0
    property real maximumValue: 40000
    property bool multipleValues: false

    property real stepSize: 1
    property int decimals: 0

    property var sliderGradient: Gradient {}

    // approximate kelvin (int kelvin) function to rgb (vector3d(r,g,b))
    property var kelvinToRgb: function(kelvin) {

        var color = Qt.vector4d(0,0,0)

        // Assumed values are within 1000 to 40000 kelvin or this gets inaccurate
        kelvin = Math.max(1000 ,Math.min(40000, kelvin))
        kelvin /= 100

        //get R
        var tmpR
        if (kelvin <= 66)
        {
            tmpR = 255
        }
        else
        {
            tmpR = kelvin - 60
            tmpR = 329.698727446 * Math.pow(tmpR,-0.1332047592)
            tmpR = Math.max(0,Math.min(255, tmpR))
        }
        color.x = tmpR / 255

        //get G
        var tmpG
        if (kelvin <= 66)
        {
            tmpG = kelvin
            tmpG = 99.4708025861 * Math.log(tmpG) - 161.1195681661
        }
        else
        {
            tmpG = kelvin - 60
            tmpG = 288.1221695283 * Math.pow(tmpG,-0.0755148492)
        }
        tmpG = Math.max(0,Math.min(255, tmpG))
        color.y = tmpG / 255

        //get B
        var tmpB
        if (kelvin >= 66)
        {
            tmpB = 255
        }
        else if (kelvin <= 19)
        {
            tmpB = 0
        }
        else
        {
            tmpB = kelvin - 10
            tmpB = 138.5177312231 * Math.log(tmpB) - 305.0447927307
        }

        tmpB = Math.max(0,Math.min(255, tmpB))

        color.z = tmpB / 255

        return color;
    }

    signal changeValue(var val)

    // creates the gradient for the colorSlider based on the min/max values
    Component.onCompleted: {
        var stops = 20
        var grad3 = [0,0,0]
        var gradString = "import QtQuick 2.4; Gradient { "
        for(var i=0; i<=stops; i++)
        {
            grad3 = kelvinToRgb(minimumValue + (((maximumValue - minimumValue) / stops) * i))
            gradString += "GradientStop { position: " + i / stops + "; color: Qt.rgba(" + grad3.x + "," + grad3.y + "," + grad3.z + ",1)} "
        }
        gradString += " }"
        var newGradient = Qt.createQmlObject(gradString,baseSlider)
        baseSlider.sliderGradient = newGradient
    }

    RowLayout {
        anchors.fill: parent
        spacing: showLabel || showValue ? defaultSpacing.standardMargin : 0

        WGSliderControl {
            id: sliderFrame

            // maybe min value should be 1000... <500 or so is infrared and doesn't really make much sense.
            minimumValue: baseSlider.minimumValue

            // 12,000 - 20,000K is 'sky' light but it doesn't change much after that before ultraviolet
            maximumValue: baseSlider.maximumValue

            Layout.fillWidth: true
            Layout.fillHeight: true

            value: baseSlider.value

            showValue: true

            decimals: baseSlider.decimals

            stepSize: baseSlider.stepSize

            multipleValues: baseSlider.multipleValues

            onChangeValue: {
                if (val != baseSlider.value)
                {
                    baseSlider.changeValue(val)
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

                onValueChanged: {
                    if (typeof sliderFrame.sliderData.stepSize != "undefined")
                    {
                        if (sliderFrame.sliderData.value != colSlider.value)
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

                gradient: baseSlider.sliderGradient
            }
        }
    }
}
