import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import WGControls 1.0


Item {
    id: root
    property var id: "showModal"
    property var title: "Custom Qml Window"
    width: 150
    height: 80
    property string defaultText: "default text"
    Component.onCompleted: {
        defaultText = textBox.text
    }

    WGFrame{
        id: mainFrame
        anchors.fill: parent

        WGColumnLayout {
            id: mainColumnLayout
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardMargin

            WGTextBoxFrame{
                id: editFrame
                Layout.fillHeight: true
                Layout.fillWidth: true
                WGTextBox{
                    id: textBox
                    text: "Click Ok to Close"
                }
            }

            WGExpandingRowLayout{
                id: buttons
                Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
                Layout.fillWidth: true

                WGPushButton {
                    id: okButton
                    text: "Ok"
                    Layout.preferredWidth: 60
                    checkable: false
                    onClicked: {
                        root.defaultText = textBox.text;
                        closeWindow( id );
                    }
                }

                WGPushButton {
                    id: cancelButton
                    text: "Cancel"
                    Layout.preferredWidth: 60
                    checkable: false
                    onClicked: {
                        textBox.text = root.defaultText;
                    }
                }
            }
        }
    }
}

