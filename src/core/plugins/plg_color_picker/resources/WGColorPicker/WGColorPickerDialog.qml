import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGColorPicker 2.0

WGFileDialog {
    id: mainDialog
    objectName: "WGColorPickerDialog"
    WGComponent { type: "WGColorPickerDialog" }

    property bool showAlphaChannel: true

    onOpen: {
        cpInstance.width = dWidth
        cpInstance.height = dHeight
        colorPicker.initialColor = curValue
        cpInstance.open()
    }

    onClose: {
        cpInstance.close()
    }


    Dialog {
        id: cpInstance
        modality: mainDialog.modality
        title: mainDialog.title

        property color initialColor: "#FFFFFF"
        property color currentColor: initialColor

        contentItem: Rectangle {
            color: palette.mainWindowColor
            anchors.margins: defaultSpacing.standardMargin

            WGColorPicker {
                id: colorPicker
                objectName: "colorPicker"
                anchors.fill: parent

                showAlphaChannel: mainDialog.showAlphaChannel

                onOkClicked: {
                    mainDialog.accepted(colorPicker.currentColor)
                }

                onCancelClicked: {
                    mainDialog.rejected()
                }
            }
        }
    }
}
