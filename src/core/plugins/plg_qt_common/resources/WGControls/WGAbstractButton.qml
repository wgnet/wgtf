import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/*!
 \brief An abstract button class to fix checked state bindings being dropped.
*/

Button {
    objectName: "WGAbstractButton"

    property bool buttonChecked: checked

    onButtonCheckedChanged: {
        checked = buttonChecked
    }

    onCheckedChanged: {
        buttonChecked = checked
    }
}
