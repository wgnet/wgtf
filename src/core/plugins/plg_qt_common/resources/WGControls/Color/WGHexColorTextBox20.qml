import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \An editable text box that accepts hex values for RGBA colors in the Qt format #AARRGGBB

Example:
\code{.js}
WGHexColorTextBox {
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
    id: hexBox
    objectName: "WGHexColorTextBox"
    WGComponent { type: "WGHexColorTextBox20" }

    altColorSpace: "RGB"

    implicitHeight: defaultSpacing.minimumRowHeight

    property color validatedColor

    onRgbaColorChanged: {
        var tempCol = tonemap(Qt.vector3d(rgbaColor.x,rgbaColor.y,rgbaColor.z))
        hexValue.text = Qt.rgba(tempCol.x,tempCol.y,tempCol.z,rgbaColor.w)
    }

    WGTextBox {
        id: hexValue
        anchors.fill: parent

        validator: RegExpValidator {
            regExp: /^#([A-Fa-f0-9]{8}|[A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/
        }

        onEditingFinished: {
            validatedColor = text
            changeColor(Qt.vector4d(validatedColor.r, validatedColor.g, validatedColor.b, validatedColor.a))
        }
    }
}
