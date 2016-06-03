import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import WGControls 1.0


/*  TODO
    Known issues:
    The color represented by the color of the button control does not match the color when you open the dialog.

/*!
    \brief Raised solid color button used to open a colour dialog

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

    /*! This property defines the starting colour to be used in the color slider
        The default value is \c "#999999"
    */
    property color color: "#999999"

    /*! This property determines whether the default qml color dialog is opened on click.
        The default value is \c true
    */
    property bool defaultColorDialog: true

    //Auto-sized widths
    implicitWidth: 40

    implicitHeight: defaultSpacing.minimumRowHeight

    // support copy&paste
    WGCopyable {
        id: copyableControl

        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( colourButton.colourVec )
            }

            onDataPasted : {
                colourButton.colourVec = data
                if(colourButton.colourVec != data)
                {
                    pasted = false;
                }
            }
        }

        onSelectedChanged : {
            if (selected)
            {
                selectControl( copyableObject )
            }
            else
            {
                deselectControl( copyableObject )
            }
        }
    }

    onClicked: {
        if (defaultColorDialog)
        {
            colorDialog.visible = true
        }
    }

    Rectangle {
        id: checkSquare
        anchors.fill: parent
        anchors.margins: defaultSpacing.rowSpacing
        visible: enabled && colorSquare.color.a != 1
        color: "transparent"
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

        color: parent.color
    }

    ColorDialog {
        id: colorDialog
        objectName: "colorDialog"
        title: "Choose a Color"
        visible: false
        onAccepted: {
            colorButton.color = colorDialog.color
        }
    }
}
