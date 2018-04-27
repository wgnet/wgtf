import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

import WGControls 2.0
import WGControls.Global 2.0
import WGControls.Private 2.0
import WGControls.Color 2.0

/*!
 \A Photoshop style Hue Saturation square.

Example:
\code{.js}
WGHSVSquareHS {
    height: 360
    width: 360

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
    id: colorSquare
    objectName: "WGHSVSquareHS"
    WGComponent { type: "WGHSVSquareHS20" }

    /*!
        The maximum v value of the control
    */
    property real brightnessRange: 1.0

    property int squareSide: {
        if (showSlider)
        {
            return Math.min(width * 0.85, colorSquare.width - defaultSpacing.minimumRowHeight - defaultSpacing.doubleBorderSize)
        }
        else {
            return width
        }
    }

    property var sliderStepSize: 1 / Math.pow(10, decimalPlaces)

    property bool showSlider: true

    signal mouseReleased()

    Component.onCompleted: {
        squareArea.updateHueSat(altColor.x, altColor.y)
    }

    RowLayout {
        id: squareLayout
        anchors.fill: parent
        spacing: 0

        Item {
            id: paletteMap

            Layout.preferredWidth: squareSide
            Layout.maximumHeight: width
            Layout.minimumHeight: width

            clip: true

            ShaderEffect {
                anchors.centerIn: parent
                width: squareSide
                height: squareSide
                layer.enabled: true
                layer.smooth: true

                property real val: altColor.z
                fragmentShader: "#version 150
                in vec2 qt_TexCoord0;
                uniform float val;

                vec3 hsv2rgb(vec3 c)
                {
                    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
                }

                void main() {
                    vec4 c = vec4(1.0);
                    c.rgb = hsv2rgb(vec3(qt_TexCoord0.s, 1.0 - qt_TexCoord0.t, val));
                    gl_FragColor = c;
                }
                "
            }

            MouseArea {
                id: squareArea
                objectName: "colorSquareDragArea"
                anchors.fill: parent

                acceptedButtons: Qt.LeftButton

                property bool dragging: false
                property var initialColor

                z: 1

                function updateHueSat(hue,sat)
                {
                    dragging = true

                    colorHandle.y = squareSide - (squareSide * sat)
                    colorHandle.x = squareSide * hue
                    dragging = false
                }

                propagateComposedEvents: false

                onPressed: {
                    beginUndoFrame();
                    initialColor = rgbaColor;
                }

                onPositionChanged: {
                    dragging = true

                    var testX = mouse.x
                    var testY = mouse.y

                    if (testX < 0)
                    {
                        testX = 0
                    }
                    else if (testX > squareSide)
                    {
                        testX = squareSide
                    }

                    if (testY < 0)
                    {
                        testY = 0
                    }
                    else if (testY > squareSide)
                    {
                        testY = squareSide
                    }

                    var newH = testX / squareSide
                    var newS = ((squareSide - testY) / squareSide)

                    if (round(newH) != round(altColor.x) || round(newS) != round(altColor.y))
                    {
                        changeAltColor(Qt.vector3d(newH, newS, altColor.z))
                    }
                    colorHandle.x = testX
                    colorHandle.y = testY
                }

                onReleased: {
                    dragging = false
                    mouseReleased()
                    if (rgbaColor != initialColor)
                    {
                        endUndoFrame();
                    }
                    else
                    {
                        abortUndoFrame();
                    }
                }

                Item {
                    id: colorHandle

                    Rectangle {
                        color: "transparent"
                        border.color: "white"
                        border.width: 2
                        width: Math.min(Math.max(((squareSide * 0.1) - defaultSpacing.standardMargin), defaultSpacing.doubleMargin),defaultSpacing.doubleMargin * 2)
                        height: width

                        radius: height
                        anchors.centerIn: parent

                        Rectangle {
                            color: "transparent"
                            border.color: "black"
                            border.width: 1
                            width: parent.width - defaultSpacing.doubleBorderSize
                            height: width
                            radius: height
                            anchors.centerIn: parent
                        }
                    }
                    Connections {
                        target: colorSquare

                        onRgbaColorChanged: {
                            if (!squareArea.dragging && squareSide > 0)
                            {
                                squareArea.updateHueSat(altColor.x, altColor.y)
                            }
                        }
                        onSquareSideChanged: {
                            if(squareSide > 0)
                            {
                                squareArea.updateHueSat(altColor.x, altColor.y)
                            }
                        }

                        onBrightnessRangeChanged: {
                            squareArea.updateHueSat(altColor.x, altColor.y)
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
            spacing: defaultSpacing.doubleBorderSize
            visible: showSlider
            Item {
                Layout.preferredHeight: squareSide
                Layout.fillWidth: true
                Layout.minimumWidth: defaultSpacing.doubleBorderSize
                Layout.maximumWidth: defaultSpacing.standardMargin
            }
            Item {
                Layout.fillWidth: true
                Layout.minimumWidth: defaultSpacing.minimumRowHeight
                Layout.preferredHeight: squareSide

                WGColorValueSlider {
                    id: brightSlider
                    objectName: "brightSlider"
                    width: squareSide
                    height: Math.min(parent.width, defaultSpacing.minimumRowHeight)

                    y: width
                    x: Math.max(((parent.width - defaultSpacing.minimumRowHeight) / 2), 0)

                    rotation: -90

                    transformOrigin: Item.TopLeft

                    rgbaColor: colorSquare.rgbaColor
                    altColor: colorSquare.altColor
                    altColorSpace: colorSquare.altColorSpace
                    useHDR: colorSquare.useHDR
                    tonemap: colorSquare.tonemap
                    decimalPlaces: colorSquare.decimalPlaces
                    brightnessRange: colorSquare.brightnessRange
                    useHexValue: false

                    primaryChannel: "z"

                    sliderLabel: "V:"

                    showLabel: false
                    showValue: false

                    onChangeColor: {
                        if (visible && col != colorSquare.rgbaColor)
                        {
                            colorSquare.changeColor(col)
                        }
                    }

                    onEndDrag: {
                        mouseReleased()
                    }
                }
            }
        }
    }
}
