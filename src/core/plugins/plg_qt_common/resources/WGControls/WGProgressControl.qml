import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

import WGControls 1.0
import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
 \brief An expanding progress bar that fills up as an action is completed.
 It can have a % complete, an indeterminate style or even a fake completion animation.

Example:
\code{.js}
WGProgressControl {
    Layout.fillWidth: true
    text: "File Importing..."
    units: "%"
}
\endcode
*/

Rectangle {
    id: progControl
    objectName: "WGProgressControl"
    WGComponent { type: "WGProgressControl" }

    /*!
        This property toggles the display of the percentage complete indicator
        The default value is \c false
    */
    property bool indeterminate: false

    /*!
        This property defines the suffix string used to indicate the units used
        The default value is an empty string
    */
    property string units: ""

    /*!
        This property will cause the progress bar to progress for demonstration purposes and testing.
        The default value is \c false
    */
    property bool fakeProgress: false

    /*!
        This property determines how long progress takes when fakeProgress_ is true.
        The default value is \c false
    */
    property int fakeDuration: 1000

    /*!
        This final valule of the progress control
        The default value is \c 1
    */
    property real endValue: 1

    /*! This property holds the string to be displayed above the progress bar.
        The default value is an empty string
    */
    property alias text: descriptionText.text

    /*!
        The current value of the progress control
    */
    property alias value: progBar.value

    property alias maximumValue: progBar.maximumValue

    /*! \internal */
    property int __percentage: 0

    /*!
        Fires when the progress is stopped.
        completed is true if the bar reached the end and finished properly.
    */
    signal progressEnded (bool completed)

    color: "transparent"

    implicitHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin
    implicitWidth: defaultSpacing.standardMargin

    PropertyAnimation {
        target: progBar; property: "value"
        duration: fakeDuration; from: 0; to: maximumValue
        easing.type: Easing.InOutQuad ; running: fakeProgress
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: defaultSpacing.standardMargin

        Rectangle {
            id: labelBox
            color: "transparent"

            Layout.fillHeight: true
            Layout.fillWidth: true


            Text {
                id: descriptionText
                anchors.left: parent.left
                anchors.top: parent.top

                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

                color: palette.textColor
            }

            Text {
                id: valueText

                anchors.left: descriptionText.right
                anchors.top: parent.top
                anchors.leftMargin: defaultSpacing.standardMargin

                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

                text: __percentage

                visible: !indeterminate
                color: palette.textColor
                horizontalAlignment: Text.AlignRight
            }

            Text {
                id: unitText
                anchors.left: valueText.right
                anchors.top: parent.top

                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

                text: units

                visible: !indeterminate && units != ""
                color: palette.textColor
                horizontalAlignment: Text.AlignRight
            }

            WGPushButton {
                text: "Cancel"
                anchors.right: parent.right
                height: defaultSpacing.minimumRowHeight
                onClicked: {
                    progControl.progressEnded(false)
                }
            }
        }

        ProgressBar {
            id: progBar

            value: progControl.indeterminate ? maximumValue : 0

            Layout.fillWidth: true
            Layout.maximumHeight: 8
            Layout.minimumHeight: 8

            onValueChanged: {
                __percentage = progBar.value/maximumValue * 100
                if (!indeterminate && progBar.value == endValue)
                {
                    progControl.progressEnded(true)
                }
            }

            style: ProgressBarStyle {

                background: WGTextBoxFrame {}

                progress: Rectangle {
                    color: palette.highlightColor
                    radius: defaultSpacing.halfRadius
                    Item {
                            anchors.fill: parent
                            anchors.margins: 1
                            visible: progControl.indeterminate
                            clip: true
                            Row {
                                Repeater {
                                    Rectangle {
                                        color: index % 2 ? "transparent" : palette.darkShade
                                        width: 10 ; height: control.height
                                    }
                                    model: control.width / 10 + 2
                                }
                                XAnimator on x {
                                    from: -20 ; to: 0
                                    loops: Animation.Infinite
                                    running: progControl.indeterminate
                                }
                            }
                    }
                }
            }
        }
    }

    /*! Deprecated */
    property alias percentage_: progControl.__percentage

    /*! Deprecated */
    property alias indeterminate_: progControl.indeterminate

    /*! Deprecated */
    property alias units_: progControl.units

    /*! Deprecated */
    property alias fakeProgress_: progControl.fakeProgress

    /*! Deprecated */
    property alias fakeDuration_: progControl.fakeDuration

    /*! Deprecated */
    property alias endValue_: progControl.endValue
}
