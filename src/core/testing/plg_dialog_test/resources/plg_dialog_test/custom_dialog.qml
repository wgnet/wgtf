import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import WGControls 2.0
import WGControls.Layouts 2.0

Rectangle {
    id: customDialog
    property var title: "Custom Dialog"
    property var id: "Custom Dialog"
    color: palette.mainWindowColor

    implicitWidth: 500
    implicitHeight: 300

    WGColumnLayout{
        id: columnLayout
        anchors.fill: parent

        WGMultiLineText{
            id: messageText
            text: "This is a dialog with a custom model.\nClick one of the options to close."
            height: 40
            Layout.fillWidth: true
        }

        WGSeparator{
            id:separator
            Layout.fillWidth: true
        }

        RowLayout{
            id: option1Row
            Layout.fillHeight: true
            Layout.fillWidth: true

            WGPushButton {
                id: option1Button
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "Pick Option A"
                onClicked: dialog.close(1)
            }
        }

        RowLayout{
            id: option2Row
            Layout.fillHeight: true
            Layout.fillWidth: true

            WGPushButton {
                id: option2Button
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "Pick Option B"
                onClicked: dialog.close(2)
            }
        }

        RowLayout{
            id: option3Row
            Layout.fillHeight: true
            Layout.fillWidth: true

            WGPushButton {
                id: option3Button
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "Pick Option C"
                onClicked: dialog.close(3)
            }
        }
    }
}
