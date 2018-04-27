import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 2.0

WGFileDialog {
    id: mainDialog
    objectName: "WGDefaultColorPickerDialog"
    WGComponent { type: "WGDefaultColorPickerDialog20" }

    property bool showAlphaChannel: true

    function open(dWidth, dHeight, curValue) {
        cpInstance.currentColor = curValue
        cpInstance.width = dWidth
        cpInstance.height = dHeight
        cpInstance.open()
    }

    function close() {
        cpInstance.close()
        mainDialog.closed();
    }

    ColorDialog {
        id: cpInstance
        objectName: "colorPicker"
        showAlphaChannel: mainDialog.showAlphaChannel

        modality: mainDialog.modality

        onAccepted: {
            mainDialog.accepted(cpInstance.color)
            mainDialog.close();
        }

        onRejected: {
            mainDialog.rejected()
            mainDialog.close();
        }
    }
}
