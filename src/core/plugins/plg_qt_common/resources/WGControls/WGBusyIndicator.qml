import QtQuick 2.5
import QtQuick.Controls 1.4

/*!
 \ingroup wgcontrols
 \brief An control used to indicate activity. Typically when the UI is blocked.

Example:
\code{.js}
WGBusyIndicator {
    running: button.checked
}
\endcode
*/

Rectangle {
    id: busyIndicator
    objectName: "WGBusyIndicator"
    WGComponent { type: "WGBusyIndicator" }
    
    /*!
        Determines whether the busy indicator is visible or not.

        Set this to true to show the spinning animation.
    */
    property bool running: false

    /*!
        The height and width of the spinning animation.

        Defaults to either minimumRowheight or a percentage of the smallest parent dimension, whichever is larger.
    */
    property int indicatorSize: Math.max(Math.min(parent.width / 5, parent.height / 5), defaultSpacing.minimumRowHeight)

    /*!
        The size of the dots in the spinning animation.

        The default is a percentage of the indicatorSize.
    */
    property int dotSize: indicatorSize / 6

    /*!
        The number of dots in the spinning animation.

        The default is 12. Recommend choosing an even multiple of 360
    */
    property int indicatorDots: 12

    /*!
        Whether or not the indicator will capture any mouse clicks or wheel events for objects behind it.

        The default is true.
    */
    property bool captureMouse: true

    /*!
        Whether or not the indicator fades in and out when running changes.

        The default is true.
    */
    property bool fadeIndicator: true

    /*!
        The duration of the busy indicator in milliseconds.

        The default value is -1 and the indicator will remain visible until running == false.
    */
    property int duration: -1

    /*!
        The color of the rotating dots.

        The default value is palette.highlightShade.
    */
    property color dotColor: palette.highlightShade

    /*!
        A string of multi-line text that is displayed below the spinning dots.

        The default value is blank.
    */
    property string busyMessage: ""

    anchors.fill: parent
    color: palette.mainWindowColor
    opacity: busyIndicator.running ? 1 : 0

    Behavior on opacity { enabled: fadeIndicator; OpacityAnimator { duration: 250 } }

    Timer {
        id: durationTimer
        running: busyIndicator.running && busyIndicator.duration >= 0
        interval: busyIndicator.duration
        repeat: false
        onTriggered: busyIndicator.running = false
    }

    Item {
        id: dotRing
        visible: busyIndicator.running
        height: indicatorSize
        width: indicatorSize

        anchors.centerIn: parent

        property int showDot: 0

        Repeater {
            model: indicatorDots
            Item {
                width: dotSize
                height: indicatorSize

                anchors.centerIn: parent
                transformOrigin: Item.Center

                rotation: index * (360 / indicatorDots)

                Rectangle {
                    id: colorDot
                    color: dotColor

                    width: dotSize
                    height: dotSize
                    radius: dotSize

                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter

                    opacity: 0

                    states: [
                        State {
                            name: "OFF"
                            when: dotRing.showDot != index
                            PropertyChanges { target: colorDot; opacity: 0.0}
                        },
                        State {
                            name: "ON"
                            when: dotRing.showDot == index
                            PropertyChanges { target: colorDot; opacity: 1.0}
                        }

                    ]
                    transitions: [
                        Transition {
                            from: "ON"
                            to: "OFF"
                            NumberAnimation { target: colorDot; properties: "opacity"; duration: 400}
                        }
                    ]

                }
            }
        }


        Timer {
            id: spinTimer
            running: busyIndicator.running
            interval: 100
            repeat: true
            onTriggered: {
                if (dotRing.showDot < indicatorDots - 1)
                {
                    dotRing.showDot++
                }
                else
                {
                    dotRing.showDot = 0
                }
            }
        }

        WGMultiLineText {
            anchors.top: parent.bottom
            anchors.topMargin: defaultSpacing.doubleMargin
            anchors.horizontalCenter: parent.horizontalCenter

            visible: busyIndicator.busyMessage

            width: busyIndicator.width / 2

            color: palette.neutralTextColor

            font.pointSize: Math.max(indicatorSize / 10, 9)

            text: busyIndicator.busyMessage
            horizontalAlignment: Text.AlignHCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: enabled
        propagateComposedEvents: false

        enabled: captureMouse && busyIndicator.running

        onWheel: {
            wheel.accepted = true
        }

        onPressed: {
            mouse.accepted = true
        }

        onDoubleClicked: {
            mouse.accepted = true
        }
    }
}
