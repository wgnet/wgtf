import QtQuick 2.3

/*!
 \brief A scrollable sub panel to be used within controls like WGSubPanel that allow anchoring to top
 but bind to the size of their contents.
 Recommend NOT using scrollpanels within scrollpanels where possible.

Example:
\code{.js}
WGSubScrollPanel {
    implicitHeight: 100
    childObject:
        WGFormLayout {
        localForm: true
            WGLabel{
                text: "Some text"
            }

            Repeater {
                model: 10
                WGPushButton {
                    text: "Button"
                    label: "Repeated Label: "
                }
            }
        }
}
\endcode
*/

WGScrollPanel {
    objectName: "WGSubScrollPanel"
    anchors.bottom: undefined
    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    childObject_:
        Text {
            text: "Warning: WGSubScrollPanel has no childObject defined"
            color: "orange"
        }
}
