import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0


import WGControls 1.0
import WGColorPicker 1.0

/*!
 \A grid of color swatches displaying variations in lightness and hue for the current color
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
    lightness: myColorLightness
    alpha: myColorAlpha

    onUpdateColor: {
        myColorHue = h
        myColorSaturation = s
        myColorLightness = l
    }
}
\endcode
*/

GridLayout {
    id: shadeGrid

    /*!
        The number of rows in the grid. This is best set to an odd number in order to have a center square.

        The default is 7.
    */
    columns: 7

    /*!
        The number of rows in the grid. This is best set to an odd number in order to have a center square.

        The default is 7.
    */
    rows: 7

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
        The amount to offset the lightness by.

        The default is 0.1
    */
    property real lightOffsetAmount: 0.1

    /*!
        The hue value or chroma of the chosen color from 0 to 1.0
    */
    property real hue: 0
    /*!
        The saturation ("amount" of color) of the chosen color from 0 to 1.0
    */
    property real saturation: 0
    /*!
        The lightness (how much black or white) of the chosen color from 0 to 1.0
    */
    property real lightness: 0
    /*!
        The transparency of the chosen color from 0 (invisible) to 1.0 (opaque)
    */
    property real alpha: 0

    /*!
        The color of the currently hovered swatch.
    */
    property color hoveredColor: "#000000"

    /*!
        If a color swatch is being hovered or not.
    */
    property bool swatchHovered: false

    /*!
        The index of the center swatch.
    */
    property int centerIndex: Math.floor((columns * rows) / 2)

    /*!
        The index of the last clicked swatch
    */
    property int lastClickedIndex: -1

    /*!
        This signal is fired when one of the swatches is clicked.

        It returns the new hue, saturation and lightness values.
    */
    signal updateHSL(real h, real s, real l)

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
                var hOff = hue + ((rowIndex - (Math.floor(rows / 2))) * hueOffsetAmount)
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

            property real lightOffset: {
                var lOff = lightness + ((columnIndex - (Math.floor(columns / 2))) * lightOffsetAmount)
                if (lOff > 1.0)
                {
                    lOff = 1.0
                }
                if (lOff < 0)
                {
                    lOff = 0.0
                }
                return lOff
            }

            Rectangle {
                id: shadeBox
                anchors.fill: parent
                anchors.margins: clickArea.containsMouse ? 0 : 2

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
                color: Qt.hsla(hueOffset,saturation,lightOffset,alpha)

                Image {
                    source: "icons/bw_check_6x6.png"
                    fillMode: Image.Tile
                    anchors.fill: parent
                    z: -1
                    visible: alpha < 1.0
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
                    swatchHovered = true
                    shadeGrid.hoveredColor = shadeBox.color
                }

                onExited: {
                    swatchHovered = false
                }

                onClicked: {
                    shadeGrid.updateHSL(shadeDelegate.hueOffset,saturation,shadeDelegate.lightOffset)
                    lastClickedIndex = paletteRepeater.count - 1 - index
                }
            }
        }
    }
}
