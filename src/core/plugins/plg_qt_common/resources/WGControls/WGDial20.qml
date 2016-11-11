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
    snapToClick: false

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
    property real roundedValue: Number(range.value).toFixed(decimals)

    /*! Value of the dial between the minimum and maximum Values
    */
    property alias value: range.value

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

    /*! String for the suffix after the value in the centre of the dial if showValue is true.
    */
    property string unitString: "°"

    /*! Indicates the zero point of the dial (0: top, 90: right, 180: bottom, 270: left) */
    property real zeroValue: 90

    /*! Indicates the direction the value increases */
    property bool clockwise: true

    /*! property indicates if the dial will spin around from min to max when the upper and lower values are reached.  */
    property bool loopAtBounds: true

    /*! If true, snaps the dial marker to the current mouse position when pressed.

        The default is false if loopAtBounds is true.

        Recommend not making snapToClick true if loopAtBounds is true or the dial will behave strangely when it nears the bounds. */
    property bool snapToClick: !loopAtBounds

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

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight * 4 : 88
    implicitWidth: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight * 4 : 88

    RangeModel {
        id: range
        minimumValue: control.minimumValue
        maximumValue: control.maximumValue
        positionAtMinimum: 0
        positionAtMaximum: 360
    }

    Item {
        id: zeroRotationFrame
        anchors.centerIn: parent
        height: parent.height
        width: parent.width

        rotation: zeroValue

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

            property real initValue: 0
            property real offset: 0

            //Gets the mouse location around the circle in degrees.
            function getMouseDegrees(mx, my, object) {
                var point =  mapToItem (object, mx, my);

                var diffX = clockwise ? (point.x - object.width/2) : (object.width/2 - point.x);
                var diffY = (object.height/2 - point.y);

                var mouseAngle = Math.acos(diffY/Math.sqrt(diffX*diffX + diffY*diffY))*180/Math.PI

                if (diffX < 0) {
                    mouseAngle = 360 - mouseAngle;
                }

                return mouseAngle
            }

            //set the initial values before rotating
            onPressed: {
                initValue = value;

                var deg = getMouseDegrees(mouse.x, mouse.y, dialDrag);

                deg = Math.abs(deg %= 360)

                var newValue = range.valueForPosition(deg);

                beginUndoFrame();

                if (snapToClick)
                {
                    value = newValue
                }
                else
                {
                    offset = initValue - newValue
                }
            }

            //rotate the dial
            onPositionChanged: {
                var deg = getMouseDegrees(mouse.x, mouse.y, dialDrag)
                deg = Math.abs(deg %= 360)
                var newValue = range.valueForPosition(deg);

                multipleValues = false

                if(!snapToClick)
                {
                    newValue = (newValue + offset)
                }

                // loops around if loopAtBounds or stays at max or min value if !loopAtBounds
                if (newValue < minimumValue)
                {
                    newValue = loopAtBounds ? maximumValue + (newValue - minimumValue) : minimumValue
                }
                else if (newValue > maximumValue)
                {
                    newValue = loopAtBounds ? minimumValue + (newValue - maximumValue) : maximumValue
                }

                // stops the dial from jumping over the min/max value if it's near the stop point and loopAtBounds is false.
                if (!loopAtBounds)
                {
                    if (value > ((maximumValue - minimumValue) * 0.80) + minimumValue && newValue < ((maximumValue - minimumValue) / 2) + minimumValue)
                    {
                        newValue = maximumValue
                    }
                    else if (value < ((maximumValue - minimumValue) * 0.20) + minimumValue && newValue > ((maximumValue - minimumValue) / 2) + minimumValue)
                    {
                        newValue = minimumValue
                    }
                }

                value = Number(newValue).toFixed(decimals);
            }

            onReleased: {
                if (value != initValue)
                {
                    endUndoFrame();
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
                var wheelAngle = wheel.angleDelta.y > 0 ? value + stepSize : value - stepSize

                if (wheelAngle < minimumValue)
                {
                    wheelAngle = loopAtBounds ? maximumValue + (wheelAngle - minimumValue) : minimumValue
                }
                else if (wheelAngle > maximumValue)
                {
                    wheelAngle = loopAtBounds ? minimumValue + (wheelAngle - maximumValue) : maximumValue
                }

                multipleValues = false

                value = wheelAngle
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
                border.color: value == 0 ? palette.highlightColor : control.enabled ? palette.neutralTextColor : palette.disabledTextColor

                rotation: -zeroValue + tickRotator.rotation

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -defaultSpacing.standardBorderSize
                    hoverEnabled: !dialPressed
                    cursorShape: dialPressed ? Qt.ArrowCursor : Qt.PointingHandCursor
                    enabled: !dialPressed

                    onClicked: {
                        multipleValues = false
                        control.value = 0
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
                                control.value == range.valueForPosition(Math.abs((topMarker.parent.rotation + tickRotator.rotation) % 360)) ? onColor : offColor
                            }
                            else
                            {
                                control.value == range.valueForPosition(Math.abs((360 - topMarker.parent.rotation - tickRotator.rotation) % 360)) ? onColor : offColor
                            }

                        MouseArea {
                            anchors.centerIn: parent
                            height: defaultSpacing.standardMargin
                            width: defaultSpacing.standardMargin
                            hoverEnabled: !dialPressed
                            cursorShape: dialPressed ? Qt.ArrowCursor : Qt.PointingHandCursor
                            enabled: !dialPressed

                            onClicked: {
                                multipleValues = false
                                if (clockwise)
                                {
                                    control.value = range.valueForPosition(Math.abs((topMarker.parent.rotation + tickRotator.rotation) % 360))
                                }
                                else
                                {
                                    control.value = range.valueForPosition(Math.abs((360 - topMarker.parent.rotation - tickRotator.rotation) % 360))
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
                                control.value == range.valueForPosition(Math.abs(bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360) ? onColor : offColor
                            }
                            else
                            {
                                control.value ==  range.valueForPosition(Math.abs(360 - bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360) ? onColor : offColor
                            }

                        MouseArea {
                            anchors.centerIn: parent
                            height: defaultSpacing.standardMargin
                            width: defaultSpacing.standardMargin
                            hoverEnabled: !dialPressed
                            cursorShape: dialPressed ? Qt.ArrowCursor : Qt.PointingHandCursor
                            enabled: !dialPressed

                            onClicked: {
                                multipleValues = false
                                if (clockwise)
                                {
                                    control.value = range.valueForPosition(Math.abs(bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360)
                                }
                                else
                                {
                                    control.value =  range.valueForPosition(Math.abs(360 - bottomMarker.parent.rotation + 180 + tickRotator.rotation) % 360)
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

    // Optional text value in centre of dial
    Text {
        anchors.centerIn: parent
        color: control.enabled ? palette.textColor : palette.disabledTextColor
        text: multipleValues ? __multipleValuesString : (roundedValue + unitString)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: showValue

        font.pixelSize: (parent.height - dialMargin) * (0.21 - (0.01 * (unitString.length + decimals)))
    }
}
