import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Timeline 2.0
import WGControls.Canvas 2.0

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
    property var timelineModel: source

    onFocusChanged: {
        gridCanvas.focus = focus;
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
                                timelineModel.removeItem(index)
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
