import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Layouts 1.0
import WGControls.Global 2.0
import WGControls.Color 2.0
import WGControls.Styles 2.0
import WGControls.Private 2.0

FocusScope {
    id: colorLayout
    WGComponent { type: "coloranim_component" }
    objectName: typeof itemData.indexPath == "undefined" ? "coloranim_component" :  itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
    implicitHeight: animated ? defaultSpacing.minimumRowHeight * 4 : defaultSpacing.minimumRowHeight
    implicitWidth: parent.width

    // Placeholder data properties
    property bool showAlpha: true
    property bool animated: false
    property bool looping: false
    property real currentTime: 0.0
    property alias currentColor: colorObject.rgbaColor
    property alias hsvColor: colorObject.altColor
    property real duration: 2.0

    WGAbstractColor {
        id: colorObject
        rgbaColor: timeToColor(currentTime)
    }

    // Placeholder keyframe data
    Item {
        id: keyList

        Item {
            property real time: 0.5
            property vector4d value: Qt.vector4d(0,0,0,1)
        }
        Item {
            property real time: 1.0
            property vector4d value: Qt.vector4d(1,0,0,1)
        }
        Item {
            property real time: 1.5
            property vector4d value: Qt.vector4d(1,1,0,1)
        }
    }

    function getIntPoint(a,b,percent)
    {
        return (a + (b - a) * percent)
    }

    // Placeholder color interpolation function
    function timeToColor(t) {
        var minKey = 0
        var maxKey = keyList.children.length - 1

        for (var i=0; i<keyList.children.length;i++)
        {
            if (keyList.children[i].time <= t && i >= minKey)
            {
                minKey = i
            }

            if (keyList.children[i].time >= t)
            {
                maxKey = i
                break;
            }
        }

        var minTime = keyList.children[minKey].time == keyList.children[maxKey].time ? 0.0 : keyList.children[minKey].time
        var maxTime = keyList.children[minKey].time == keyList.children[maxKey].time ? duration : keyList.children[maxKey].time

        var minColor = keyList.children[minKey].value
        var maxColor = keyList.children[maxKey].value

        var percent = (t - minTime) / (maxTime - minTime)

        var intCol = Qt.vector4d((getIntPoint(minColor.x, maxColor.x, percent)),
                                 (getIntPoint(minColor.y, maxColor.y, percent)),
                                 (getIntPoint(minColor.z, maxColor.z, percent)),
                                 (getIntPoint(minColor.w, maxColor.w, percent)))

        return(intCol)
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            id: firstRow
            Layout.fillWidth: true
            Layout.maximumWidth: colButton.width + hsvLbl.width + splitTextFrame.width + (showAlpha ? (aLabel.width + colA.width) : 0) + defaultSpacing.doubleMargin
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            WGColorButton {
                // TODO: Show color picker to change the color/add a handle at the current time?
                id: colButton
                objectName: "colorButton"
                color: currentColor
                Layout.preferredWidth: 40
                Layout.preferredHeight: parent.height
            }

            WGLabel {
                id: hsvLbl
                objectName: "colorButton_Label"
                Layout.minimumWidth: 40
                Layout.maximumWidth: 40
                Layout.preferredHeight: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: " HSV: "
            }

            WGSplitTextFrame {
                // TODO: Maybe make this editable and change the color/add a handle at the current time?
                id: splitTextFrame
                Layout.preferredHeight: parent.height
                Layout.preferredWidth: 180
                boxList: [
                    WGNumberBox {
                        id: colH
                        value: hsvColor.x
                        stepSize: 0.01
                        decimals: 2
                        minimumValue: 0
                        maximumValue: 1
                        multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                        readOnly: true

                    },
                    WGNumberBox {
                        id: colS
                        value: hsvColor.y
                        stepSize: 0.01
                        decimals: 2
                        minimumValue: 0
                        maximumValue: 1
                        multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                        readOnly: true

                    },
                    WGNumberBox {
                        id: colV
                        value: hsvColor.z
                        stepSize: 0.01
                        decimals: 2
                        minimumValue: 0
                        maximumValue: 1
                        multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                        readOnly: true

                    }
                ]
            }

            WGLabel {
                id: aLabel
                objectName: "a_Label"
                Layout.minimumWidth: contentWidth + defaultSpacing.rowSpacing
                Layout.maximumWidth: contentWidth + defaultSpacing.rowSpacing
                Layout.preferredHeight: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: " A: "
                visible: showAlpha
            }

            WGNumberBox {
                // TODO: Maybe make this editable and change the alpha/add a handle at the current time?
                id: colA
                objectName: "numberBox_W"
                visible: showAlpha

                Layout.preferredHeight: parent.height
                Layout.preferredWidth: colS.width

                value: currentColor.w
                minimumValue: 0
                maximumValue: 1
                stepSize: 0.01
                decimals: 2
                multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                readOnly: true
                hasArrows: false
                horizontalAlignment: Text.AlignHCenter
            }

            WGPushButton {
                // Toggles if the color is animated or not
                id: animatedToggle
                iconSource: "icons/animate_16x16.png"
                checkable: true
                checked: animated

                onClicked: {
                    animated = animatedToggle.checked
                }
            }
        } //firstRow

        RowLayout {
            id: secondRow
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            spacing: 0

            visible: animated

            Item {
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.fillWidth: true

                // Double line height frame for the secondRow and thirdRow
                WGButtonFrame {
                    anchors.top: parent.top
                    width: parent.width
                    height: defaultSpacing.minimumRowHeight * 2
                }

                WGGradientSlider {
                    // TODO: Make this slider link to proper data
                    id: gradientSlider
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: defaultSpacing.doubleMargin
                    anchors.rightMargin: defaultSpacing.doubleMargin
                    y: defaultSpacing.standardBorderSize
                    minimumValue: 0
                    maximumValue: duration

                    Repeater {
                        model: keyList.children.length
                        WGGradientSliderHandle {
                            value: keyList.children[index].time
                            color: keyList.children[index].value
                        }
                    }

                    // temporary stuff to make this slider uninteractive
                    focus: false
                    allowMouseWheel: false
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton | Qt.LeftButton
                        hoverEnabled: true
                    }
                }
            }

        } //secondRow

        RowLayout {
            id: thirdRow
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            spacing: 0

            visible: animated

            Item {
                Layout.preferredWidth: defaultSpacing.doubleMargin
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
            }

            // Draggable current time handle that moves automatically on playback.
            WGSlider {
                id: frameScrubber
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight

                value: currentTime

                minimumValue: 0
                maximumValue: duration
                stepSize: 0.1

                handleClamp: false

                style: WGSliderStyle {
                    groove: Item {
                        implicitWidth: defaultSpacing.minimumRowHeight
                        implicitHeight: defaultSpacing.minimumRowHeight - defaultSpacing.doubleBorderSize
                        y: -defaultSpacing.standardBorderSize
                        WGTextBoxFrame {
                            radius: 0
                            anchors.fill: parent
                        }
                    }
                    bar: null
                }

                onChangeValue: {
                    currentTime = val
                }

                // small chunky handle with a black/white indicator line above it
                WGSliderHandle {
                    showBar: false
                    handleStyle: WGButtonFrame {
                        id: scrubberHandle
                        implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
                        implicitHeight: defaultSpacing.minimumRowHeight
                        color: palette.mainWindowColor
                        borderColor: frameScrubber.enabled ? palette.darkerShade : palette.darkShade
                        highlightColor: frameScrubber.hoveredHandle === handleIndex ? palette.lighterShade : "transparent"
                        innerBorderColor: frameScrubber.__activeHandle === handleIndex && frameScrubber.activeFocus ? palette.highlightShade : "transparent"
                        y: -defaultSpacing.standardBorderSize
                        Item {
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            height: defaultSpacing.minimumRowHeight * 2
                            width: 2
                            z: -1
                            Rectangle {
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                anchors.left: parent.left
                                width: 1
                                color: palette.darkestShade
                            }
                            Rectangle {
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                anchors.right: parent.right
                                width: 1
                                color: palette.lightestShade
                            }
                        }
                    }
                }
            }

            Item {
                Layout.preferredWidth: defaultSpacing.doubleMargin
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
            }
        } //thirdRow

        RowLayout {
            id: fourthRow
            Layout.fillWidth: true
            Layout.maximumWidth: firstRow.width
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            visible: animated

            WGLabel {
                id: currentTimeLabel
                objectName: "currentTimeLabel"
                Layout.minimumWidth: contentWidth + defaultSpacing.rowSpacing
                Layout.maximumWidth: contentWidth + defaultSpacing.rowSpacing
                Layout.preferredHeight: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: " Current Time: "
            }

            // TODO: Make this show the current time and change the scrubber position when edited
            WGNumberBox {
                id: currNumBox
                objectName: "numberBox_currTime"

                Layout.preferredHeight: parent.height
                Layout.preferredWidth: 80

                value: currentTime
                minimumValue: 0.0
                maximumValue: duration
                stepSize: 0.01
                decimals: 1
                horizontalAlignment: Text.AlignHCenter
                readOnly: true

                onEditingFinished: {
                    currentTime = value
                }
            }

            WGLabel {
                id: durationLabel
                objectName: "durationLabel"
                Layout.minimumWidth: contentWidth + defaultSpacing.rowSpacing
                Layout.maximumWidth: contentWidth + defaultSpacing.rowSpacing
                Layout.preferredHeight: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: " Duration: "
            }

            // TODO: Make this change the total duration of both sliders when edited
            WGNumberBox {
                id: durNumBox
                objectName: "numberBox_duration"

                Layout.preferredHeight: parent.height
                Layout.preferredWidth: currNumBox.width

                value: duration
                minimumValue: 0.0
                maximumValue: 9999.0
                stepSize: 0.01
                decimals: 1
                horizontalAlignment: Text.AlignHCenter
                readOnly: true

                onEditingFinished: {
                    duration = value
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height
            }


            WGPushButton {
                id: loopToggle
                iconSource: "icons/repeat_16x16.png"
                checkable: true
                checked: false
                text: "Loop"

                onClicked: {
                    looping = loopToggle.checked
                }
            }

        } //fourthRow
    } //column
}
