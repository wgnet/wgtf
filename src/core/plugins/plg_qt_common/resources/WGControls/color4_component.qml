import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2


// TODO:
// * Labels should probably be set via C++/data somehow
// * Fix magic width numbers & alignments
// * Assign proper border/rectangle colours from palette
// * Vertically align label text with number text boxes.
// * Number text boxes should be sized for 3-digit values but clip decimal places unless editing
// * Fix tab focus

WGExpandingRowLayout {
    id: colorLayout
    objectName: typeof itemData.indexPath == "undefined" ? "color4_component" :  itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly

    property bool showAlpha: true

    function getColor(vectorColor) {
        if(showAlpha)
        {
            return Qt.rgba(vectorColor.x / 255, vectorColor.y / 255, vectorColor.z / 255, vectorColor.w / 255);
        }
        else
        {
            return Qt.rgba(vectorColor.x / 255, vectorColor.y / 255, vectorColor.z / 255, 1);
        }
    }

    function getVector(color) {
        if(showAlpha)
        {
            return Qt.vector4d(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
        }
        else
        {
            return Qt.vector3d(color.r * 255, color.g * 255, color.b * 255)
        }
    }

    anchors.left: parent.left
    anchors.right: parent.right

    Item {
        Layout.preferredHeight: parent.height
        Layout.minimumWidth: 80
        Layout.maximumWidth: 80

        WGColorButton {
            id: colButton
            objectName: "colorButton"
            color: getColor(itemData.value)
            anchors.left: parent.left
            width: 40
            height: parent.height
            defaultColorDialog: false
            multipleValues: itemData.multipleValues

            onClicked: {
                beginUndoFrame();
                // moving init ColorDialog's color property here since using color: getColor(itemData.value)
                // in ColorDialog, the alpha channel data is always 255 which may not equals itemData.value.w
                reflectColorDialog.color = getColor(itemData.value);
                reflectColorDialog.visible = true
            }

        }

        WGLabel {
            objectName: "colorButton_Label"
            anchors.left: colButton.right
            width: 40
            height: parent.height
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter

            text: {
                if(showAlpha)
                {
                    "RGBA:"
                }
                else
                {
                    "RGB:"
                }
            }
        }

        /*
         * don't use modal dialog since On MacOS the color dialog is only allowed to be non-modal.
         * see http://doc.qt.io/qt-5/qml-qtquick-dialogs-colordialog.html#modality-prop for detail info
        */
        ColorDialog {
            id: reflectColorDialog
            objectName: "colorDialog"
            title: "Please choose a color"
            showAlphaChannel: showAlpha

            onAccepted: {
                setValueHelper(colButton, "color", reflectColorDialog.color);
                var vector = getVector(reflectColorDialog.color);
                itemData.value = vector;
                endUndoFrame();
            }
            onCurrentColorChanged: {
                if (!Qt.colorEqual(reflectColorDialog.currentColor, getColor(itemData.value))) {
                    itemData.value = getVector(reflectColorDialog.currentColor)
                    setValueHelper(colButton, "color", reflectColorDialog.currentColor);
                }
            }
            onRejected: {
                setValueHelper(colButton, "color", reflectColorDialog.color);
                abortUndoFrame();
            }
        }
    }

    Item {
        Layout.preferredHeight: parent.height
        Layout.minimumWidth: 160 + defaultSpacing.rowSpacing
        Layout.maximumWidth: 160 + defaultSpacing.rowSpacing

        WGSplitTextFrame {
            id: splitTextFrame
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            height: parent.height
            width: 120
            boxList: [
                WGNumberBox {
                    id: boxX
                    objectName: "numberBox_X"
                    number: itemData.value.x
                    minimumValue: 0
                    maximumValue: 255
                    multipleValues: itemData.multipleValues
                    readOnly: itemData.readOnly

                    onNumberChanged: {
                        itemData.value.x = number
                    }
                },
                WGNumberBox {
                    id: boxY
                    objectName: "numberBox_Y"
                    number: itemData.value.y
                    minimumValue: 0
                    maximumValue: 255
                    multipleValues: itemData.multipleValues
                    readOnly: itemData.readOnly

                    onNumberChanged: {
                        itemData.value.y = number
                    }
                },
                WGNumberBox {
                    id: boxZ
                    objectName: "numberBox_Z"
                    number: itemData.value.z
                    minimumValue: 0
                    maximumValue: 255
                    multipleValues: itemData.multipleValues
                    readOnly: itemData.readOnly

                    onNumberChanged: {
                        itemData.value.z = number
                    }
                }
            ]
        }

        WGNumberBox {
            id: boxW
            objectName: "numberBox_W"

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: splitTextFrame.right
            anchors.leftMargin: defaultSpacing.rowSpacing
            height: parent.height
            width: 40

            visible: showAlpha

            number: showAlpha ? itemData.value.w : 255
            minimumValue: 0
            maximumValue: 255
            multipleValues: itemData.multipleValues
            readOnly: itemData.readOnly
            hasArrows: false
            horizontalAlignment: Text.AlignHCenter

            onNumberChanged: {
                if(showAlpha)
                {
                    itemData.value.w = number
                }
            }
        }
    }

    Item{
        Layout.fillWidth: true
    }
}
