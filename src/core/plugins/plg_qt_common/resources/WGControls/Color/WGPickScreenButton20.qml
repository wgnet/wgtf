import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \A button that lets you pick a color from the screen by pressing and holding, mouseing over the desired
 color and releasing.

Example:
\code{.js}
WGPickScreenButton {
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
    id: pickControl
    objectName: "WGPickScreenButton"
    WGComponent { type: "WGPickScreenButton20" }

    altColorSpace: "RGB"

    WGPushButton {
        id: pickButton
        objectName: "pickFromScreenButton"
        anchors.fill: parent
        text: "Pick from screen"
        iconSource: "icons/dropper_16x16.png"
        checkable: true
        MouseArea {
            id: pickButtonMouseArea
            anchors.fill: parent

            onPressed: {
                pickButton.checked = true
            }

            onReleased: {
                pickButton.checked = false
                previewColor(false, Qt.vector4d(0, 0, 0, 1))

                var sampledColor = grabScreenColor(mouse.x, mouse.y, pickButtonMouseArea);
                changeColor(Qt.vector4d(sampledColor.r, sampledColor.g, sampledColor.b, 1))
            }

            onPositionChanged: {
                if ( pickButton.checked === true )
                {
                    var sampledColor = grabScreenColor(mouse.x, mouse.y, pickButtonMouseArea)
                    previewColor(true, Qt.vector4d(sampledColor.r, sampledColor.g, sampledColor.b, 1))
                }
            }
        }
    }
}
