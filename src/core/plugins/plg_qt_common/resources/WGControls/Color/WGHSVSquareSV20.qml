import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

import WGControls 2.0
import WGControls.Global 2.0
import WGControls.Private 2.0

/*!
 \A Photoshop style Saturation Value color square.

Example:
\code{.js}
WGHSVSquareSV {
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
    objectName: "WGHSVSquareSV"
    WGComponent { type: "WGHSVSquareSV20" }

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
        squareArea.updateSatValue(altColor.y, altColor.z)
    }

    RowLayout {
        id: squareLayout
        anchors.fill: parent
        spacing: 0

        Item {
            id: paletteMap

            Layout.minimumWidth: squareSide
            Layout.maximumWidth: squareSide
            Layout.maximumHeight: width
            Layout.minimumHeight: width

            clip: true

            property var div: 10
            property var hue
            Connections {
                target: colorSquare
                onAltColorChanged: {
                    if (paletteMap.hue != colorSquare.altColor.x) {
                        paletteMap.hue = colorSquare.altColor.x
                    }
                }
            }

            Repeater {
                model: paletteMap.div * paletteMap.div

                ShaderEffect {
                    property var invDiv: 1 / paletteMap.div
                    property var xRatio: (index % paletteMap.div) * invDiv
                    property var yRatio: (Math.floor(index / paletteMap.div)) * invDiv

                    x: squareSide * xRatio
                    y: squareSide * yRatio
                    width: squareSide * invDiv
                    height: squareSide * invDiv
                    layer.enabled: true
                    layer.smooth: true

                    visible: useHDR
                    enabled: useHDR

                    property var vColor0: tonemap(WGColor.hsvToRgb(paletteMap.hue,xRatio,(1-yRatio)*brightnessRange)) // 0,0
                    property var vColor1: tonemap(WGColor.hsvToRgb(paletteMap.hue,xRatio,(1-yRatio-invDiv)*brightnessRange)) // 0,1
                    property var vColor2: tonemap(WGColor.hsvToRgb(paletteMap.hue,xRatio + invDiv,(1-yRatio-invDiv)*brightnessRange)) // 1,1
                    property var vColor3: tonemap(WGColor.hsvToRgb(paletteMap.hue,xRatio + invDiv,(1-yRatio)*brightnessRange)) // 1,0

                    vertexShader: useHDR ? "
                        uniform highp mat4 qt_Matrix;
                        attribute highp vec4 qt_Vertex;
                        attribute highp vec2 qt_MultiTexCoord0;

                        uniform vec3 vColor0;
                        uniform vec3 vColor1;
                        uniform vec3 vColor2;
                        uniform vec3 vColor3;
                        varying vec4 vColor;

                        void main() {
                            if (qt_MultiTexCoord0.x == 0 && qt_MultiTexCoord0.y == 0) {
                                vColor = vec4(vColor0,1);
                            } else if (qt_MultiTexCoord0.x == 0 && qt_MultiTexCoord0.y == 1) {
                                vColor = vec4(vColor1,1);
                            } else if (qt_MultiTexCoord0.x == 1 && qt_MultiTexCoord0.y == 1) {
                                vColor = vec4(vColor2,1);
                            } else if (qt_MultiTexCoord0.x == 1 && qt_MultiTexCoord0.y == 0) {
                                vColor = vec4(vColor3,1);
                            }
                            gl_Position = qt_Matrix * qt_Vertex;
                        }" : ""
                        // leave blank if not using HDR

                    fragmentShader: "
                        varying vec4 vColor;

                        void main() {
                            gl_FragColor = vColor;
                        }"
                }
            }

            ShaderEffect {
                id: ldrMap
                width: squareSide
                height: squareSide
                layer.enabled: true
                layer.smooth: true
                anchors.centerIn: parent

                visible: !useHDR
                enabled: !useHDR

                property real hue: altColor.x
                property real minBr: 0
                property real maxBr: brightnessRange

                fragmentShader: useHDR ?
                    // generic shader
                    "varying vec4 vColor;

                    void main() {
                        gl_FragColor = vColor;
                    }" :

                    // HSV square shader
                    "in vec2 qt_TexCoord0;
                    uniform float hue;
                    uniform float minBr;
                    uniform float maxBr;

                    vec3 hsv2rgb(vec3 c)
                    {
                        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
                    }

                    void main() {
                        vec4 c = vec4(1.0);
                        c.rgb = hsv2rgb(vec3(hue, qt_TexCoord0.s, minBr + (1.0 - qt_TexCoord0.t) * maxBr));
                        gl_FragColor = c;
                    }"
            }

            MouseArea {
                id: squareArea
                objectName: "colorSquareDragArea"
                anchors.fill: parent

                acceptedButtons: Qt.LeftButton

                property bool dragging: false
                property var initialColor

                z: 1

                function updateSatValue(sat,value)
                {
                    dragging = true

                    colorHandle.y = squareSide - (squareSide * (value / brightnessRange))
                    colorHandle.x = squareSide * sat
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

                    var newS = testX / squareSide
                    var newV = brightnessRange * ((squareSide - testY) / squareSide)

                    if (round(newS) != round(altColor.y) || round(newV) != round(altColor.z))
                    {
                        changeAltColor(Qt.vector3d(altColor.x, newS, newV))
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
                                squareArea.updateSatValue(altColor.y, altColor.z)
                            }
                        }
                        onSquareSideChanged: {
                            if(squareSide > 0)
                            {
                                squareArea.updateSatValue(altColor.y, altColor.z)
                            }
                        }

                        onBrightnessRangeChanged: {
                            squareArea.updateSatValue(altColor.y, altColor.z)
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
                    id: hueSlider
                    objectName: "hueColorSlider"
                    width: squareSide
                    height: Math.min(parent.width, defaultSpacing.minimumRowHeight)

                    x: Math.max(((parent.width - defaultSpacing.minimumRowHeight) / 2), 0)
                    y: width

                    rotation: -90

                    transformOrigin: Item.TopLeft

                    rgbaColor: colorSquare.rgbaColor
                    altColor: colorSquare.altColor
                    altColorSpace: colorSquare.altColorSpace
                    useHDR: colorSquare.useHDR
                    tonemap: colorSquare.tonemap
                    decimalPlaces: colorSquare.decimalPlaces
                    brightnessRange: 1.0
                    useHexValue: false

                    primaryChannel: "x"

                    sliderLabel: "H:"

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
