import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import WGControls 1.0


Item {
    id: root
    property var id: "newProjectDlg"
    property var title: "Choose a project name"
    width: 270
    height: 60

    Connections {
        target: textBox
        onEditAccepted: {
            var isOk = isProjectNameOk( textBox.text );
            if(isOk)
            {
                okButton.enabled = true;
            }       
        }
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
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: ""
                    placeholderText: "Input a project name here"
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
                    enabled: false
                    onClicked: {
                        setNewProjectName(textBox.text);
                        textBox.text = "";
                        okButton.enabled = false;
                        closeWindow( id );
                    }
                }

                WGPushButton {
                    id: cancelButton
                    text: "Cancel"
                    Layout.preferredWidth: 60
                    checkable: false
                    onClicked: {
                        setNewProjectName("");
                        textBox.text = "";
                        okButton.enabled = false;
                        closeWindow( id );
                    }
                }
            }
        }
    }
}

