import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import WGControls 2.0

/*!
    \ingroup wgcontrols
    \brief Draggable dial control that has value 0.0 to 1.0 as well as this converted to degrees (angle) or radians.

    TODO: Have dials that don't revolve a full revolution (Like a volume knob)

Example:
\code{.js}

//Mathematical style anti-clockwise "East is 0" snapping dial in radians with no tickmarks
WGDial {
    height: 100
    width: 100

    minimumValue: 0
    maximumValue: Math.PI * 2

    value: itemData.value

    clockwise: false

    showValue: true
    unitString: "rad"
    decimals: 3

    tickInterval: 0
    minorTickInterval: 0
}

//Compass style "North is 0" dial with major and minor tickmarks
WGDial {
    height: 100
    width: 100

    value: itemData.value

    showValue: true
    zeroValue: 0

    decimals: 0
    stepSize: 1

    tickInterval: 45
    minorTickInterval: 15
}

//Positive/Negative dial from -180 to 180 that doesn't loop around limits
WGDial {
    Layout.preferredHeight: 100
    Layout.preferredWidth: 100

    value: itemData.value

    minimumValue: -180
    maximumValue: 180

    showValue: true
    zeroValue: 180

    loopAtBounds: false

    decimals: 0
    stepSize: 1

    tickInterval: 45
    minorTickInterval: 15
}

// Coarse 'clicky' dial from 1 to 10 with no units.
WGDial {
    Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
    Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

    value: 5

    minimumValue: 0
    maximumValue: 10

    showValue: true
    clockwise: true
    unitString: ""

    zeroValue: 0

    loopAtBounds: false

    decimals: 0
    stepSize: 1

    tickInterval: 36
    minorTickInterval: 0
}

// 'Infinite' Degrees dial that accepts multiple rotations
WGDial {
    Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
    Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

    value: 0

    minimumValue: -36000
    maximumValue: 36000
    minRotation: 0
    maxRotation: 360

    showValue: true
    showRotations: true
    clockwise: true
    zeroValue: 0
    loopAtBounds: true

    decimals: 0
    stepSize: 1

    tickInterval: 45
    minorTickInterval: 15
}

\endcode
*/

Item {
    id: control
    objectName: "WGDial"
    WGComponent { type: "WGDial20" }

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! The minimum value of the dial. Can be negative. */
    property real minimumValue: 0

    /*! The maximum value of the dial. */
    property real maximumValue: 360

    /*! Shows the value as an angle from 0 to 360.
    */
    property real angle: range.position;

    /*! Shows the value rounded to the decimals and stepSize
    */
    property real roundedValue: Number(value).toFixed(decimals)

    /*! Value of the dial between the minimum and maximum Values
    */
    property real value: 0

    /*! Number of decimal points after the 0.
    */
    property int decimals: 1

    /*! Step size when dragging the dial. This affects how much the mousewheel increases/decreases the value.
    */
    property real stepSize: 0.1

    /*! The number of degrees between each major tickmark. Recommend choosing a whole multiple of 360. Preferrably > 2.
    */
    property int tickInterval: 45

    /*! The number of degrees between each minor tickmark. Recommend choosing a whole multiple of 360. Preferrably > 2.
    */
    property int minorTickInterval: 15

    /*! The width of the major tickmarks.
    */
    property int tickWidth: 2

    /*! The width of the minor tickmarks.
    */
    property int minorTickWidth: 2

    /*! Shows a non-editable textbox containing the value when true.
    */
    property bool showValue: false

    /*! Shows the number of rotations in the textbox.
    */
    property bool showRotations: multipleRotations

    /*! String for the suffix after the value in the centre of the dial if showValue is true.
    */
    property string unitString: "°"

    /*! Indicates the zero point of the dial (0: top, 90: right, 180: bottom, 270: left) */
    property real zeroValue: 90

    /*! Indicates the direction the value increases */
    property bool clockwise: true

    /*! property indicates if the dial will spin around from min to max when the upper and lower values are reached.  */
    property bool loopAtBounds: true

    /*! The minimum value of a single 'rotation' if multiple rotations are possible.

        Strongly recommend this being zero if minimumValue < 0

        The default value is the minimumValue (no multiple rotations) */
    property real minRotation: minimumValue

    /*! The maximum value of a single 'rotation' if multiple rotations are possible.

        The default value is the maximumValue (no multiple rotations) */
    property real maxRotation: maximumValue

    /*! The number of units in a single rotation  */
    readonly property real singleRotation: (maxRotation - minRotation)

    /*! The number of rotations of the dial beyond the initial rotation. */
    readonly property int rotations: multipleRotations ? (value > 0 ? Math.floor(value / singleRotation) : Math.ceil(value / singleRotation)) : 0

    /*! The current value of the dial ignoring multiple rotations.  */
    readonly property real rotationValue: value % singleRotation

    /*! True if either of the minimum and maximum values do not == the minimum and maximum rotations.  */
    readonly property bool multipleRotations: minRotation != minimumValue || maxRotation != maximumValue

    /*! The gap between the dial itself and the edge of the control. Also affects the tickmark heights. */
    property int dialMargin: defaultSpacing.standardMargin + defaultSpacing.standardBorderSize

    /*! property indicates if the control represents multiple data values */
    property bool multipleValues: false

    /*! the property containing the string to be displayed when multiple values are represented by the control
    */
    property string __multipleValuesString: multipleValues ? "Multi?" : ""

    /*! Returns true when dial is pressed using the mouse */
    readonly property bool dialPressed: dialDrag.pressed

    /*! The RangeModel for the dial control */
    property alias range: range

    signal changeValue(var val)

    function clamp(val) {
        return Math.min(maximumValue, Math.max(minimumValue, val))
    }

    function setValue(newValue) {
        setValueHelper(control, "value", newValue)
        changeValue(newValue)
    }

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight * 4 : 88
    implicitWidth: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight * 4 : 88

    RangeModel {
        // if multiple rotations are possible this becomes a double range with + and - values.
        id: range
        minimumValue: multipleRotations ? minRotation - maxRotation : minRotation
        maximumValue: maxRotation
        value: rotationValue
        positionAtMinimum: multipleRotations ? -360 : 0
        positionAtMaximum: 360
    }

    Item {
        id: zeroFrame
        anchors.centerIn: parent
        height: parent.height
        width: parent.width

        rotation: zeroValue

        property int rotationChange: 0
        property real initValue: 0
        property real offset: 0

        function checkBounds(val)
        {
            // reset any changes to the number of rotations
            zeroFrame.rotationChange = 0

            // checks if the dial has hit the minimum or maximum rotation point and updates the changes to rotations.
            if (multipleRotations)
            {
                if (value == 0)
                {
                    zeroFrame.rotationChange = rotations * -1
                }
                else if (value > 0)
                {
                    if (val <= maxRotation && val > (singleRotation * 0.8) && rotationValue < (singleRotation * 0.2) && rotationValue >= minRotation)
                    {
                        zeroFrame.rotationChange -= 1
                    }
                    else if (val >= minRotation && val < (singleRotation * 0.2) && rotationValue <= maxRotation && rotationValue > (singleRotation * 0.8))
                    {
                        zeroFrame.rotationChange += 1
                    }
                }
                else
                {
                    if (val <= minRotation && val > (singleRotation * -0.2) && rotationValue < (singleRotation * -0.8) && rotationValue >= minRotation - maxRotation)
                    {
                        zeroFrame.rotationChange -= 1
                    }
                    else if (val >= minRotation - maxRotation && val < (maxRotation * -0.8) && rotationValue <= 0 && rotationValue > (singleRotation * -0.2))
                    {
                        zeroFrame.rotationChange += 1
                    }
                }
            }
            else if (!loopAtBounds)
            {
                // stops the dial from jumping over the min/max value if it's near the stop point and loopAtBounds is false.
                if (value > ((maxRotation - minRotation) * 0.80) + minRotation && val < ((maxRotation - minRotation) / 2) + minRotation)
                {
                    val = maxRotation
                }
                else if (value < ((maxRotation - minRotation) * 0.20) + minRotation && val > ((maxRotation - minRotation) / 2) + minRotation)
                {
                    val = minRotation
                }
            }
            val = clamp(val + ((rotations + zeroFrame.rotationChange) * singleRotation))
            return val
        }

        // draggable dial control
        WGButtonFrame {
            id: wheel
            radius: width

            anchors.centerIn: parent
            height: parent.height - (dialMargin * 2) - defaultSpacing.doubleBorderSize
            width: height

            rotation: clockwise ? range.position : 360 - range.position

            // arrow inside the dial
            Text {
                id: dialMarker
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                color: hoverArea.containsMouse ? palette.highlightColor : control.enabled ? palette.textColor : palette.disabledTextColor

                visible: !multipleValues

                font.family : "Marlett"
                font.pixelSize: Math.max(defaultSpacing.doubleMargin - defaultSpacing.doubleBorderSize, Math.round(control.height / 8))
                renderType: Text.QtRendering
                text : "\uF035"
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
            }
        }

        MouseArea {
            id: dialDrag
            objectName: "DragArea"
            anchors.fill: parent

            preventStealing: true

            property bool negativeRotation: false

            //Gets the mouse location around the circle in degrees.
            function getMouseDegrees(mx, my, object) {
                var point =  mapToItem (object, mx, my);

                var diffX = clockwise ? (point.x - object.width/2) : (object.width/2 - point.x);
                var diffY = (object.height/2 - point.y);

                var mouseAngle = Math.acos(diffY/Math.sqrt(diffX*diffX + diffY*diffY))*180/Math.PI

                if (diffX < 0) {
                    mouseAngle = 360 - mouseAngle;
                }

                // checks if the dial has reached the min or max rotation in a particular direction to change the sign of the value if multipleRotations
                if (mouseAngle > (360 * 0.8) && range.position < (360 * 0.2) && !negativeRotation && multipleRotations)
                {
                    negativeRotation = true
                }
                else if (mouseAngle < (360 * 0.2) && range.position > (-360 * 0.2) && negativeRotation && multipleRotations)
                {
                    negativeRotation = false
                }
                // changes the sign of the value if multipleRotations
                if (value < 0 && multipleRotations)
                {
                    mouseAngle = mouseAngle - 360
                }

                return mouseAngle
            }

            //set the initial values before rotating
            onPressed: {
                zeroFrame.initValue = rotationValue

                var deg = getMouseDegrees(mouse.x, mouse.y, dialDrag);

                deg = deg %= 360

                var newValue = range.valueForPosition(deg);

                zeroFrame.rotationChange = 0

                beginUndoFrame();

                newValue = clamp(newValue + (rotations * singleRotation))
                setValue(newValue)
            }

            //rotate the dial
            onPositionChanged: {
                var deg = getMouseDegrees(mouse.x, mouse.y, dialDrag)

                deg = deg %= 360

                var newValue = range.valueForPosition(deg);

                newValue = zeroFrame.checkBounds(newValue)

                setValue(newValue)
                zeroFrame.rotationChange = 0
            }

            onReleased: {
                if (rotationValue != zeroFrame.initValue || zeroFrame.rotationChange != 0 )
                {
                    var newValue = rotationValue;
                    abortUndoFrame();
                    setValue(newValue);
                }
                else
                {
                    abortUndoFrame();
                }
            }
        }

        // Hover enabled mouse area to make the dial marker blue when hovered and for wheel
        MouseArea {
            id: hoverArea
            objectName: "HoverArea"
            anchors.fill: parent
            hoverEnabled: true

            acceptedButtons: Qt.NoButton

            //change value on mouse wheel
            onWheel: {
                var wheelAngle = wheel.angleDelta.y > 0 ? rotationValue + stepSize : rotationValue - stepSize

                wheelAngle = zeroFrame.checkBounds(wheelAngle)
                setValue(wheelAngle)
                zeroFrame.rotationChange = 0
            }
        }

        Item {
            // This rotates the tickmarks and zeroValues if 0 is not actually the min value
            id: tickRotator
            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter

            // for some reason, just a binding doesn't work here. Has to be done onCompleted.
            Component.onCompleted: {
                rotation = (range.positionForValue(0))
            }

            Text {
                id: minusMarker

                anchors.left: parent.left
                anchors.top: parent.top

                visible: minimumValue < 0 && maximumValue > 0

                color: control.enabled ? palette.neutralTextColor : palette.disabledTextColor

                rotation: -zeroValue + tickRotator.rotation

                text: clockwise ? "-" : "+"

            }

            Text {
                id: plusMarker

                anchors.right: parent.right
                anchors.top: parent.top

                visible: minimumValue < 0 && maximumValue > 0

                color: control.enabled ? palette.neutralTextColor : palette.disabledTextColor

                rotation: -zeroValue + tickRotator.rotation

                text: clockwise ? "+" : "-"
            }

            // "0" marker
            Rectangle {
                id: zeroPoint
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top

                height: dialMargin + defaultSpacing.standardBorderSize
                width: height * 0.6

                radius: height * 0.4

                color: "transparent"
                border.width: 1
                border.color: rotationValue == 0 ? palette.highlightColor : control.enabled ? palette.neutralTextColor : palette.disabledTextColor

                rotation: -zeroValue + tickRotator.rotation

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -defaultSpacing.standardBorderSize
                    hoverEnabled: !dialPressed
                    cursorShape: dialPressed ? Qt.ArrowCursor : Qt.PointingHandCursor
                    enabled: !dialPressed

                    onClicked: {
                        setValue(clamp(rotations * singleRotation))
                    }
                }
            }

            // Clickable major tickmarks that light up. Tick delegates are actually two marks, top and bottom, to keep rotational symmetry
            Repeater {
                // Only make them if there are at least 2 total and less than or equal to 360
                model: if (tickInterval >= 0 && tickInterval <= 180)
                       {
                           Math.max((360 / (tickInterval * 2)), 0)
                       }
                       else
                       {
                           0
                       }

                delegate: Item {
                    anchors.centerIn: parent
                    height: parent.height
                    width: defaultSpacing.doubleMargin

                    rotation: index * tickInterval

                    // Top tick mark of pair. Different coloured if at zero point
                    Rectangle {
                        id: topMarker
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top

                        antialiasing: true

                        height: dialMargin
                        width: Math.max(tickWidth, 1)

                        visible: index !== 0

                        property color onColor: palette.highlightShade
                        property color offColor: control.enabled ? palette.darkerShade : palette.lighterShade

                        color:
                            if (clockwise)
                            {
                                control.rotationValue == range.valueForPosition((topMarker.parent.rotation + tickRotator.rotation) % 360) ? onColor : offColor
                            }
                            else
                            {
                                control.rotationValue == range.valueForPosition((360 - topMarker.parent.rotation - tickRotator.rotation) % 360) ? onColor : offColor
                            }

                        MouseArea {
                            anchors.centerIn: parent
                            height: defaultSpacing.standardMargin
                            width: defaultSpacing.standardMargin
                            hoverEnabled: !dialPressed
                            cursorShape: dialPressed ? Qt.ArrowCursor : Qt.PointingHandCursor
                            enabled: !dialPressed

                            onClicked: {
                                var newVal = 0
                                if (clockwise)
                                {
                                    if (value >= 0)
                                    {
                                        newVal = clamp(range.valueForPosition((topMarker.parent.rotation + tickRotator.rotation) % 360) + (rotations * singleRotation))
                                    }
                                    else
                                    {
                                        newVal = clamp(range.valueForPosition((topMarker.parent.rotation + tickRotator.rotation) % 360) + (rotations * singleRotation) - 360)
                                    }
                                    setValue(newVal)
                                }
                                else
                                {
                                    if (value >= 0)
                                    {
                                        newVal = clamp(range.valueForPosition((360 - topMarker.parent.rotation - tickRotator.rotation) % 360) + (rotations * singleRotation))
                                    }
                                    else
                                    {
                                        newVal = clamp(range.valueForPosition((360 - topMarker.parent.rotation - tickRotator.rotation) % 360) + (rotations * singleRotation) - 360)
                                    }
                                    setValue(newVal)
                                }
                            }
                        }
                    }

                    // bottom tick mark of pair
                    Rectangle {
                        id: bottomMarker
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom

                        antialiasing: true

                        height: dialMargin
                        width: Math.max(tickWidth, 1)

                        visible: tickInterval <= 180 && tickInterval > 0

                        property color onColor: palette.highlightShade
                        property color offColor: control.enabled ? palette.darkerShade : palette.lighterShade

                        color:
                            if (clockwise)
                            {
                                control.rotationValue == range.valueForPosition((bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360) ? onColor : offColor
                            }
                            else
                            {
                                control.rotationValue ==  range.valueForPosition((360 - bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360) ? onColor : offColor
                            }

                        MouseArea {
                            anchors.centerIn: parent
                            height: defaultSpacing.standardMargin
                            width: defaultSpacing.standardMargin
                            hoverEnabled: !dialPressed
                            cursorShape: dialPressed ? Qt.ArrowCursor : Qt.PointingHandCursor
                            enabled: !dialPressed

                            onClicked: {
                                var newVal = 0
                                if (clockwise)
                                {
                                    if (value >= 0)
                                    {
                                        newVal = clamp(range.valueForPosition((bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360) + (rotations * singleRotation))
                                    }
                                    else
                                    {
                                        newVal = clamp(range.valueForPosition((bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360) + (rotations * singleRotation) - 360)
                                    }
                                    setValue(newVal)
                                }
                                else
                                {
                                    if (value >= 0)
                                    {
                                        newVal = clamp(range.valueForPosition((360 - bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360) + (rotations * singleRotation))
                                    }
                                    else
                                    {
                                        newVal = clamp(range.valueForPosition((360 - bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360) + (rotations * singleRotation) - 360)
                                    }
                                    setValue(newVal)
                                }
                            }
                        }
                    }
                }
            }

            // Minor tickmarks. Tick delegates are actually two marks, top and bottom, to keep rotational symmetry
            Repeater {
                // Only draw ticks if there are at least 2 and less than or equal to 360
                model: if (minorTickInterval >= 1 && minorTickInterval <= 180)
                       {
                           360 / (minorTickInterval * 2)
                       }
                       else
                       {
                           0
                       }

                delegate: Item {
                    anchors.centerIn: parent
                    height: parent.height
                    width: defaultSpacing.doubleMargin

                    rotation: index * minorTickInterval
                    visible: rotation !== (index * tickInterval)

                    // Top tick mark of pair. Different coloured if at zero point
                    Rectangle {
                        id: topMinorMarker
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: dialMargin / 2

                        antialiasing: true

                        height: dialMargin / 2
                        width: Math.max(minorTickWidth, 1)

                        color: control.enabled ? palette.darkShade : palette.lightShade
                    }

                    // bottom tick mark of pair
                    Rectangle {
                        id: bottomMinorMarker
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: dialMargin / 2

                        antialiasing: true

                        height: dialMargin / 2
                        width: Math.max(minorTickWidth, 1)

                        color: control.enabled ? palette.darkShade : palette.lightShade
                    }
                }
            }
        }
    }

    Text {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: showValue ? -font.pixelSize - defaultSpacing.rowSpacing : 0
        color: control.enabled ? palette.textColor : palette.disabledTextColor
        text: multipleValues ? "" : showRotations && rotations != 0 ? rotations + "x" : ""
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: showRotations

        font.pixelSize: valueText.font.pixelSize - 2
    }

    // Optional text value in centre of dial
    Text {
        id: valueText
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: -unitString.length
        color: control.enabled ? palette.textColor : palette.disabledTextColor
        text: multipleValues ? __multipleValuesString : ((showRotations ? Number(rotationValue).toFixed(decimals) : roundedValue ) + unitString )
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: showValue

        font.pixelSize: (parent.height - dialMargin) * (0.21 - (0.01 * (unitString.length + decimals)))
    }

    /* DEPRECATED */
    property bool snapToClick: false
}
