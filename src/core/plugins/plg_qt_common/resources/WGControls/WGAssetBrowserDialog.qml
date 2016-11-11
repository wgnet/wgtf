import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Layouts 2.0


/*!
 \ingroup wgcontrols
 \brief An AssetBrowser specific file dialog
*/

WGFileDialog {
    id: mainDialog
    objectName: "WGAssetBrowserDialog"
    WGComponent { type: "WGAssetBrowserDialog" }

    property alias model : assetBrowser.model
    property alias nameFilters: assetBrowser.nameFilters
    property alias selectedNameFilter: assetBrowser.selectedNameFilter

    onOpen: {
        if (!assetBrowser.selectPath(curValue)) {
            assetBrowser.selectFolder(folder);
        }

        abInstance.width = dWidth
        abInstance.height = dHeight
        abInstance.open()
    }

    onClose: {
        abInstance.close()
    }

    Dialog {
        id: abInstance
        modality: mainDialog.modality
        title: mainDialog.title

        property var fileUrl: ""

        contentItem: Rectangle {
            color: palette.mainWindowColor

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardMargin

                WGAssetBrowser {
                    id: assetBrowser
                    objectName: "assetBrowser"
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    onCurrentPathChanged: {
                        abInstance.fileUrl = currentPath
                    }

                    onAssetAccepted: {
                        abInstance.fileUrl = assetPath
                        abInstance.accepted()
                        abInstance.close()
                    }
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
                            abInstance.close()
                        }
                    }

                    WGPushButton {
                        objectName: "openButton"
                        text: "Cancel"
                        onClicked: {
                            abInstance.rejected()
                            abInstance.close()
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
