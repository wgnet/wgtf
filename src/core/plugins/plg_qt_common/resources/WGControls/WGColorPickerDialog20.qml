import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 2.0

WGFileDialog {
    id: mainDialog
    objectName: "WGColorPickerDialog"
    WGComponent { type: "WGColorPickerDialog20" }

    property bool showAlphaChannel: true
    property bool useHDR: false
    property bool hue360: false
    property var tonemap: function(col) { return col; }

    property color initialColor: "black"

    function open(dWidth, dHeight, curValue) {
        if (useHDR)
        {
            var max = Math.max(curValue.x, curValue.y, curValue.z)
            if (max <= 1.0)
            {
                colorPicker.brightnessRange = 1.0
            }
            else if (max > 1.0 && max <= 10.0)
            {
                colorPicker.brightnessRange = 10.0
            }
            else
            {
                colorPicker.brightnessRange = 100.0
            }
        }
        else {
            colorPicker.brightnessRange = 1.0
        }
        if (!showAlphaChannel)
        {
            curValue = Qt.vector4d(curValue.x, curValue.y,curValue.z, 1)
        }
        colorPicker.viewId = mainDialog.viewId
        colorPicker.viewPreference = mainDialog.viewPreference
        cpInstance.width = 680
        cpInstance.height = !useHDR && !showAlphaChannel ? 380 - defaultSpacing.minimumRowHeight * 2 : 380
        mainDialog.opened();
        cpInstance.open()
        colorPicker.currentColor = curValue
        colorPicker.initialColor = curValue
    }

    function close() {
        cpInstance.close()
        mainDialog.closed();
    }

    Dialog {
        id: cpInstance
        modality: mainDialog.modality
        title: mainDialog.title

        contentItem: Rectangle {
            color: palette.mainWindowColor
            anchors.margins: defaultSpacing.standardMargin

            WGColorPicker {
                id: colorPicker
                objectName: "colorPicker"
                anchors.fill: parent

                showAlphaChannel: mainDialog.showAlphaChannel
                useHDR: mainDialog.useHDR
                hue360: mainDialog.hue360
                tonemap: mainDialog.tonemap

                onOkClicked: {
                    mainDialog.accepted(colorPicker.currentColor)
                    mainDialog.close();
                }

                onCancelClicked: {
                    mainDialog.rejected()
                    mainDialog.close();
                }
            }
        }
    }
}
