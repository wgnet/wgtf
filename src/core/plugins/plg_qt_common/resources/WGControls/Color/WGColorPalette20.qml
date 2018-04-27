import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \A grid of color swatches that can save new values and delete old ones.

 TODO: Make these values save persistantly.

Example:
\code{.js}
WGColorPalette {
    id: palette
    Layout.fillWidth: true
}
\endcode
*/

WGAbstractColor {
    id: colorPalette
    objectName: "WGColorPalette"
    WGComponent { type: "WGColorPalette20" }

    /*!
        The array of saved palette colors.

        Currently this is only saved per instance of the color picker.
    */
    property var savedColors: [Qt.vector4d(0,0,0,1),Qt.vector4d(1,1,1,1),Qt.vector4d(0.5,0.5,0.5,1),Qt.vector4d(1,0,0,1),Qt.vector4d(0,1,0,1),Qt.vector4d(0,0,1,1),Qt.vector4d(0,1,1,1),Qt.vector4d(1,0,1,1),Qt.vector4d(1,1,0,1)]

    /*!
        The number of rows in the palette
    */
    property alias rows: paletteGrid.rows
    /*!
        The number of columns in the palette
    */
    property alias columns: paletteGrid.columns

    /*!
        The maximum value of the brightness or r,g,b channels
    */
    property real brightnessRange: 1.0

    altColorSpace: "RGB"

    signal updatePalette()
    signal changeBrightness (var br)

    Rectangle {
        id: innerBorder
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height
        width: parent.width
        color: palette.darkColor

        GridLayout {
            id: paletteGrid
            anchors.fill: parent
            columnSpacing: 0
            rowSpacing: 0

            columns: !useHDR && showAlphaChannel ? 8 : 12
            rows: !useHDR && showAlphaChannel ? 3 : 2

            Repeater {
                model: 23
                Item {
                    id: swatchBorder
                    Layout.preferredWidth: height
                    Layout.preferredHeight: Math.floor((innerBorder.height - defaultSpacing.doubleBorderSize) / paletteGrid.rows)

                    property vector4d swatchColor: Qt.vector4d(0,0,0,0)
                    property bool containsColor: index < savedColors.length

                    Connections {
                        target: colorPalette
                        onUpdatePalette: {
                            containsColor = index < savedColors.length
                            if (containsColor)
                            {
                                swatchColor = savedColors[index]
                            }
                            else
                            {
                                swatchColor = Qt.vector4d(0,0,0,0)
                            }
                        }
                    }

                    Rectangle {
                        id: swatch
                        anchors.centerIn: parent
                        height: parent.height - defaultSpacing.doubleBorderSize
                        width: height
                        color: {
                            if (typeof swatchColor === "undefined")
                            {
                                return "transparent"
                            }
                            else
                            {
                                var tempCol = tonemap(Qt.vector3d(swatchColor.x,swatchColor.y,swatchColor.z))
                                return Qt.rgba(tempCol.x,tempCol.y,tempCol.z,swatchColor.w)
                            }
                        }
                        border.width: containsColor ? 0 : 1
                        border.color: containsColor ? "transparent" : palette.midLightColor

                        Image {
                            source: "icons/bw_check_6x6.png"
                            fillMode: Image.Tile
                            anchors.fill: parent
                            visible: containsColor
                            z: -1
                        }

                        MouseArea {
                            objectName: "paletteSwatchButton"
                            anchors.fill: parent
                            cursorShape: containsColor ? Qt.PointingHandCursor : Qt.ArrowCursor
                            enabled: containsColor
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            onClicked: {
                                // pick as new rgbaColor
                                if (mouse.button == Qt.LeftButton)
                                {
                                    changeColor(Qt.vector4d(swatchColor.x, swatchColor.y, swatchColor.z, swatchColor.w))
                                    var max = Math.max(swatchColor.x, swatchColor.y, swatchColor.z)
                                    if (max <= 1.0)
                                    {
                                        changeBrightness(1.0)
                                    }
                                    else if (max <= 10.0)
                                    {
                                        changeBrightness(10.0)
                                    }
                                    else
                                    {
                                        changeBrightness(100.0)
                                    }
                                }
                                // if Right Mouse button, delete color from palette
                                else if (mouse.button == Qt.RightButton)
                                {
                                    savedColors.splice(index,1)
                                    colorPalette.updatePalette()
                                }

                            }
                        }
                    }

                    Component.onCompleted: {
                        colorPalette.updatePalette()
                    }
                }
            }

            Item {
                Layout.preferredWidth: height
                Layout.preferredHeight: Math.floor((innerBorder.height - defaultSpacing.doubleBorderSize) / paletteGrid.rows)

                WGPushButton {
                    objectName: "addColorToPaletteButton"
                    anchors.centerIn: parent
                    height: parent.height - defaultSpacing.doubleBorderSize
                    width: height
                    iconSource: "icons/add_16x16.png"

                    tooltip: "Add current color to palette."

                    onClicked: {
                        if (savedColors.length < (rows * columns) - 1)
                        {
                            savedColors.push(Qt.vector4d(rgbaColor.x,rgbaColor.y,rgbaColor.z,rgbaColor.w))
                        }
                        else
                        {
                            savedColors.shift()
                            savedColors.push(Qt.vector4d(rgbaColor.x,rgbaColor.y,rgbaColor.z,rgbaColor.w))
                        }
                        colorPalette.updatePalette()
                    }
                }
            }
        }
    }
}
