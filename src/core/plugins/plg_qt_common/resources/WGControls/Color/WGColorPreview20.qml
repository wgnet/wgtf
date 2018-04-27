import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \Two overlapping color squares that show the color before and after editing. The before
 square can be clicked to reset changes.

Example:
\code{.js}
WGColorPreview {
    id: palette
    Layout.fillWidth: true
    rgbaColor: myVec4Color

    onChangeColor: {
        if (myVec4Color != col)
        {
            myVec4Color = Qt.vector4d(col.x,col.y,col.z,col.w)
        }
    }
}
\endcode
*/

WGAbstractColor {
    id: previewSquares
    objectName: "WGColorPreview"
    WGComponent { type: "WGColorPreview20" }

    altColorSpace: "RGB"

    property vector4d initialColor: Qt.vector4d(0,0,0,1)

    Item {
        anchors.centerIn: parent
        height: parent.height
        width: height

        Rectangle {
            id: secondSquareBorder
            height: Math.round(parent.height * 0.75)
            width: height
            color: palette.darkColor
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }

        Rectangle {
            height: Math.round(parent.height * 0.75)
            width: height
            color: palette.darkColor

            // Initial square. Can be clicked to reset currentColor to initialColor
            Rectangle {
                id: initialPreviewSquare
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardMargin
                width: height
                color: {
                    var tempCol = tonemap(Qt.vector3d(initialColor.x,initialColor.y,initialColor.z))
                    return Qt.rgba(tempCol.x,tempCol.y,tempCol.z,initialColor.w)
                }

                MouseArea {
                    objectName: "resetColorSquare"
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        changeColor(Qt.vector4d(initialColor.x,initialColor.y,initialColor.z,initialColor.w))
                    }
                }

                Image {
                    source: "icons/bw_check_6x6.png"
                    fillMode: Image.Tile
                    anchors.fill: parent
                    z: -1
                }
            }
        }

        // currentColor preview square
        Rectangle {
            id: currentPreviewSquare
            anchors.fill: secondSquareBorder
            anchors.margins: defaultSpacing.standardMargin
            color: {
                var tempCol
                var aVal
                if (showPreviewColor)
                {
                    tempCol = tonemap(Qt.vector3d(previewedColor.x,previewedColor.y,previewedColor.z))
                    aVal = previewedColor.w
                }
                else
                {
                    tempCol = tonemap(Qt.vector3d(currentColor.x,currentColor.y,currentColor.z))
                    aVal = currentColor.w
                }
                return Qt.rgba(tempCol.x,tempCol.y,tempCol.z,aVal)
            }

            // Dummy mouseover to mask cursor change on inital square
            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: false
            }

            Image {
                source: "icons/bw_check_6x6.png"
                fillMode: Image.Tile
                anchors.fill: parent
                z: -1
            }
        }
    }
}
