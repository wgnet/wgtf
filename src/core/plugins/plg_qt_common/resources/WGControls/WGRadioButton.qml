import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls.Styles 1.0
import WGControls.Private 1.0

/*!
 \ingroup wgcontrols
 \brief A WG styled radio button control, typically for selecting one of a set of predefined options.


Example:
\code{.js}
WGBoolGridLayout {
    ExclusiveGroup { id: radioPanelGroup }
    WGRadioButton {
        text: "Radio Button"
        exclusiveGroup: radioPanelGroup
        checked: true
    }
    WGRadioButton {
        text: "Radio Button"
        exclusiveGroup: radioPanelGroup
    }
    WGRadioButton {
        text: "Radio Button with a long name"
        exclusiveGroup: radioPanelGroup
    }
}
\endcode
*/

WGCheckBase {
    id: radioButton
    objectName: "WGRadioButton"
    WGComponent { type: "WGRadioButton" }

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    activeFocusOnTab: enabled

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    style: WGRadioStyle {

    }

    /*! Deprecated */
    property alias label_: radioButton.label
}
