import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0


import WGControls 1.0
import WGColorPicker 1.0

/*!
 \A Corel Painter style HSL color triangle/wheel

Example:
\code{.js}
WGColorWheel {
    height: 360
    width: 360
}
\endcode
*/


Rectangle {
    id: colorWheel
    objectName: "WGColorWheel"

    color: palette.mainWindowColor

    /*!
        The hue value or hue of the chosen color from 0 to 1.0
    */
    property real hue: 0
    /*!
        The saturation ("amount" of color) of the chosen color from 0 to 1.0
    */
    property real saturation: 0
    /*!
        The lightness (how much black or white) of the chosen color from 0 to 1.0
    */
    property real lightness: 0

    /*!
        The currently chosen color.
    */
    property color wheelColor: Qt.hsla(hue,saturation,lightness,1)

    /*!
        The width of the hue ring around the color triangle.
        Changing this also affects the size of the drag handles and
        the margin of the triangle inside the wheel.

        The default value is \c width/8
    */
    property int colorWheelWidth: width/8

    /*!
        Shows circle buttons outside the hue ring to quickly
        select primary, secondary and greyscale colors.

        The default value is \c true
    */
    property bool showShortCuts: true

    /*! \internal */
    property int __shortCutButtonSize: colorWheel.width / 16

    /*! \internal */
    property int __hueRotation: hue * 360

    property bool __allowUpdating: true

    property int triangleHeight: height - (colorWheelWidth * 2.5) - defaultSpacing.doubleBorderSize
    property int triangleWidth: Math.sqrt(3)/2 * triangleHeight
    property int triangleOffset: triangleWidth / 2

    signal updateHSL(real h, real s, real l)

    onUpdateHSL: {
        __allowUpdating = false
        hue = h
        saturation = s
        lightness = l
        __allowUpdating = true
        updateColor()
    }

    function updateColor()
    {
        if(__allowUpdating)
        {
            wheelColor = Qt.hsla(hue,saturation,lightness,1)
        }
    }

    //keeps hue within bounds
    onHueChanged: {
        if (hue > 1.0)
        {
            hue -= 1.0
        }
        else if (hue < 0)
        {
            hue = 1.0 + hue
        }
        updateColor()
    }

    onSaturationChanged: {
        updateColor()
    }
    onLightnessChanged: {
        updateColor()
    }

    Rectangle {
        id: ringBorder
        anchors.fill: parent
        anchors.margins: defaultSpacing.doubleBorderSize
        height: parent.height
        width: height

        color: palette.darkColor

        radius: width

        Rectangle {
            id: colorRing
            radius: width

            anchors.centerIn: parent
            height: parent.height - defaultSpacing.doubleBorderSize * 2
            width: height

            ConicalGradient {
                id: rainbow
                angle: __hueRotation
                source: colorRing
                anchors.fill: parent

                //Makes 90 degrees 'zero'
                rotation: 90

                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#FF0000"}
                    GradientStop { position: 0.167; color: "#FF00FF"}
                    GradientStop { position: 0.333; color: "#0000FF"}
                    GradientStop { position: 0.5; color: "#00FFFF"}
                    GradientStop { position: 0.667; color: "#00FF00"}
                    GradientStop { position: 0.833; color: "#FFFF00"}
                    GradientStop { position: 1.0; color: "#FF0000"}
                }
            }

            Rectangle {
                id: hueSelectorMark
                color: "transparent"
                border.color: "white"
                border.width: 2
                width: (colorWheelWidth - defaultSpacing.doubleBorderSize) / 2
                height: width
                radius: height

                z: 1

                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: defaultSpacing.standardBorderSize

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

            MouseArea {
                id: ringDrag
                objectName: "hueRingDragWheel"
                anchors.fill: parent

                property real initOffset: 0
                property real initRotation: 0
                property real currRotation: 0

                //Gets the mouse location around the circle in degrees.
                function getMouseDegrees(mx, my, object) {
                    var point =  mapToItem (object, mx, my);
                    var diffX = (point.x - object.width/2);
                    var diffY = -1 * (point.y - object.height/2);

                    if (diffX == 0 && diffY < 0)
                    {
                        return 180
                    }
                    else if (diffX == 0 && diffY > 0)
                    {
                        return 0
                    }
                    else if (diffX < 0 && diffY == 0)
                    {
                        return 270
                    }
                    else if (diffX > 0 && diffY == 0)
                    {
                        return 90
                    }
                    else if (diffX == 0 && diffY == 0)
                    {
                        return 0
                    }

                    var rad = Math.atan (diffY / diffX);
                    var deg = (rad * 180 / Math.PI);
                    if (diffX > 0 && diffY > 0) {
                        return 90 - Math.abs (deg);
                    }
                    else if (diffX > 0 && diffY < 0) {
                        return 90 + Math.abs (deg);
                    }
                    else if (diffX < 0 && diffY > 0) {
                        return 270 + Math.abs (deg);
                    }
                    else if (diffX < 0 && diffY < 0) {
                        return 270 - Math.abs (deg);
                    }
                }

                //change the hue on mouse wheel
                onWheel: {
                    if (wheel.angleDelta.y > 0)
                    {
                        hue += 0.05
                    }
                    else
                    {
                        hue -= 0.05
                    }
                }

                //set the initial values before rotating
                onPressed: {
                    //check mouse is not inside the wheel
                    var xVal = mouse.x - colorWheelWidth / 2
                    var yVal = mouse.y - colorWheelWidth / 2

                    xVal = (xVal - (height - colorWheelWidth)/2) * (xVal - (height - colorWheelWidth)/2)
                    yVal = (yVal - (width - colorWheelWidth)/2) * (yVal - (width - colorWheelWidth)/2)

                    var innerRad = Math.sqrt(xVal + yVal)

                    if (innerRad < (height - colorWheelWidth)/2)
                    {
                        mouse.accepted = false
                    }

                    initRotation = __hueRotation
                    initOffset = getMouseDegrees(mouse.x, mouse.y, ringDrag)
                }

                //jump to color on double click
                onDoubleClicked: {
                    var deg = getMouseDegrees(mouse.x, mouse.y, ringDrag)
                    deg -= 90
                    deg = initRotation - deg
                    if (deg < 0)
                    {
                        deg += 360
                    }
                    if (deg > 360)
                    {
                        deg -= 360
                    }
                    hue = deg  / 360
                }

                //rotate the hue wheel
                onPositionChanged: {
                    currRotation = getMouseDegrees(mouse.x, mouse.y, ringDrag)
                    hue = (initRotation + (currRotation - initOffset)) / 360
                }
            }

            Rectangle {
                id: paletteMap

                anchors.centerIn: parent

                //inner circle that obscures the rainbow gradient to form a ring
                width: parent.width - colorWheelWidth
                height: width

                radius: width

                color: palette.mainWindowColor

                clip: true

                MouseArea {
                    id: triangleArea
                    objectName: "colorTriangleDragArea"
                    anchors.top: map.top
                    anchors.bottom: map.bottom
                    anchors.left: map.left

                    acceptedButtons: Qt.LeftButton

                    property bool dragging: false

                    z: 1

                    //make sure the width is setup to give an equilateral triangle
                    width: triangleWidth

                    function getCurrentWidth(yVal)
                    {
                        if (yVal <= triangleHeight / 2)
                        {
                            return yVal * ((triangleWidth / triangleHeight) * 2)
                        }
                        else
                        {
                            return ((triangleHeight - yVal) * ((triangleWidth / triangleHeight) * 2))
                        }
                    }

                    function updateSatLight(sat,light)
                    {
                        dragging = true
                        colorHandle.y = triangleHeight * (1 - lightness)
                        if (lightness == 1)
                        {
                            colorHandle.x = 0.0
                        }
                        else if (lightness == 0)
                        {
                            colorHandle.x = 0.0
                        }
                        else
                        {
                            colorHandle.x =  saturation * triangleArea.getCurrentWidth(colorHandle.y)
                        }
                        dragging = false
                    }

                    propagateComposedEvents: false

                    //ignore dragging if the user clicks outside the triangle to begin with
                    onPressed: {
                        if (mouse.y <= triangleHeight / 2)
                        {
                            if (mouse.x > mouse.y * ((triangleWidth / triangleHeight) * 2))
                            {
                                mouse.accepted = false
                            }
                        }

                        else
                        {
                            if (mouse.x > triangleWidth - ((((mouse.y - triangleHeight/2) / triangleHeight)*2) * triangleWidth))
                            {
                                mouse.accepted = false
                            }
                        }
                    }

                    onPositionChanged: {
                        dragging = true
                        var testX = mouse.x
                        var testY = mouse.y

                        //This function keeps the x value inside the visible triangle. The y is just kept inside the square.

                        //Check if the mouse is in the top or bottom half of the square

                        if (testY <= triangleHeight / 2)
                        {
                            //Check if the mouse is inside the triangle area of that half
                            if (testX <= testY * ((triangleWidth / triangleHeight) * 2))
                            {
                                colorHandle.y = testY
                                colorHandle.x = testX
                            }
                            else
                            {
                                colorHandle.y = testY
                                colorHandle.x = testY * ((triangleWidth / triangleHeight) * 2)
                            }
                        }

                        else
                        {
                            //Check if the mouse is inside the triangle area of that half
                            if (testX < triangleWidth - ((((testY - triangleHeight/2) / triangleHeight)*2) * triangleWidth))
                            {
                                colorHandle.y = testY
                                colorHandle.x = testX
                            }
                            else
                            {
                                colorHandle.y = testY
                                colorHandle.x = triangleWidth - ((((testY- triangleHeight/2) / triangleHeight)*2) * triangleWidth)
                            }
                        }

                        //Check that the coordinates are still inside the original bounds
                        if (colorHandle.x <= 0)
                        {
                            colorHandle.x = 0
                        }

                        else if (colorHandle.x >= triangleWidth)
                        {
                            colorHandle.x = triangleWidth
                            colorHandle.y = triangleHeight/2
                        }

                        if (colorHandle.y <= 0)
                        {
                            colorHandle.y = 0
                        }
                        else if (colorHandle.y >= triangleHeight)
                        {
                            colorHandle.y = triangleHeight
                        }

                        // This a helper to make it easier to put the handle in the extreme right corner of the triangle.
                        if (testX > triangleWidth + 5 && testY > triangleHeight/2 - 3 && testY < triangleHeight/2 + 3)
                        {
                            colorHandle.x = triangleWidth
                            colorHandle.y = triangleHeight/2
                        }

                        // This is saturation calculated as a polar coordinate. Incorrect for HSL but useful
                        // if we want to change the model to HSV so leaving it here.
                        /*
                        var satDeg

                        if (colorHandle.y > 0 && colorHandle.y < triangleHeight)
                        {
                            satDeg = (Math.atan((triangleHeight - colorHandle.y) / colorHandle.x) * (180 / Math.PI))
                        }
                        else
                        {
                            satDeg = 90
                        }

                        var rounding = satDeg - Math.round(satDeg)
                        if (rounding < 0.01)
                        {
                            satDeg = Math.round(satDeg)
                        }

                        satDeg = 1 - ((satDeg - 30) / 60)

                        saturation = satDeg
                        */

                        if (colorHandle.y == 0 || colorHandle.x == triangleWidth)
                        {
                            saturation = 1.0
                        }
                        else if (colorHandle.y == triangleHeight)
                        {
                            saturation = 0.0
                        }
                        else
                        {
                            saturation = colorHandle.x / triangleArea.getCurrentWidth(colorHandle.y)
                        }

                        lightness = 1 - (colorHandle.y / triangleHeight)

                    }

                    onReleased: {
                        dragging = false
                    }

                    Item {
                        id: colorHandle

                        Rectangle {
                            color: "transparent"
                            border.color: "white"
                            border.width: 2
                            width: (colorWheelWidth - defaultSpacing.doubleBorderSize) / 2
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
                            target: colorWheel
                            onSaturationChanged: {
                                if (!triangleArea.dragging && (triangleHeight > 0))
                                {
                                    triangleArea.updateSatLight(saturation, lightness)
                                }
                            }
                            onLightnessChanged: {
                                if (!triangleArea.dragging && (triangleHeight > 0))
                                {
                                    triangleArea.updateSatLight(saturation, lightness)
                                }
                            }
                            onTriangleHeightChanged: {
                                if(triangleHeight > 0)
                                {
                                    triangleArea.updateSatLight(saturation, lightness)
                                }
                            }
                        }
                    }
                }

                // Shader to draw the HSL triangle.
                ShaderEffect {
                    id: map
                    width: triangleWidth
                    height: triangleHeight
                    layer.enabled: true
                    layer.smooth: true
                    anchors.verticalCenter: parent.verticalCenter
                    //anchors.right: parent.right
                    x: triangleOffset - (colorWheelWidth / 2) - 2

                    //set the points of the triangle
                    property var p1: Qt.vector2d(0, 0);
                    property var p2: Qt.vector2d(triangleWidth, height / 2);
                    property var p3: Qt.vector2d(0, height);

                    property real chroma: hue

                    // set the color of the points of the triangle and let the graphics card blend the values

                    fragmentShader: "
                    in vec2 qt_TexCoord0;
                    varying vec4 vColor;

                    void main() {
                        gl_FragColor = vColor;
                    }
                    "

                    // fancy vertex shader that draws a triangle... but probably isn't exactly right.

                    vertexShader: "
                    attribute highp vec4 qt_Vertex;
                    attribute highp vec2 qt_MultiTexCoord0;
                    varying highp vec2 qt_TexCoord0;
                    uniform highp mat4 qt_Matrix;
                    uniform highp vec2 p1;
                    uniform highp vec2 p2;
                    uniform highp vec2 p3;

                    uniform highp float chroma;

                    varying vec4 vColor;

                    float hueToIntensity(float v1, float v2, float h) {
                        h = fract(h);
                        if (h < 1.0 / 6.0)
                            return v1 + (v2 - v1) * 6.0 * h;
                        else if (h < 1.0 / 2.0)
                            return v2;
                        else if (h < 2.0 / 3.0)
                            return v1 + (v2 - v1) * 6.0 * (2.0 / 3.0 - h);

                        return v1;
                    }

                    vec3 HSLtoRGB(vec3 color) {
                        float h = color.x;
                        float l = color.z;
                        float s = color.y;

                        if (s < 1.0 / 256.0)
                            return vec3(l, l, l);

                        float v1;
                        float v2;
                        if (l < 0.5)
                            v2 = l * (1.0 + s);
                        else
                            v2 = (l + s) - (s * l);

                        v1 = 2.0 * l - v2;

                        float d = 1.0 / 3.0;
                        float r = hueToIntensity(v1, v2, h + d);
                        float g = hueToIntensity(v1, v2, h);
                        float b = hueToIntensity(v1, v2, h - d);
                        return vec3(r, g, b);
                    }

                    void main() {
                        highp vec2 pos;
                        if (qt_MultiTexCoord0.y < 0.1) {
                            pos = p1;
                            vColor = vec4(1,1,1,1);
                        } else if (qt_MultiTexCoord0.x < 0.1) {
                            pos = p2;
                            vColor.rgb = HSLtoRGB(vec3 (chroma,1,0.5));
                            vColor.a = 1.0;
                        } else {
                            pos = p3;
                            vColor = vec4(0,0,0,1);
                        }
                        qt_TexCoord0 = qt_MultiTexCoord0;
                        gl_Position = qt_Matrix * vec4(pos, 0, 1);
                    }"
                }
            }
        }

        // Short cut buttons
        Item {
            anchors.fill: parent
            visible: showShortCuts
            //Greyscale
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 35
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(hue,0,0,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            saturation = 0
                            lightness = 0
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 45
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(hue,0,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            saturation = 0
                            lightness = 0.5
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 55
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(hue,0,1,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            saturation = 0
                            lightness = 1
                        }
                    }
                }
            }
            //Greyscale end

            //Primary
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -35
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(0,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            hue = 0
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -45
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(0.333,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            hue = 0.3333333333
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -55
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(0.667,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            hue = 0.6666666666
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }
            //Primary end

            //Secondary
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -125
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(0.167,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            hue = 0.1666666666
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -135
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(0.5,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            hue = 0.5
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -145
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: Qt.hsla(0.833,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            hue = 0.8333333333
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }
            //Secondary end

            //Buttons
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 125
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: palette.lightShade
                    Text {
                        anchors.centerIn: parent

                        anchors.horizontalCenterOffset: 1
                        anchors.verticalCenterOffset: 1

                        color : palette.neutralTextColor

                        rotation: 10

                        font.family : "Marlett"
                        font.pixelSize: Math.round(parent.width)

                        renderType: Text.QtRendering
                        text : "\uF033"
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 135
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    color: "transparent"
                    Image {
                        anchors.centerIn: parent
                        rotation: -135
                        source: "icons/loop_16x16.png"
                    }
                }
            }
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 145
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.darkColor
                    color: palette.lightShade
                    Text {
                        anchors.centerIn: parent

                        anchors.horizontalCenterOffset: 1
                        anchors.verticalCenterOffset: 1

                        color : palette.neutralTextColor

                        rotation: -10

                        font.family : "Marlett"
                        font.pixelSize: Math.round(parent.width)

                        renderType: Text.QtRendering
                        text : "\uF034"
                    }
                }
            }
            //Buttons end
        }
    }
}
