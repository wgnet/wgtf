import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
    id: shortcutPanel

    property var sourceModel: shortcutModel
    property var id: "ShortcutConfig"
    title: "Actions Shortcut Configuration"
    layoutHints: { 'default': 0.1 }
    color: palette.mainWindowColor
    property int margin: 8
    width: mainLayout.implicitWidth + 2 * margin
    height: mainLayout.implicitHeight + 2 * margin
    Component.onCompleted: {
        View.minimumWidth = mainLayout.Layout.minimumWidth + 2 * margin;
        View.minimumHeight = mainLayout.Layout.minimumHeight + 2 * margin
        if(typeof qmlView.windowClosed !== "undefined") {
            qmlView.windowClosed.connect(onWindowClosed);
        }
    }

    function onWindowClosed()
    {
        // TODO: remove this workaround
        // explicitly call this because CollectionModel doesn't emit a signal when underly source changed
        sourceModel = Qt.binding( function() { return shortcutModel } );
    }
    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        WGScrollView {
            id: scrollView
            anchors.leftMargin: margin
            anchors.rightMargin: margin
            anchors.topMargin: margin
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: buttons.top
            anchors.bottomMargin: margin
            WGListView {
                id: listView
                columnSpacing: 2
                columnDelegates: [keyDelegate, valueDelegate]
                columnSequence: [0,0]
                model: sourceModel
                roles: ["key", "value"]
                currentIndex: 0
                clamp: false

                Component {
                    id: keyDelegate
                    Text {
                        text: itemData.key
                        color: palette.TextColor
                        width: itemWidth
                    }
                }

                Component { 
                    id: valueDelegate
                    WG1.WGTextBox {
                        text: itemData.value
                        width: itemWidth
                        onEditAccepted: {
                            itemData.value = text;
                        }
                    } 
                }
            }
        }
        RowLayout {
            id: buttons
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: margin
            anchors.rightMargin: margin
            anchors.bottomMargin: margin
            WG1.WGPushButton {
                id: reset
                anchors.left: parent.left
                text: "Reset to Default"
                onClicked: {
                    resetToDefault();
                    // TODO: remove this workaround
                    // explicitly call this because CollectionModel doesn't emit a signal when underly source changed
                    sourceModel = Qt.binding( function() { return shortcutModel } );
                }
            }
            Rectangle {
                id: space1
                Layout.fillWidth: true
                height: reset.height
                color: "transparent"
            }
            WG1.WGPushButton {
                id: ok
                anchors.right: space2.left
                text: "Ok"
                onClicked: {
                    applyChanges();
                    View.close();
                }
            }
            Rectangle {
                id: space2
                anchors.right: cancel.left
                width: 2
                height: reset.height
                color: "transparent"
            }
            WG1.WGPushButton {
                id: cancel
                anchors.right: parent.right
                text: "Cancel"
                onClicked: {
                    View.close();
                }
            }
        }
    }
}