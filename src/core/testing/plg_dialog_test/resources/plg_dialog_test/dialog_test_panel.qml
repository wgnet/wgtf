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

    property var source: getSource

    WGColumnLayout{
        id: columnLayout
        anchors.fill: parent

        RowLayout{
            id: reflectedDialogRow
            Layout.fillHeight: false
            Layout.fillWidth: true

            property alias modal: showReflectedDialogModal.checked

            WGColumnLayout{
                anchors.fill: parent

                WGPushButton {
                    id: modifyDirectlyBtn
                    text: "Reflected Dialog\n(Modify Directly)"
                    Layout.fillHeight: true
                    Layout.fillWidth: false
                    Layout.minimumWidth: internal.buttonWidth
                    onClicked: callReflectedDialog(reflectedDialogRow.modal, true)
                }

                WGPushButton {
                    id: modifyCopyBtn
                    text: "Reflected Dialog\n(Modify Copy)"
                    Layout.fillHeight: true
                    Layout.fillWidth: false
                    Layout.minimumWidth: internal.buttonWidth
                    onClicked: callReflectedDialog(reflectedDialogRow.modal, false)
                }
            }

            Item {
                Layout.minimumHeight: reflectedView.contentHeight
                Layout.minimumWidth: reflectedView.contentWidth
                Layout.fillHeight: true
                Layout.fillWidth: true
                WGPropertyTreeView {
                    id: reflectedView
                    anchors.fill: parent
                    model: getReflectedModel
                    columnWidths: [150, 200]
                    clamp: false
                }
            }

            WGCheckBox {
                id: showReflectedDialogModal
                text: "Modal"
                Layout.fillHeight: false
                Layout.fillWidth: false
                checked: false
            }
        }

        RowLayout{
            id: basicDialogRow
            Layout.fillHeight: false
            Layout.fillWidth: true
            height: internal.rowHeight

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
