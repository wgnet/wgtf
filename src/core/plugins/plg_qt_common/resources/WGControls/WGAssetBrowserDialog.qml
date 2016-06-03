import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2



/*!
 \brief An AssetBrowser specific file dialog
*/

WGFileDialog {
    id: mainDialog
    objectName: "WGAssetBrowserDialog"

    onOpen: {
        abInstance.width = dWidth
        abInstance.height = dHeight
        //TODO: Reenable this and make this point to the currently selected file
        //abInstance.fileUrl = curValue
        abInstance.open()
    }

    onClose: {
        abInstance.close()
    }

    Dialog {
        id: abInstance
        modality: mainDialog.modality
        title: mainDialog.title

        //TODO: make this point to the currently selected AB instance file
        property url fileUrl: "file:///sample_file"

        contentItem: Rectangle {
            width: parent.width
            height: parent.height
            color: palette.mainWindowColor
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardMargin

                WGAssetBrowser {
                    objectName: "assetBrowser"
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    //TODO: Make this load a proper file system and AB stuff.
                    viewModel: view
                }

                WGExpandingRowLayout {
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.fillWidth: true

                    WGLabel {
                        text: "Selected File: "
                    }

                    WGTextBox {
                        objectName: "fileSelectBox"
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.fillWidth: true
                        readOnly: true
                        text: abInstance.fileUrl
                    }

                    WGPushButton {
                        objectName: "openButton"
                        text: "Open"
                        onClicked: {
                            abInstance.accepted()
                        }
                    }

                    WGPushButton {
                        objectName: "openButton"
                        text: "Cancel"
                        onClicked: {
                            abInstance.rejected()
                        }
                    }
                }
            }
        }

        onAccepted: {
            mainDialog.accepted(fileUrl)
        }

        onRejected: {
            mainDialog.rejected()
        }
    }
}
