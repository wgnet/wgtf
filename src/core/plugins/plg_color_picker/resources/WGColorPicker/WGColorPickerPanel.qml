import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0


import WGControls 1.0
import WGColorPicker 1.0

WGPanel {

    title: qsTr("Prototype Color Picker Panel")
    layoutHints: { 'test': 0.1 }

    color: palette.mainWindowColor

    width: 640
    height: 480

    WGColorButton {
        id: colButton
        color: "#FF0000"

        defaultColorDialog: false

        onClicked: {
            colorDialog.open(600,374,colButton.color)
        }

    }

    WGColorPickerDialog {
        id: colorDialog

        title: "Pick a Color"

        modality: Qt.WindowModal

        visible: false

        onAccepted: {
            colButton.color = selectedValue
            colorDialog.close()
        }
        onRejected: {
            colorDialog.close()
        }
    }
}
