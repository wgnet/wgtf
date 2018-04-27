import QtQuick 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

Rectangle{
    id: reflectedDialog
    property var title: "Reflected Object"
    property var id: "Reflected Object"
    color: palette.mainWindowColor
    implicitWidth: 350
    implicitHeight: 200
	property var dataEdited: getDataEdited

    WGColumnLayout{
        id: columnLayout
        anchors.fill: parent

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: palette.lighterShade
            WGPropertyTreeView {
                id: testTreeView
                anchors.fill: parent
                model: getModel
                columnWidths: [150, 200]
            }
        }

        WGSeparator{
            Layout.fillWidth: true
        }

        RowLayout{
            Layout.fillHeight: false
            Layout.fillWidth: true

            WGPushButton {
                id: saveButton
                Layout.fillHeight: false
                Layout.minimumWidth: columnLayout.width / 2
                text: "Save"
                enabled: false
                onClicked: {
                    dialog.close(1);
                }
            }

            WGPushButton {
                id: exitButton
                Layout.fillHeight: false
                Layout.minimumWidth: columnLayout.width / 2
                text: "Cancel"
                onClicked: {
                    if(dataEdited) {
                        onCloseRequested.visible = true;
                    }
                    else {
                        dialog.close(0);
                    }
                }
            }
        }
    }

    MessageDialog {
        id: onCloseRequested
        icon: StandardIcon.Warning
        title: "Warning"
        text: "Do you want to discard your changes?"
        standardButtons: StandardButton.Yes | StandardButton.No
        visible: false

        onYes: {
            visible = false;
            dialog.close(0);
        }
        onNo: {
            visible = false;
        }
    }

	onDataEditedChanged: {
		exitButton.text = dataEdited ? "Cancel" : "Close";
		saveButton.enabled = dataEdited ? true : false;
	}
}
