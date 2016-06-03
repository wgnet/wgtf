import QtQuick 2.3

/*!
 \brief Panel intended to be placed inside a regular SubPanel.
 Consider using WGFrame for simple panels to group objects that don't need a title or to expand.

\code{.js}
WGSubPanel {
    text: "Sub Panel"
    childObject :
        WGColumnLayout {

            WGInternalPanel {
                text: "Scrolling Panel of Buttons"
                clipContents: true
                expanded: false
                childObject :
                    WGSubScrollPanel {
                        implicitHeight: 100
                        childObject:
                            WGFormLayout {
                            localForm: true
                                WGLabel{
                                    text: "Internal scrollbars possible, but not recommended"
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
            }
        }
}
\endcode
*/

WGSubPanel{
    objectName: "WGInternalPanel"
    colorHeader: palette.mainWindowColor
    colorBody: palette.mainWindowColor
}
