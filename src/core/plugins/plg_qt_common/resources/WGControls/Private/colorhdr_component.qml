import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Layouts 1.0
import WGControls.Global 2.0
import WGControls.Color 2.0

import Qt.labs.controls 1.0 as Labs

FocusScope {
    id: colorLayout
    WGComponent { type: "colorhdr_component" }
    objectName: typeof itemData.indexPath == "undefined" ? "colorhdr_component" :  itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
    implicitHeight: defaultSpacing.minimumRowHeight * 2
    implicitWidth: hsvLbl.width + splitTextFrame.width + (showAlpha ? (aLabel.width + hdrA.width) : 0)

    property var colorObj: itemData.object.getMetaObject(itemData.path, "HDRColor")

    property bool showAlpha: false

    QtObject {
        id: internal
        function setColor(color) {
            if(itemData.multipleValues) {
                beginUndoFrame();
                itemData.value = color;
                endUndoFrame();
            } else {
                itemData.value = color;
            }
        }
    }

    WGAbstractColor {
        id: absColor
        height: parent.height
        anchors.left: parent.left
        anchors.right: parent.right

        rgbaColor: (typeof itemData.value == "undefined") && itemData.multipleValues ? Qt.vector4d(0,0,0,1) :
                           showAlpha ? itemData.value : Qt.vector4d(itemData.value.x, itemData.value.y, itemData.value.z,1)

        altColorSpace: "HSV"

        decimalPlaces: 3

        property real brightnessRange: 1.0

        onChangeColor: {
            internal.setColor(col);
            if (absColor.altColor.z <= 1.0)
            {
                absColor.brightnessRange = 1.0
            }
            else if (absColor.altColor.z <= 10.0)
            {
                absColor.brightnessRange = 10.0
            }
            else
            {
                absColor.brightnessRange = 100.0
            }
        }

        tonemap: function(col) {
            if(colorObj == null) {
                return Qt.vector3d(Math.min(col.x, 1),Math.min(col.y, 1),Math.min(col.z, 1))
            }
            var ldr = colorObj.tonemap(Qt.vector4d(col.x,col.y,col.z,1));
            return Qt.vector3d(ldr.x,ldr.y,ldr.z);
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            RowLayout {
                id: firstRow
                Layout.fillWidth: true
                Layout.maximumWidth: colorLayout.implicitWidth
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                spacing: 0

                WGLabel {
                    id: hsvLbl
                    objectName: "colorButton_Label"
                    Layout.minimumWidth: 40
                    Layout.maximumWidth: 40
                    Layout.preferredHeight: parent.height
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: " HSV: "
                }

                WGSplitTextFrame {
                    id: splitTextFrame
                    Layout.preferredHeight: parent.height
                    Layout.preferredWidth: 180
                    boxList: [
                        WGNumberBox {
                            id: hdrH
                            value: hDc.value
                            stepSize: hDc.stepSize
                            decimals: hDc.decimals
                            minimumValue: hDc.minimumValue
                            maximumValue: hDc.maximumValue
                            multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                            readOnly: itemData.readOnly

                            WGDataConversion {
                                id: hDc
                                value: (typeof itemData.value == "undefined") && itemData.multipleValues ? 0 : dataToValue(absColor.altColor.x)
                                stepSize: 1
                                decimals: absColor.decimalPlaces
                                minimumValue: 0
                                maximumValue: 360
                                autoChangeData: false
                                round: absColor.round
                                dataToValue: function(val) {
                                    return val * 360
                                }
                                valueToData: function(val) {
                                    return val / 360
                                }

                                onChangeValue: {
                                    if(round(valueToData(val)) != round(absColor.altColor.x))
                                    {
                                        absColor.changeAltColor(Qt.vector3d(valueToData(val),absColor.altColor.y,absColor.altColor.z))
                                    }
                                }
                            }
                            onEditingFinished: {
                                hDc.changeValue(hdrH.value)
                            }
                        },
                        WGNumberBox {
                            id: hdrS
                            value: absColor.altColor.y
                            stepSize: 0.01
                            decimals: absColor.decimalPlaces
                            minimumValue: 0
                            maximumValue: 1
                            multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                            readOnly: itemData.readOnly

                            onEditingFinished: {
                                absColor.changeAltColor(Qt.vector3d(absColor.altColor.x,hdrS.value,absColor.altColor.z))
                            }
                        },
                        WGNumberBox {
                            id: hdrV
                            value: absColor.altColor.z
                            stepSize: 0.1
                            decimals: absColor.decimalPlaces
                            minimumValue: 0
                            maximumValue: 100
                            multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                            readOnly: itemData.readOnly

                            onEditingFinished: {
                                absColor.changeAltColor(Qt.vector3d(absColor.altColor.x,absColor.altColor.y,hdrV.value))
                            }
                        }
                    ]
                }

                WGLabel {
                    id: aLabel
                    objectName: "a_Label"
                    Layout.minimumWidth: contentWidth + defaultSpacing.rowSpacing
                    Layout.maximumWidth: contentWidth + defaultSpacing.rowSpacing
                    Layout.preferredHeight: parent.height
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: " A: "
                    visible: showAlpha
                }

                WGNumberBox {
                    id: hdrA
                    objectName: "numberBox_W"
                    visible: showAlpha

                    Layout.preferredHeight: parent.height
                    Layout.preferredWidth: hdrS.width

                    value: absColor.rgbaColor.w
                    minimumValue: 0
                    maximumValue: 1
                    stepSize: 0.01
                    decimals: absColor.decimalPlaces
                    multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                    readOnly: itemData.readOnly
                    hasArrows: false
                    horizontalAlignment: Text.AlignHCenter

                    onEditingFinished: {
                        absColor.changeColor(Qt.vector4d(absColor.rgbaColor.x,absColor.rgbaColor.y,absColor.rgbaColor.z,hdrA.value))
                    }
                }
            }

            RowLayout {
                id: secondRow
                Layout.fillWidth: true
                Layout.maximumWidth: colorLayout.implicitWidth
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                spacing: 0

                WGColorButton {
                    id: colButton
                    objectName: "colorButton"
                    color: (typeof itemData.value == "undefined") && itemData.multipleValues ?  Qt.vector4d(0,0,0,1) : absColor.rgbaColor
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: parent.height
                    defaultColorDialog: false
                    multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
                    showAlphaChannel: colorLayout.showAlpha
                    useHDR: true
                    hue360: true

                    focus: true

                    tonemap: absColor.tonemap

                    onColorChosen: {
                        internal.setColor(selectedColor);
                    }

                    MouseArea {
                        id: doubleClickArea
                        anchors.fill: parent
                        hoverEnabled: false

                        property bool doubleCheck: false

                        onPressed: {
                            if(!doubleClickArea.doubleCheck)
                            {
                                doubleClickArea.doubleCheck = true
                                WGDoubleClickTimer.restartTimer()
                                mouse.accepted = false
                            }
                            else
                            {
                                makeFakeMouseRelease();
                                huePopup.close()
                                colButton.openDialog();
                                doubleClickArea.doubleCheck = false
                            }
                        }

                        Connections {
                            target: WGDoubleClickTimer
                            onTimerTriggered: {
                                doubleClickArea.doubleCheck = false;
                            }
                        }
                    }



                    Item {
                        anchors.fill: parent
                        anchors.margins: defaultSpacing.rowSpacing
                        clip: true

                        Image {
                            source: "icons/drop_down_arrow_16x16.png"
                            height: 16
                            width: 16
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                        }
                    }

                    WGPopup {
                        id: huePopup
                        openPolicy: openOnClick

                        closePolicy: Labs.Popup.OnPressOutsideParent | Labs.Popup.onEscape

                        WGHSVSquareHS {
                            id: hsSquare
                            rgbaColor: absColor.rgbaColor
                            brightnessRange: absColor.brightnessRange

                            width: defaultSpacing.minimumRowHeight * 5 + defaultSpacing.doubleMargin
                            height: width * 0.80

                            useHDR: true
                            tonemap: absColor.tonemap

                            showSlider: false

                            onChangeColor: {
                                if (absColor.rgbaColor != col)
                                {
                                    absColor.changeColor(col)
                                }
                            }
                        }
                    }
                }

                WGSlider {
                    id: valueSlider

                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.fillWidth: true

                    //This needs to have giant values or binding loop rounding errors break it
                    stepSize: 0.1
                    value: vDc.value
                    minimumValue: 0
                    maximumValue: 10000

                    customTickmarks: [0, 3333, 6666, 10000]
                    customTickmarkLabels: ["0", "1", "10", "100"]
                    showTickmarkLabels: true

                    onValueChanged: {
                        vDc.changeValue(valueSlider.value)
                    }

                    stickyValues: [0, 3333, 6666, 10000]
                    stickyMargin: 10

                    WGDataConversion {
                        id: vDc
                        minimumValue: 0
                        maximumValue: 10000
                        decimals: 1
                        stepSize: 0.01

                        value: dataToValue(absColor.altColor.z)
                        round: function round(num) {
                            return Number(Math.round(num+'e'+decimals)+'e-'+decimals);
                        }

                        // This is a massive hack... but it works.
                        dataToValue: function(val) {
                            var result = 0
                            if (val <= 1)
                            {
                                result =  val * 3333
                            }
                            else if (val > 1 && val <= 1.5)
                            {
                                result = (val * 666.6) + 2666.4
                            }
                            else if (val > 1.5 && val <= 10)
                            {
                                result = val * 333.3 + 3333
                            }
                            else if (val > 10 && val <= 20)
                            {
                                result = (val * 66.66) + 5999.4
                            }
                            else
                            {
                                result = val*33.34 + 6666
                            }
                            return round(result)
                        }
                        valueToData: function(val) {
                            var result = 0
                            if (val <= 3333)
                            {
                                result = val / 3333
                            }
                            else if (val > 3333 && val <= 3333 + 333.3)
                            {
                                result = (val - 2666.4) / 666.6
                            }
                            else if (val > 3333 + 333.3 && val <= 6666)
                            {
                                result = Math.max((val-3333)/333.3, 1.5)
                            }
                            else if (val > 6666 && val <= 6666 + 666.6)
                            {
                                result = (val - 5999.4) / 66.66
                            }
                            else
                            {
                                result = Math.max((val-6666)/33.34,20)
                            }
                            return round(result)
                        }

                        onChangeValue: {
                            if(round(valueToData(val)) != round(absColor.altColor.z))
                            {
                                absColor.changeAltColor(Qt.vector3d(absColor.altColor.x, absColor.altColor.y, valueToData(val)))
                            }
                        }
                    }
                } //slider
                Item {
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.minimumWidth: defaultSpacing.doubleBorderSize
                    Layout.maximumWidth: defaultSpacing.doubleBorderSize
                }
            } //secondRow
        } //column
    } //abstract color
}
