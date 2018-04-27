import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Private 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

Item
{
    id: selectFolderDialog
    objectName: "WGFolderBrowserDialog"
    WGComponent { type: "WGFolderBrowserDialog" }
    property var model: null
    signal folderSelected(string folder)
    property alias modality: fbInstance.modality

    function open(dWidth, dHeight) {
        fbInstance.width = dWidth
        fbInstance.height = dHeight
        fbInstance.open()
    }

    function close() {
        fbInstance.close()
    }
    
    Dialog {
        id: fbInstance
        title: "Select a Folder"
        property var selectedFolder: ""
        contentItem: Rectangle {
            color: palette.mainWindowColor
            //implicitWidth: 400
            //implicitHeight: 600

            WGColumnLayout{
                id: columnLayout
                anchors.fill: parent
                anchors.margins: defaultSpacing.doubleMargin
                WGTextBoxFrame {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    WGScrollView {
                        anchors.fill: parent
                        WGTreeView {
                            id: folderTreeView
                            columnSpacing: 1
                            columnDelegates: [folderDelegate]
                            model: selectFolderDialog.model
                            clamp: true

                            onItemPressed: {
                                var pressedItem = folderTreeView.view.extendedModel.indexToItem(rowIndex);
                                fbInstance.selectedFolder = pressedItem.value;
                            }
                        }
                    }

                    Component {
                        id: folderDelegate
                        WGLabel {
                            objectName: "folderDelegate" + text
                            text: itemData != null ? itemData.display : ""
                        }
                    }
                }

                Rectangle{
                    Layout.fillWidth: true
                    height: defaultSpacing.standardMargin
                    color: palette.mainWindowColor
                }
                WGSeparator{
                    Layout.fillWidth: true
                }
                Rectangle{
                    Layout.fillWidth: true
                    height: defaultSpacing.standardMargin
                    color: palette.mainWindowColor
                }

                RowLayout{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    }
                    WGPushButton {
                        id: okButton
                        text: "Ok"
                        Layout.preferredWidth: cancelButton.width
                        enabled: folderTreeView.view.selectionModel.hasSelection
                        onClicked: {
                            if(fbInstance.selectedFolder != "")
                            {
                                 folderSelected(fbInstance.selectedFolder);
                            }
                            selectFolderDialog.close();
                        }
                    }

                    WGPushButton {
                        id: cancelButton
                        text: "Cancel"
                        onClicked: {
                            selectFolderDialog.close();
                        }
                    }
                }
            }
        }
    }
}
