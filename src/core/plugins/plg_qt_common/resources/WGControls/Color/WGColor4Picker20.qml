import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Layouts 2.0
import WGControls.Global 2.0

FocusScope {
    id: colorLayout
    objectName: "WGColor4Picker"
    WGComponent { type: "WGColor4Picker20" }
    implicitHeight: defaultSpacing.minimumRowHeight
    enabled: true
    implicitWidth: colorButtonFrame.width + numberBoxesFrame.width + defaultSpacing.standardMargin

    property bool showAlpha: true
    property bool hasMultipleValues: false
    property bool isReadOnly: false
    property vector4d color: Qt.vector4d(0, 0, 0, 1)

    function updateColorValue() {
        setValueHelper(colorLayout, "color", Qt.vector4d(boxX.value / 255.0,
                                                         boxY.value / 255.0,
                                                         boxZ.value / 255.0,
                                                         showAlpha ? boxW.value / 255.0 : 1.0))
    }

    RowLayout {
        anchors.fill: parent
        Item {
            id: colorButtonFrame
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.minimumWidth: 80
            Layout.maximumWidth: 80

            WGColorButton {
                id: colButton
                objectName: "colorButton"
                color: colorLayout.color
                anchors.left: parent.left
                width: 40
                height: parent.height
                defaultColorDialog: false
                multipleValues: hasMultipleValues
                showAlphaChannel: showAlpha
                useHDR: false

                focus: true

                onClicked: {
                    openDialog()
                }

                onColorChosen: {
                    var newColor;
                    if(showAlpha)
                    {
                        newColor = Qt.vector4d(selectedColor.x, selectedColor.y, selectedColor.z, selectedColor.w);
                    }
                    else
                    {
                        newColor = Qt.vector4d(selectedColor.x, selectedColor.y, selectedColor.z, 1);
                    }
                    setValueHelper(colorLayout, "color", newColor)
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
        }

        Item {
            id: numberBoxesFrame
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
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
                        value: colorLayout.color.x * 255
                        minimumValue: 0
                        maximumValue: 255
                        multipleValues: hasMultipleValues
                        readOnly: isReadOnly

                        onEditingFinished: {
                           updateColorValue()
                        }
                    },
                    WGNumberBox {
                        id: boxY
                        objectName: "numberBox_Y"
                        value: colorLayout.color.y * 255
                        minimumValue: 0
                        maximumValue: 255
                        multipleValues: hasMultipleValues
                        readOnly: isReadOnly

                        onEditingFinished: {
                            updateColorValue()
                        }
                    },
                    WGNumberBox {
                        id: boxZ
                        objectName: "numberBox_Z"
                        value: colorLayout.color.z * 255
                        minimumValue: 0
                        maximumValue: 255
                        multipleValues: hasMultipleValues
                        readOnly: isReadOnly

                        onEditingFinished: {
                            updateColorValue()
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

                value: !showAlpha ? 255 : colorLayout.color.w * 255
                minimumValue: 0
                maximumValue: 255
                multipleValues: hasMultipleValues
                readOnly: isReadOnly
                hasArrows: false
                horizontalAlignment: Text.AlignHCenter

                onEditingFinished: {
                    updateColorValue()
                }
            }
        }

        Item{
            Layout.fillWidth: true
        }
    }
}
