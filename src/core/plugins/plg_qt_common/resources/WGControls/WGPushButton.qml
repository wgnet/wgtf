import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import WGControls.Styles 1.0
import WGControls.Private 1.0

/*!
 \ingroup wgcontrols
 \brief Raised button with border and label.
 Button width determined by label width.
 Can have text, text+icon or just icon

Example:
\code{.js}
WGPushButton {
    text: "Open"
    iconSource: "icons/file"
}
\endcode
*/

WGAbstractButton {
    id: pushButton
    objectName: "WGPushButton"
    WGComponent { type: "WGPushButton" }

    /*! This property determines the checked state of the control
        The default value is false
    */
    property bool checked: false

    /*! This property determines the radius of the button corners
    */
    property int radius: defaultSpacing.standardRadius

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! This property holds the menu for the button
        This intentionally hides the menu property of the Button base class
    */
    property WGMenu menu: null

    /*! This property determines if the down arrow should appear if the button has a menu
    */
    property bool showMenuIndicator: true

    onButtonCheckedChanged: {
        setValueHelper( pushButton, "checked", buttonChecked);
    }

    onCheckedChanged: {
        buttonChecked = checked
    }

    // Assists in maintaining the checked property binding.
    buttonChecked: checked

    implicitHeight: defaultSpacing.minimumRowHeight

    style: WGButtonStyle{

    }

    Connections {
        target: __behavior
        onReleased: {
            if (__behavior.containsMouse && menu)
                menu.popup()
        }
    }

    /*! Deprecated */
    property alias label_: pushButton.label
}
