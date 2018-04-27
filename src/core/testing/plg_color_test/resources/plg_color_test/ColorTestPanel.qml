import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Private 2.0
import WGControls.Layouts 2.0
import WGControls.Color 2.0
import WGControls.Global 2.0

WGPanel {
    id: colorTestPanel
    title: "Color Test Panel"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    property vector4d currentColor: Qt.vector4d(0.0,1,1,1)
    property vector4d initialColor: Qt.vector4d(0.0,1,1,1)
    property vector4d previewedColor: currentColor
    property bool showPreviewColor: false
    property bool showAlphaChannel: true

    property bool useHDR: brightnessRange > 1.0

    property bool useHexValue: false
    property real brightnessRange: 1.0

    property var tonemap: function(col) {
        return col
    }

    Rectangle {
        anchors.fill: parent
        color: palette.darkShade
    }

    /* list of color controls to be placed in gridView

       **** IF ADDING/REMOVING CONTROLS, UPDATE THE MODEL OF THE REPEATER BELOW ****

       Using controlList.data.length does work, but it gives a "non-notifiable property" warning.
       Updating the model onCompleted breaks the grid cell sizes. */

    Item {
        id: controlList

        Component {
            WGAbstractColor {
                objectName: "Read Only RGBA Data"
                anchors.centerIn: parent
                height: defaultSpacing.minimumRowHeight * 4
                width: parent.width * 0.70
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardMargin
                    WGTextBox {
                        id: boxX
                        text: currentColor.x
                        readOnly: true
                        Layout.fillWidth: true

                        Text {
                            anchors.right: parent.left
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            color: palette.TextColor
                            text: "R: "
                        }
                    }
                    WGTextBox {
                        id: boxY
                        text: currentColor.y
                        readOnly: true
                        Layout.fillWidth: true

                        Text {
                            anchors.right: parent.left
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            color: palette.TextColor
                            text: "G: "
                        }
                    }
                    WGTextBox {
                        id: boxZ
                        text: currentColor.z
                        readOnly: true
                        Layout.fillWidth: true

                        Text {
                            anchors.right: parent.left
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            color: palette.TextColor
                            text: "B: "
                        }
                    }
                    WGTextBox {
                        id: boxW
                        text: currentColor.w
                        readOnly: true
                        Layout.fillWidth: true

                        Text {
                            anchors.right: parent.left
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            color: palette.TextColor
                            text: "A: "
                        }
                    }
                }
            }
        }

        Component {
            WGHDRSelector {
                height: defaultSpacing.minimumRowHeight * 2
                anchors.centerIn: parent
            }
        }
        Component {
            WGColorPreview {
                height: width
                anchors.centerIn: parent
            }
        }
        Component {
            WGRgbSlider {
                height: defaultSpacing.minimumRowHeight * 4
                anchors.centerIn: parent

                WGCheckBox {
                    anchors.top: parent.bottom
                    text: "Use RGB Hex Values"
                    onClicked: {
                        var tempCol = currentColor
                        useHexValue = !useHexValue
                        checked = useHexValue
                        currentColor = tempCol
                    }
                }
            }
        }
        Component {
            WGHueSlider {
                objectName: "WGHueSlider (HSV)"
                height: defaultSpacing.minimumRowHeight * 3
                anchors.centerIn: parent
            }
        }
        Component {
            WGHueSlider {
                objectName: "WGHueSlider (HSL)"
                height: defaultSpacing.minimumRowHeight * 3
                anchors.centerIn: parent
                altColorSpace: "HSL"
            }
        }
        Component {
            WGHueSlider {
                objectName: "WGHueSlider (H as 360)"
                height: defaultSpacing.minimumRowHeight * 3
                anchors.centerIn: parent
                hue360: true
            }
        }
        Component {
            WGAlphaSlider {
                height: defaultSpacing.minimumRowHeight * 1
                anchors.centerIn: parent
            }
        }
        Component {
            WGHSVSquareHS {
                height: width
                anchors.centerIn: parent
            }
        }
        Component {
            WGHSVSquareSV {
                height: width
                anchors.centerIn: parent
            }
        }
        Component {
            WGColorWheel {
                objectName: "WGColorWheel (HSL)"
                height: width
                anchors.centerIn: parent
            }
        }
        Component {
            WGShadeGrid {
                height: width
                anchors.centerIn: parent
            }
        }
        Component {
            WGColorPalette {
                height: defaultSpacing.minimumRowHeight * 3
                anchors.centerIn: parent

                rows: 3
                columns: 8
            }
        }
        Component {
            WGHexColorTextBox {
                height: defaultSpacing.minimumRowHeight * 1
                anchors.centerIn: parent
            }
        }
        Component {
            WGPickScreenButton {
                height: defaultSpacing.minimumRowHeight * 1
                anchors.centerIn: parent
            }
        }
        Component {
            WGValueLogSlider {
                height: defaultSpacing.minimumRowHeight * 1
                anchors.centerIn: parent
            }
        }
    }

    WGScrollView {
        anchors.fill: parent
        contentItem: WGColumnLayout {
            id: columnLayout
            width: colorTestPanel.width

            Item {
                id: treeParent
                Layout.fillHeight: false
                Layout.fillWidth: true
                Layout.minimumWidth: 10
                Layout.preferredHeight: treeView.height

                WGPropertyTreeView {
                    id: treeView
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: contentHeight
                    model: getTreeModel
                    columnWidths: [150, 200]
                    clamp: true
                }
            }

            /* from here on is a fairly complicated but very awesome grid view that loads the controls in the controlList
                into a totally sweet labelled grid that hooks everything up automatically */
            GridLayout {
                id: grid
                Layout.fillWidth: true
                columns: Math.min(Math.max(3, Math.floor(grid.width / 200)), 6)

                Repeater {
                    id: gridRepeater

                    // **** MAKE SURE TO UPDATE THIS ****
                    model: 16
                    Loader {
                        id: delegateLoader
                        Layout.fillWidth: true
                        Layout.preferredHeight: width + defaultSpacing.minimumRowHeight
                        sourceComponent: gridDelegate
                        onLoaded: {
                            item.cellContents = controlList.data[index]
                        }

                        Connections {
                            target: item.cellLoader
                            ignoreUnknownSignals: true
                            onLoaded: {
                                item.cellItem.width = Qt.binding(function() { return delegateLoader.width * 0.90 })

                                item.cellItem.rgbaColor = Qt.binding(function() { return colorTestPanel.currentColor })
                                item.cellItem.useHDR = Qt.binding(function() { return colorTestPanel.useHDR })
                                item.cellItem.tonemap = Qt.binding(function() { return colorTestPanel.tonemap })
                                item.cellItem.showPreviewColor = Qt.binding(function() { return colorTestPanel.showPreviewColor })
                                item.cellItem.previewedColor = Qt.binding(function() { return colorTestPanel.previewedColor })

                                if (typeof item.cellItem.initialColor != "undefined")
                                {
                                    item.cellItem.initialColor = Qt.binding(function() { return colorTestPanel.initialColor })
                                }
                                if (typeof item.cellItem.useHexValue != "undefined")
                                {
                                    item.cellItem.useHexValue = Qt.binding(function() { return colorTestPanel.useHexValue })
                                }
                                if (typeof item.cellItem.brightnessRange != "undefined")
                                {
                                    item.cellItem.brightnessRange = Qt.binding(function() { return colorTestPanel.brightnessRange })
                                }
                            }
                        }
                        Connections {
                            target: item.cellItem
                            ignoreUnknownSignals: true
                            onChangeColor: {
                                if (colorTestPanel.currentColor != col)
                                {
                                    colorTestPanel.currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                                }
                            }

                            onPreviewColor: {
                                colorTestPanel.showPreviewColor = show
                                if (show && colorTestPanel.previewedColor != col)
                                {
                                    colorTestPanel.previewedColor = col
                                }
                            }
                            onChangeBrightness: {
                                colorTestPanel.brightnessRange = br
                            }
                        }
                    }
                }
            }

            Item {
                Layout.preferredHeight: defaultSpacing.standardMargin
                Layout.fillWidth: true
                WGSeparator {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            //Gradient Slider
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardBorderSize
                    color: palette.mainWindowColor

                    WGGradientSlider {
                        anchors.fill: parent
                        Component.onCompleted: {
                            createColorHandle(.25,handleStyle,__handlePosList.length, Qt.vector4d(1,0,0,1))
                            createColorHandle(.5,handleStyle,__handlePosList.length, Qt.vector4d(1,1,0,1))
                            createColorHandle(.75,handleStyle,__handlePosList.length, Qt.vector4d(1,1,1,1))
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: "WGGradientSlider (Double click handles to change color, Shift/Ctrl click to add/remove handles)"
                    color: palette.textColor
                    font.italic: true
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: gridDelegate

        ColumnLayout {
            property string cellText: "Placeholder"
            property Component cellContents
            property alias cellLoader: cellLoader
            property alias cellItem: cellLoader.item
            anchors.fill: parent

            onCellContentsChanged: {
                cellLoader.sourceComponent = cellContents
                cellText = cellLoader.item.objectName
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: width
                Rectangle {
                    id: cellFrame
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardBorderSize
                    color: palette.mainWindowColor

                    Loader {
                        id: cellLoader
                        anchors.centerIn: parent
                    }
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: cellText
                    color: palette.textColor
                    font.italic: true
                }
            }
        }
    }
}
