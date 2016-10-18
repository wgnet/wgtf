import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import WGControls 2.0
import WGControls.Layouts 2.0

Rectangle{
    id: basicDialog
    property var title: "Basic Dialog"
    property var id: "Basic Dialog"
    color: palette.mainWindowColor

    implicitWidth: 500
    implicitHeight: 300

    WGColumnLayout{
        id: columnLayout
        anchors.fill: parent

        WGMultiLineText{
            id: messageText
            text: "This is a basic custom dialog.\nClick one of the options to close."
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
                text: "Pick Option 1"
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
                text: "Pick Option 2"
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
                text: "Pick Option 3"
                onClicked: dialog.close(3)
            }
        }
    }
}
