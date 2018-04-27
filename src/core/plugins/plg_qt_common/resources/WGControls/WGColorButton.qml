import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Global 2.0

/*!
    \ingroup wgcontrols
    \brief Raised solid color button used to open a colour dialog

    \todo Known issues:
    The color represented by the color of the button control does not match the color when you open the dialog.

\code{.js}
WGColorButton {
    color: "red"
    Layout.fillWidth: true
}
\endcode
*/

WGPushButton {
    id: colorButton
    objectName: "WGColorButton"
    WGComponent { type: "WGColorButton" }

    /*! This property defines the starting colour to be used in the color slider
        The default value is \c "#999999"
    */
    property vector4d color: Qt.vector4d(0.5,0.5,0.5,1)

    /*! This property determines whether the default qml color dialog is opened on click.
        The default value is \c true
    */
    property bool defaultColorDialog: false

    /*! This property determines whether the dialog shows alpha values.
        The default value is \c true
    */
    property bool showAlphaChannel: true

    property bool useHDR: false

    property var tonemap: function(col) { return col; }

    property bool hue360: false

    // Signals
    signal colorChosen(var selectedColor)
    signal colorRejected()

    //Auto-sized widths
    implicitWidth: 40

    implicitHeight: defaultSpacing.minimumRowHeight

    /*! internal */
    property var __dialogInstance: null

    /*! This function opens the desired dialog box.
    */
    function openDialog() {
        if (defaultColorDialog) {
            WGDialogs.defaultColorPickerDialog.close()
            __dialogInstance = WGDialogs.defaultColorPickerDialog

            //MacOS default color picker cannot be modal.
            __dialogInstance.modality = Qt.NonModal
            __dialogInstance.showAlphaChannel = colorButton.showAlphaChannel
            __dialogInstance.open(600, 380, Qt.rgba(colorButton.color.x,
                                                    colorButton.color.y,
                                                    colorButton.color.z,
                                                    showAlphaChannel ? colorButton.color.w : 1))
        }
        else {
            __dialogInstance = WGDialogs.customColorPickerDialog
            if(typeof viewId != "undefined")
            {
                __dialogInstance.viewId = viewId
            }
            if(typeof viewPreference != "undefined")
            {
                __dialogInstance.viewPreference = viewPreference
            }
            __dialogInstance.modality = Qt.ApplicationModal
            __dialogInstance.useHDR = colorButton.useHDR
            __dialogInstance.tonemap = colorButton.tonemap
            __dialogInstance.showAlphaChannel = colorButton.showAlphaChannel
            __dialogInstance.hue360 = colorButton.hue360
            __dialogInstance.open(600, 380, colorButton.color)
        }
    }

    /*! This function closes the desired dialog box depending on whether useAssetBrowser == true or not.
    */
    function closeDialog() {
        __dialogInstance.close()
    }

    Connections {
        target: __dialogInstance
        ignoreUnknownSignals: true

        onAccepted: {
            colorChosen(selectedValue)
        }

        onRejected: {
            colorRejected()
        }

        onClosed: {
            __dialogInstance = null
        }
    }

    Item {
        id: checkSquare
        anchors.fill: parent
        anchors.margins: defaultSpacing.rowSpacing
        visible: enabled && colorButton.color.w != 1
        Image {
            source: "icons/bw_check_6x6.png"
            fillMode: Image.Tile
            anchors.fill: parent
        }
    }

    //colour square over the top of the standard button frame.
    Rectangle {
        id: colorSquare
        anchors.fill: parent
        anchors.margins: defaultSpacing.rowSpacing

        opacity: enabled ? 1 : 0.4

        color: {
            var newCol = tonemap(Qt.vector3d(colorButton.color.x,colorButton.color.y,colorButton.color.z))
            return Qt.rgba(newCol.x, newCol.y, newCol.z, colorButton.color.w)
        }
    }

    Item {
        id: multipleValuesDisplay
        anchors.fill: parent
        anchors.margins: defaultSpacing.rowSpacing
        visible: multipleValues
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                    GradientStop { position: 0.0; color: "red" }
                    GradientStop { position: 0.5; color: "green" }
                    GradientStop { position: 1.0; color: "blue" }
                }
        }
    }
}
