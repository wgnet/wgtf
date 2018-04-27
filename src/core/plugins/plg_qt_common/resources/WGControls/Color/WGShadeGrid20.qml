import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Global 2.0
import WGControls.Private 2.0

/*!
 \A grid of color swatches displaying variations in value and hue for the current color
 allowing the user to make small changes that are easily reversed.

 This is best paired with some other method of making larger color changes and editing the saturation.

 The grid will occupy the height and width of its parent object with some rounding on the width to make the grid
 squares and gaps even pixel ratios if equalizeGrid = true.

Example:
\code{.js}
WGShadeGrid {
    anchors.centerIn: parent
    hue: myColorHue
    saturation: myColorSaturation
    value: myColorvalue
    alpha: myColorAlpha

    onUpdateColor: {
        myColorHue = h
        myColorSaturation = s
        myColorValue = v
    }
}
\endcode
*/

WGAbstractColor {
    id: shadeGrid
    objectName: "WGShadeGrid"
    WGComponent { type: "WGShadeGrid20" }

    /*!
        The number of rows in the grid. This is best set to an odd number in order to have a center square.

        The default is 7.
    */
    property alias columns: grid.columns

    /*!
        The number of rows in the grid. This is best set to an odd number in order to have a center square.

        The default is 7.
    */
    property alias rows: grid.rows

    /*!
        Makes sure the cells and gaps are equal at the expense of some margins/rounding widthwise.

        The default is true.
    */
    property bool equalizeGrid: true

    /*!
        The amount to offset the hue by.

        The default is 0.05
    */
    property real hueOffsetAmount: 0.05

    /*!
        The amount to offset the value by.

        The default is 0.1
    */
    property real valueOffsetAmount: 0.1

    /*!
        The index of the center swatch.
    */
    property int centerIndex: Math.floor((columns * rows) / 2)

    /*!
        The index of the last clicked swatch
    */
    property int lastClickedIndex: -1

    altColorSpace: "HSL"

    height: {
        if (Component.status == Component.Ready)
        {
            if (equalizeGrid)
            {
                (parent.height) - parent.height % rows
            }
            else
            {
                parent.height
            }
        }
    }
    width: height

    GridLayout {
        id: grid
        anchors.fill: parent

        columns: 7
        rows: 7

        columnSpacing: 0
        rowSpacing: 0

        Repeater {
            id: paletteRepeater
            model: columns * rows

            Item {
                id: shadeDelegate
                Layout.fillWidth:  equalizeGrid ? false : true
                Layout.fillHeight:  equalizeGrid ? false : true
                Layout.preferredWidth: equalizeGrid ? shadeGrid.width / columns : -1
                Layout.preferredHeight:  equalizeGrid ? shadeGrid.height / rows : -1

                property int rowIndex: index / columns
                property int columnIndex: index % columns

                property bool centerSquare: index == centerIndex ? true : false

                property real hueOffset: {
                    var hOff = altColor.x + ((rowIndex - (Math.floor(rows / 2))) * hueOffsetAmount)

                    if (hOff > 1.0)
                    {
                        hOff -= 1.0
                    }
                    if (hOff < 0)
                    {
                        hOff = 1.0 + hOff
                    }
                    return hOff
                }

                property real valueOffset: {
                    var vOff = altColor.z + ((columnIndex - (Math.floor(columns / 2))) * valueOffsetAmount)

                    if (vOff > 1.0)
                    {
                        vOff = 1.0
                    }
                    if (vOff < 0)
                    {
                        vOff = 0.0
                    }
                    return vOff
                }

                Rectangle {
                    id: shadeBox
                    anchors.fill: parent
                    anchors.margins: clickArea.containsMouse ? 0 : 2

                    property vector3d cellColor: Qt.vector3d(shadeDelegate.hueOffset, altColor.y, shadeDelegate.valueOffset)

                    //give the center square a white border (WILL NOT WORK WITH EVEN ROWS AND COLUMNS)
                    border.width: 1
                    border.color: {
                        if (shadeDelegate.centerSquare)
                        {
                            "white"
                        }
                        else if (index == lastClickedIndex)
                        {
                            "#AAAAAA"
                        }
                        else
                        {
                            "transparent"
                        }
                    }
                    color: {
                        var rgbCol = WGColor.hslToRgb(cellColor.x,cellColor.y,cellColor.z)
                        rgbCol = tonemap(rgbCol)
                        return Qt.rgba(rgbCol.x, rgbCol.y, rgbCol.z, rgbaColor.w)
                    }

                    Image {
                        source: "icons/bw_check_6x6.png"
                        fillMode: Image.Tile
                        anchors.fill: parent
                        z: -1
                        visible: rgbaColor.w < 1.0
                    }
                }

                MouseArea {
                    id: clickArea
                    objectName: "shadeSquareButton"
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    enabled: !shadeDelegate.centerSquare

                    onEntered: {
                        var hCol = WGColor.hslToRgb(shadeBox.cellColor.x, shadeBox.cellColor.y, shadeBox.cellColor.z)
                        hCol = tonemap(hCol)
                        previewColor(true, Qt.vector4d(hCol.x,hCol.y,hCol.z,rgbaColor.w))
                    }

                    onExited: {
                        previewColor(false, Qt.vector4d(0,0,0,1))
                    }

                    onClicked: {
                        changeAltColor(Qt.vector3d(shadeDelegate.hueOffset, altColor.y, shadeDelegate.valueOffset))
                        lastClickedIndex = paletteRepeater.count - 1 - index
                    }
                }
            }
        }
    }
}
