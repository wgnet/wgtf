import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

WGPanel {
    id: basicDialog
    title: "Dialog Test Panel"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor
	
    WGColumnLayout{
        id: columnLayout
        anchors.fill: parent

        RowLayout{
            id: reflectedDialogRow
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.preferredHeight: treeParent.height

            property alias modal: showReflectedDialogModal.checked

            WGColumnLayout{
                Layout.fillWidth: false
                Layout.preferredWidth: internal.buttonWidth
                Layout.fillHeight: false
                Layout.preferredHeight: parent.height

                WGPushButton {
                    id: modifyDirectlyBtn
                    text: "Reflected Dialog\n(Modify Directly)"
                    Layout.fillHeight: true
                    Layout.fillWidth: false
                    Layout.preferredWidth: parent.width
                    onClicked: callReflectedDialogModifyDirect(reflectedDialogRow.modal)
                }

                WGPushButton {
                    id: modifyCopyBtn
                    text: "Reflected Dialog\n(Modify Copy)"
                    Layout.fillHeight: true
                    Layout.fillWidth: false
                    Layout.preferredWidth: parent.width
                    onClicked: callReflectedDialogModifyCopy(reflectedDialogRow.modal)
                }
            }

            Item {
                id: treeParent
                Layout.fillHeight: false
                Layout.fillWidth: true
                Layout.minimumWidth: 10
                Layout.preferredHeight: reflectedView.height

                WGPropertyTreeView {
                    id: reflectedView
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: contentHeight
                    model: getReflectedModel
                    columnWidths: [150, 200]
                    clamp: true
                }
            }

            WGCheckBox {
                id: showReflectedDialogModal
                text: "Modal"
                Layout.fillWidth: false
                Layout.fillHeight: false
                Layout.preferredHeight: parent.height
                checked: false
            }
        }
        
        RowLayout{
            id: basicDialogRow
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.preferredHeight: internal.rowHeight

            property alias modal: showBasicDialogModal.checked

            WGPushButton {
                id: showBasicDialogButton
                text: "Basic Dialog"
                Layout.fillHeight: true
                Layout.fillWidth: false
                Layout.minimumWidth: internal.buttonWidth
                onClicked: callBasicDialog(basicDialogRow.modal)
            }

            WGTextBox {
                id: basicDialogResultValue
                placeholderText: "dialog result"
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: basicDialogResult
                readOnly: true
            }

            WGCheckBox {
                id: showBasicDialogModal
                text: "Modal"
                Layout.fillHeight: true
                Layout.fillWidth: false
                checked: false
            }
        }

        RowLayout{
            id: customDialogRow
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.preferredHeight: internal.rowHeight

            property alias modal: showCustomDialogModal.checked

            WGPushButton {
                id: showCustomDialogButton
                text: "Custom Dialog"
                Layout.fillHeight: true
                Layout.fillWidth: false
                Layout.minimumWidth: internal.buttonWidth
                onClicked: callCustomDialog(customDialogRow.modal)
            }

            WGTextBox {
                id: customDialogResultValue
                placeholderText: "dialog result"
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: customDialogResult
                readOnly: true
            }

            WGCheckBox {
                id: showCustomDialogModal
                text: "Modal"
                Layout.fillHeight: true
                Layout.fillWidth: false
                checked: false
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    QtObject {
        id: internal
        property real rowHeight: 30
        property real buttonWidth: 150
    }
}
