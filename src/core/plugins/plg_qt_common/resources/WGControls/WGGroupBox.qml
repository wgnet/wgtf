import QtQuick 2.5

/*!
    \ingroup wgcontrols
    \brief A style varition of WGFrame with thin separators top and bottom.
    Good for toggleable groups that enable/disable their contents.
    Not intended to be used as a collapsible group.

Example:
\code{.js}
    WGFrame {
        text: "Hello World"
        toggleable: true
        childObject :
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
    }
\endcode
*/

WGFrame{
    objectName: "WGGroupBox"
    WGComponent { type: "WGGroupBox" }
    
    lineFrame: true
    toggleable: true
}
