import QtQuick 2.5
import QtQuick.Controls 1.4
import WGControls 1.0
import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
 \brief A low-overhead label object with a loader that then makes it editable.

Example:
\code{.js}

WGEditableLabel {
    id: editableText
    text: "myText"
    MouseArea {
        anchors.fill: parent
        onClicked: {
            editableText.editable = true
    }
}
\endcode
*/

WGLabel {
    id: textBox
    objectName: "WGEditableLabel"
    WGComponent { type: "WGEditableLabel20" }
    property Component editableComponent: WGTextBox {
        id: editBox
        anchors.fill: parent
        text: textBox.editText

        onEditingFinished: {
            textBox.editText = editBox.text
            textBox.stopEditing()
        }

        Rectangle{
            id: editContainer
            anchors.fill: parent
            color: palette.mainWindowColor
            z: -1
        }
    }

    property alias editTextBox: editLoader.item
    property string editText: ""
    text: editText

    function edit() {
        editLoader.sourceComponent = editableComponent
        editTextBox.visible = true
        editTextBox.forceActiveFocus();
        editTextBox.selectAll()
    }
    function stopEditing() {
        // can't immediately set the loader to null here or it can crash. Extra tick of setting it to !visible seems to help.
        editTextBox.visible = false
        editFinished();
        textBox.forceActiveFocus();
    }

    signal editFinished()

    Connections {
        target: editTextBox
        onVisibleChanged: {
            if(!visible)
            {
                editLoader.sourceComponent = null
            }
        }
    }

    Loader {
        id: editLoader
        anchors.fill: parent
    }
}
