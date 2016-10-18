import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

/*!
 \ingroup wgcontrols
 \brief An abstract button class to fix checked state bindings being dropped.
*/

Button {
    objectName: "WGAbstractButton"

    property bool buttonChecked: checked

    /*! property determines if the control represents multiple values */
    property bool multipleValues: false

    onButtonCheckedChanged: {
        checked = buttonChecked
    }

    onCheckedChanged: {
        buttonChecked = checked
    }
}
