import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import WGControls 2.0

/*!
    \ingroup wgcontrols
    \brief A toolbar for holding useful buttons that affect the WGTimelineControl
*/

Rectangle {
    id: timelineToolbar
    WGComponent { type: "WGTimelineToolbar20" }
    
    color: palette.mainWindowColor

    property bool looping: false
    property bool playing: false
    property bool paused: false

    RowLayout {
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin

        // start/stop playback
        WGPushButton {
            id: play
            checkable: true
            iconSource: checked ? "icons/stop_16x16.png" : "icons/play_on_16x16.png"
            onCheckedChanged: {
                if (checked)
                {
                    playing = true
                }
                else
                {
                    playing = false
                    paused = false
                }
            }

            Connections {
                target: timelineToolbar
                onPlayingChanged: {
                    if (playing)
                    {
                        play.checked = true
                    }
                    else
                    {
                        play.checked = false
                    }
                }
            }
        }

        // pause/resume playback
        WGPushButton {
            id: pause
            iconSource: "icons/pause_16x16.png"
            enabled: play.checked
            checkable: true
            onCheckedChanged: {
                if (checked)
                {
                    paused = true
                }
                else
                {
                    paused = false
                }
            }
            Connections {
                target: timelineToolbar
                onPlayingChanged: {
                    if (!playing)
                    {
                        pause.checked = false
                    }
                }
            }
            Connections {
                target: timelineToolbar
                onPausedChanged: {
                    if (paused)
                    {
                        pause.checked = true
                    }
                    else
                    {
                        pause.checked = false
                    }
                }
            }
        }

        // loop playback
        WGPushButton {
            id: loop
            iconSource: "icons/loop_16x16.png"
            checkable: true
            enabled: !play.checked

            onCheckedChanged: {
                if (checked)
                {
                    looping = true
                }
                else
                {
                    looping = false
                }
            }
        }

        // time broken up into seconds and frames
        WGLabel {
            text: "Current Time: "
        }
        WGNumberBox {
            id: currentTimeBox

            Layout.preferredWidth: 50

            value: Math.floor(timelineFrame.currentFrame / timelineFrame.framesPerSecond)
            minimumValue: 0.1
            maximumValue: 100
            stepSize: 1
            decimals: 0
            suffix: "s"

            // Need to upgrade WGSpinBox to QtQuick.Controls 1.4
            onEditingFinished: {
                timelineFrame.currentFrame = (currentTimeBox.value * timelineFrame.framesPerSecond) + currentFrameBox.value
            }

            Connections {
                target: timelineFrame
                onCurrentFrameChanged: {
                    currentTimeBox.value = Math.floor(timelineFrame.currentFrame / timelineFrame.framesPerSecond)
                }
            }
        }

        WGNumberBox {
            id: currentFrameBox

            Layout.preferredWidth: 50

            value: timelineFrame.currentFrame % timelineFrame.framesPerSecond
            minimumValue: 0
            maximumValue: timelineFrame.framesPerSecond
            stepSize: 1
            decimals: 0
            suffix: "f"

            // Need to upgrade WGSpinBox to QtQuick.Controls 1.4
            onEditingFinished: {

                // TODO make it tick back a second if pressing the down arrow
                if (currentFrameBox.value == timelineFrame.framesPerSecond)
                {
                    currentTimeBox.value += 1
                    currentFrameBox.value = 0
                }

                timelineFrame.currentFrame = (currentTimeBox.value * timelineFrame.framesPerSecond) + currentFrameBox.value
            }

            Connections {
                target: timelineFrame
                onCurrentFrameChanged: {
                    currentFrameBox.value = timelineFrame.currentFrame % timelineFrame.framesPerSecond
                }
            }
        }

        // changing the framerate at the moment does bad things.
        WGLabel {
            text: "(" + framesPerSecond + " fps)"
        }

        // show frame labels
        WGPushButton {
            id: showLabelBtn
            iconSource: checked ? "icons/tag_on_16x16.png" : "icons/tag_off_16x16.png"
            checkable: true
            checked: timelineFrame.showLabels

            onClicked: {
                timelineFrame.showLabels = !timelineFrame.showLabels
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        // TODO: Make changing the duration less destructive
        WGLabel {
            text: "Duration: "
        }

        WGNumberBox {
            id: durationNumberBox

            Layout.preferredWidth: 50

            value: timeScale
            minimumValue: 0.1
            maximumValue: 100
            stepSize: 1
            decimals: 0
            suffix: "s"

            // Need to upgrade WGSpinBox to QtQuick.Controls 1.4
            onEditingFinished: {
                timelineFrame.timeScale = durationNumberBox.value
            }

            Connections {
                target: timelineFrame
                onTimeScaleChanged: {
                    durationNumberBox.value = timeScale
                }
            }
        }
    }
}
