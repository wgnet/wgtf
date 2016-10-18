import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Layouts 2.0

/*!
    \ingroup wgcontrols
    \brief A container for all the dialog boxes required by the TimelineControl
*/

Item {
    WGComponent { type: "WGTimelineDialogs20" }

    property alias setTimeDialog: setTimeDialog

    Dialog {
        id: setTimeDialog
        title: "Set Time"
        standardButtons: StandardButton.Ok | StandardButton.Cancel

        property bool endTime

        property QtObject startHandle: fakeHandle
        property QtObject endHandle: fakeHandle

        Item
        {
            id: fakeHandle
            property int value: 0
        }

        onAccepted: {
            beginUndoFrame();
            startHandle.value = secondsTime.value * timelineFrame.framesPerSecond + framesTime.value
            if (endTime)
            {
                endHandle.value = secondsEndTime.value * timelineFrame.framesPerSecond + framesEndTime.value
            }
            endUndoFrame();
            endTime = false
            startHandle = fakeHandle
            endHandle = fakeHandle
        }

        onVisibleChanged: {
            if (visible)
            {
                if (endTime)
                {
                    endLabel.visible = true
                    secondsEndTime.visible = true
                    framesEndTime.visible = true
                }
                else
                {
                    endLabel.visible = false
                    secondsEndTime.visible = false
                    framesEndTime.visible = false
                }
            }
        }

        Item {
            height: (setTimeDialog.endTime ? defaultSpacing.minimumRowHeight * 4 : defaultSpacing.minimumRowHeight * 2) + defaultSpacing.doubleMargin
            WGColumnLayout {
                anchors.fill: parent
                localForm: true

                WGLabel {
                    text: setTimeDialog.endTime ? "Set Start Time: " : "Set Keyframe Time: "
                }

                RowLayout {
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.fillWidth: true

                    spacing: defaultSpacing.doubleMargin
                    WGNumberBox {
                        id: secondsTime
                        Layout.preferredWidth: 50
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        minimumValue: 0
                        maximumValue: timelineFrame.totalFrames / timelineFrame.framesPerSecond
                        value: Math.floor(setTimeDialog.startHandle.value / timelineFrame.framesPerSecond)

                        suffix: "s"

                        onValueChanged: {
                            if (value == maximumValue)
                            {
                                framesTime.value = 0
                            }
                        }
                    }

                    WGNumberBox {
                        id: framesTime
                        Layout.preferredWidth: 50
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        minimumValue: 0
                        maximumValue: {
                            secondsTime.value < secondsTime.maximumValue ? timelineFrame.framesPerSecond : 0
                        }

                        value: setTimeDialog.startHandle.value % timelineFrame.framesPerSecond

                        suffix: "f"

                        onValueChanged: {
                            if (value == timelineFrame.framesPerSecond)
                            {
                                value = 0
                                secondsTime.value += 1
                            }
                        }
                    }
                }

                WGLabel {
                    id: endLabel
                    text: "Set End Time: "
                    visible: setTimeDialog.endTime
                }

                RowLayout {
                    visible: setTimeDialog.endTime
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.fillWidth: true

                    spacing: defaultSpacing.doubleMargin

                    WGNumberBox {
                        id: secondsEndTime
                        Layout.preferredWidth: 50
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        minimumValue: 0
                        maximumValue: timelineFrame.totalFrames / timelineFrame.framesPerSecond
                        value: Math.floor(setTimeDialog.endHandle.value / timelineFrame.framesPerSecond)

                        suffix: "s"

                        onValueChanged: {
                            if (value == maximumValue)
                            {
                                framesEndTime.value = 0
                            }
                        }
                    }


                    WGNumberBox {
                        id: framesEndTime
                        Layout.preferredWidth: 50
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        minimumValue: 0
                        maximumValue: {
                            secondsEndTime.value < secondsEndTime.maximumValue ? timelineFrame.framesPerSecond : 0
                        }

                        value: setTimeDialog.endHandle.value % timelineFrame.framesPerSecond

                        suffix: "f"

                        onValueChanged: {
                            if (value == timelineFrame.framesPerSecond)
                            {
                                value = 0
                                secondsEndTime.value += 1
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
