import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Private 2.0


/*!
 \ingroup wgcontrols
 \brief A splitTextField that shows H:M:S:F based of a time in total seconds with a customiseable framerate.

 TODO: Make timeFormat accept different values
 TODO: Make down arrow on NumberBoxes go below zero and adjust the other values. (eg 01:00 tick down to 0:59)

Example:
\code{.js}
WGTimeBox {
    width: 400

    Component.onCompleted: {
        setTime(12,30,15,0)
    }
}
\endcode
*/

RowLayout {
    id: timeBox
    objectName: "WGTimeBox"
    WGComponent { type: "WGTimeBox20" }

    /*! property indicates if the control represents multiple data values */
    property bool multipleValues: false

    /*! This property determines if the control is read only
    */
    property bool readOnly: false

    /*! This property determines if the framerate is shown as a label after the number boxes
        The default value is \c true
    */
    property bool showFramerateLabel: true

    /*! This minimum value of the time property
        The default value is \c 0
    */
    property var minimumValue: 0

    /*! This minimum value of the time property
        The default value is \c 2147483647
    */
    property var maximumValue: 2147483647

    /*! The time represented by the timeBox in total seconds.
    */
    property var time

    /*! Readonly. The number of hours displayed in the Hours box.
    */
    readonly property int hours: Math.floor(time / 3600)

    /*! Readonly. The number of minutes displayed in the Minutes box (not total minutes).
    */
    readonly property int minutes: Math.floor((time % 3600) / 60)

    /*! Readonly. The number of seconds displayed in the Seconds box (not total Seconds).
    */
    readonly property int seconds: Math.floor(time % 60)

    /*! Readonly. The number of frames displayed in the Frames box (not total Frames).
    */
    readonly property int frames: Math.round((time - Math.floor(time)) * framerate)

    /*! The number of frames per second.
        The default value is \c 30
    */
    property int framerate: 30

    /*! INACTIVE: The visible time boxes.
        The default value is \c ["h","m","s","f"]

        TODO: Make this work
    */
    readonly property var timeFormat: ["h","m","s","f"]

    /*! \internal */
    property bool __updatingTime: false

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: ((sizeText.contentWidth + defaultSpacing.doubleMargin) * timeFormat.length) + defaultSpacing.doubleMargin + frameLabel.width

    /*! Set the time of all the number boxes and the time property to [h,m,s,f]
    */
    function setTime(h, m, s, f) {
        if (!__updatingTime)
        {
            __updatingTime = true

            var temp_time = Math.min(maximumValue, (h * 3600))
            temp_time = Math.min(maximumValue, temp_time + (m * 60))
            temp_time = Math.min(maximumValue, temp_time + s)
            temp_time = Math.min(maximumValue, temp_time + (f / Math.max(framerate, 1)))

            beginUndoFrame();

            time = temp_time

            endUndoFrame();

            boxH.value = hours;
            boxM.value = minutes;
            boxS.value = seconds;
            boxF.value = frames;

            __updatingTime = false
        }
    }

    Text {
        id: sizeText
        visible: false
        text: "99999s"
    }

    Text {
        id: minSizeText
        visible: false
        text: "99s"
    }

    WGSplitTextFrame {
        id: splitTimeFrame
        height: parent.height
        characterSeparator: true
        separatorCharacter: ":"

        Layout.maximumWidth: (sizeText.contentWidth + defaultSpacing.doubleMargin) * 4
        Layout.minimumWidth: (minSizeText.contentWidth + defaultSpacing.doubleMargin) * 4
        Layout.preferredWidth: (sizeText.contentWidth + defaultSpacing.doubleMargin) * 4
        Layout.fillWidth: true

        states: [
            State {
                name: "READ ONLY"
                when: timeBox.enabled && timeBox.readOnly
                PropertyChanges {target: splitTimeFrame; color: "transparent"}
                PropertyChanges {target: splitTimeFrame; border.color: palette.darkestShade}
            },
            State {
                name: "DISABLED"
                when: !timeBox.enabled
                PropertyChanges {target: splitTimeFrame; border.color: palette.darkestShade}
            }
        ]

        boxList: [
            WGNumberBox {
                id: boxH
                objectName: "numberBox_H"
                value: hours
                minimumValue: 0
                maximumValue: timeBox.maximumValue
                multipleValues: timeBox.multipleValues
                readOnly: timeBox.readOnly
                suffix: "h"

                visible: timeFormat.indexOf("h") != -1

                width: visible ? sizeText.contentWidth + defaultSpacing.doubleMargin : 0

                onEditingFinished: {
                    setTime(value, minutes, seconds, frames)
                }
            },
            WGNumberBox {
                id: boxM
                objectName: "numberBox_M"
                value: minutes
                minimumValue: 0
                maximumValue: timeBox.maximumValue
                multipleValues: timeBox.multipleValues
                readOnly: timeBox.readOnly
                suffix: "m"

                visible: timeFormat.indexOf("m") != -1

                width: visible ? sizeText.contentWidth + defaultSpacing.doubleMargin : 0

                onEditingFinished: {
                    setTime(hours, value, seconds, frames)
                }
            },
            WGNumberBox {
                id: boxS
                objectName: "numberBox_S"
                value: seconds
                minimumValue: 0
                maximumValue: timeBox.maximumValue
                multipleValues: timeBox.multipleValues
                readOnly: timeBox.readOnly
                suffix: "s"

                visible: timeFormat.indexOf("s") != -1

                width: visible ? sizeText.contentWidth + defaultSpacing.doubleMargin : 0

                onEditingFinished: {
                    setTime(hours, minutes, value, frames)
                }
            },
            WGNumberBox {
                id: boxF
                objectName: "numberBox_F"
                value: frames
                minimumValue: 0
                maximumValue: timeBox.maximumValue
                multipleValues: timeBox.multipleValues
                readOnly: timeBox.readOnly
                suffix: "f"

                visible: timeFormat.indexOf("f") != -1

                width: visible ? sizeText.contentWidth + defaultSpacing.doubleMargin : 0

                onEditingFinished: {
                    setTime(hours, minutes, seconds, value)
                }
            }
        ]
    }

    WGLabel {
        id: frameLabel

        Layout.preferredWidth: contentWidth
        anchors.verticalCenter: parent.verticalCenter

        visible: timeFormat.indexOf("f") != -1 && showFramerateLabel

        text: "(" + framerate + "fps)"
    }

    Item {
        Layout.fillWidth: true
    }
}
