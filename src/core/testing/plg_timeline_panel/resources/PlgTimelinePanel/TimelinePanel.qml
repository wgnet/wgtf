import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.2

import WGControls 1.0
import WGControls 2.0

/*!
    \brief Timeline Panel WIP.
*/

WGPanel {
    id: timelinePanel
    objectName: "TimelinePanel"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.minimumWidth: 400

    property var title: qsTr( "Timeline Panel" )

    ListModel {
        id: timelineModel

        ListElement {
            name: "Component 1"
            type: "fullBar"
            barColor: "#666666"
            rowSpan: 1
        }
        ListElement {
            name: "Condition 1"
            text: "if (life == 0)"
            type: "textBox"
            barColor: "#6666DD"
            rowSpan: 1
        }
        ListElement {
            name: "Health"
            type: "frameSlider"
            barColor: "#22EE22"
            eventName: "set"
            eventProperty: "health"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0
                    eventValue: "0"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Model Property"
            type: "frameSlider"
            barColor: "#3333BB"
            eventName: "set"
            eventProperty: "baseModel"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.0
                    eventValue: "hide"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Model Property"
            type: "frameSlider"
            barColor: "#3333BB"
            eventName: "set"
            eventProperty: "wreckModel"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "show"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Particles"
            type: "frameSlider"
            barColor: "#BB33BB"
            eventName: "set"
            eventProperty: "particleSpawn"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "true"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Sound"
            type: "barSlider"
            startTime: 0.1
            endTime: 2.2
            barColor: "#3FA9F5"
            eventProperty: "file"
            eventAction: "="
            eventValue: "explosion.fsb"
            rowSpan: 1
        }
        ListElement {
            name: "Volume"
            type: "frameSlider"
            barColor: "#3FA9F5"
            eventName: "set"
            eventProperty: "volume"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "0"
                    type: "linear"
                },
                ListElement {
                    time: 0.9
                    eventValue: "100"
                    type: "linear"
                },
                ListElement {
                    time: 1.6
                    eventValue: "100"
                    type: "linear"
                },
                ListElement {
                    time: 2.2
                    eventValue: "0"
                    type: "linear"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Condition 2"
            text: "if (componentLife == 0 && onFire)"
            barColor: "#6666DD"
            type: "textBox"
            rowSpan: 1
        }
        ListElement {
            name: "Health"
            type: "frameSlider"
            barColor: "#22EE22"
            eventName: "set"
            eventProperty: "health"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0
                    eventValue: "0"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Model Property"
            type: "frameSlider"
            barColor: "#3333BB"
            eventName: "set"
            eventProperty: "baseModel"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.0
                    eventValue: "hide"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Model Property"
            type: "frameSlider"
            barColor: "#3333BB"
            eventName: "set"
            eventProperty: "objectWreckModel"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "show"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Model Property"
            type: "frameSlider"
            barColor: "#3333BB"
            eventName: "set"
            eventProperty: "componentModel"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "show"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Physics"
            type: "frameSlider"
            barColor: "#BB3333"
            eventName: "set"
            eventProperty: "impulse"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "(0,0,200)"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Particles"
            type: "frameSlider"
            barColor: "#BB33BB"
            eventName: "set"
            eventProperty: "particleSpawn"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "true"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Sound"
            type: "barSlider"
            startTime: 0.1
            endTime: 3
            barColor: "#3FA9F5"
            eventProperty: "file"
            eventAction: "="
            eventValue: "cookoff.fsb"
            rowSpan: 1
        }
        ListElement {
            name: "Volume"
            type: "frameSlider"
            barColor: "#3FA9F5"
            eventName: "set"
            eventProperty: "volume"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: 100
                    type: "boolean"
                },
                ListElement {
                    time: 3.0
                    eventValue: 0
                    type: "boolean"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Component 2"
            type: "fullBar"
            barColor: "#666666"
            rowSpan: 1
        }
        ListElement {
            name: "Condition 1"
            text: "if (componentLife == 0 && speed >= 10)"
            type: "textBox"
            barColor: "#6666DD"
            rowSpan: 1
        }
        ListElement {
            name: "Physics"
            type: "frameSlider"
            barColor: "#BB3333"
            eventName: "set"
            eventProperty: "speed"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0
                    eventValue: "0"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Model Property"
            type: "frameSlider"
            barColor: "#3333BB"
            eventName: "set"
            eventProperty: "componentModel"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.0
                    eventValue: "hide"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Model Property"
            type: "frameSlider"
            barColor: "#3333BB"
            eventName: "set"
            eventProperty: "wreckedComponentModel"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "show"
                    type: "constant"
                }
            ]
            rowSpan: 1
        }
        ListElement {
            name: "Sound"
            type: "barSlider"
            startTime: 0.1
            endTime: 1.5
            barColor: "#3FA9F5"
            eventProperty: "file"
            eventAction: "="
            eventValue: "componentBreak.fsb"
            rowSpan: 1
        }
        ListElement {
            name: "Volume"
            type: "frameSlider"
            barColor: "#3FA9F5"
            eventName: "set"
            eventProperty: "volume"
            eventAction: "="
            keyFrames: [
                ListElement {
                    time: 0.1
                    eventValue: "0"
                    type: "linear"
                },
                ListElement {
                    time: 0.5
                    eventValue: "100"
                    type: "linear"
                },
                ListElement {
                    time: 1.5
                    eventValue: "0"
                    type: "linear"
                }
            ]
            rowSpan: 1
        }
    }

    RowLayout {
        anchors.fill: parent

        // This is just a placeholder tree to show that one can be made to match the TimelineView
        Item {
            Layout.preferredWidth: 280
            Layout.fillHeight: true

            Text {
                anchors.bottom: tempListView.top
                anchors.bottomMargin: defaultSpacing.standardMargin
                text: "Placeholder Property Tree"
                color: "white"
                font.bold: true
            }

            ListView {
                id: tempListView
                anchors.fill: parent
                anchors.topMargin: gridCanvas.topMargin
                model: timelineModel

                spacing: 1

                onContentYChanged: {
                    gridCanvas.setViewYPosition(tempListView.contentY)
                }

                Connections {
                    target: gridCanvas
                    onYPositionChanged: {
                        if (tempListView.contentY != yPos)
                        {
                            tempListView.contentY = yPos
                        }
                    }
                }

                delegate: RowLayout {
                    height: defaultSpacing.minimumRowHeight
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: {
                        if (type == "fullBar")
                        {
                            return defaultSpacing.standardMargin
                        }
                        else if (type == "textBox")
                        {
                            return defaultSpacing.standardMargin * 3
                        }
                        else
                        {
                            return defaultSpacing.standardMargin * 6
                        }
                    }
                    anchors.rightMargin: defaultSpacing.standardMargin

                    spacing: 2

                    Rectangle {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight - defaultSpacing.doubleMargin
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight - defaultSpacing.doubleMargin

                        color: (typeof barColor != "undefined") ? barColor : "transparent"
                        border.width: 1
                        border.color: (typeof barColor != "undefined") ? "white" : "transparent"
                    }

                    Text {
                        Layout.fillWidth: true
                        color: "white"
                        text: name
                        font.bold: type == "fullBar" ? true : false
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    WGToolButton {
                        id: delButton
                        iconSource: "icons/delete_sml_16x16.png"
                        onClicked: {
                            deleteDialog.open()
                        }
                        visible: type != "fullBar" && type != "textBox"
                        Dialog {
                            id: deleteDialog
                            visible: false
                            title: "Delete item"
                            width: 160
                            height: 100

                            contentItem: Rectangle {
                                    color: palette.mainWindowColor
                                    ColumnLayout {
                                        anchors.top: parent.top
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.margins: defaultSpacing.standardMargin
                                        height: 80
                                        WGLabel {
                                            text: "Delete item " + model.name + " ?"
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.leftMargin: defaultSpacing.standardMargin
                                        }
                                        Item {
                                            Layout.fillHeight: true
                                            Layout.fillWidth: true
                                        }
                                        RowLayout {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: defaultSpacing.minimumRowHeight

                                            Item {
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                            }
                                            WGPushButton {
                                                text: "Yes"
                                                onClicked: {
                                                    deleteDialog.accepted()
                                                    deleteDialog.close()
                                                }
                                            }
                                            WGPushButton {
                                                text: "No"
                                                onClicked: {
                                                    deleteDialog.rejected()
                                                    deleteDialog.close()
                                                }
                                            }
                                        }
                                    }
                            }

                            onAccepted: {
                                timelineModel.remove(index)
                            }
                        }
                    }
                }
            }
        }

        WGTimelineControl {
            id: gridCanvas
            Layout.fillHeight: true
            Layout.fillWidth: true
            focus: true

            model: timelineModel

            timeScale: 5
        }
    }
}
