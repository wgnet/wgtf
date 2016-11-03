import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import WGControls 1.0

/*!
    \ingroup wgcontrols 
    \brief Drop Down box intended to be used over a 3D viewport with styleable menu
    This control is still a WIP
    The default QML ComboBox is feature lacking.
    This modification adds a styleable menu into it but still has most of its other problems.

Example:
\code{.js}
WGOverlayDropDownBox {
    id: dropDown

    model: ListModel {
        ListElement { text: "Option 1" }
        ListElement { text: "Option 2" }
        ListElement { text: "Option 3 Has a Really long Name" }
        ListElement { text: "Option 4" }
        ListElement { text: "Option 5" }
    }
}
\endcode
*/

WGDropDownBox {
    id: box
    objectName: "WGOverlayDropDownBox"
    WGComponent { type: "WGOverlayDropDownBox" }

    __textColor: palette.overlayDarkerShade

    style: ComboBoxStyle {
        id: comboBox
        background: WGButtonFrame {
            id: buttonFrame

            border.color: palette.overlayDarkerShade
            color: palette.overlayLightShade

            states: [
                State {
                    name: "PRESSED"
                    when: control.pressed && control.enabled
                    PropertyChanges {target: buttonFrame; color: palette.darkestShade}
                },
                State {
                    name: "HOVERED"
                    when: control.hovered && control.enabled
                    PropertyChanges {target: buttonFrame; color: palette.overlayLighterShade}
                    PropertyChanges {target: box; __textColor: palette.overlayTextColor}
                },
                State {
                    name: "DISABLED"
                    when: !control.enabled
                    PropertyChanges {target: buttonFrame; color: palette.lightestShade }
                    PropertyChanges {target: box; __textColor: palette.darkestShade}
                },
                State {
                    name: "ACTIVE FOCUS"
                    when: control.enabled && control.activeFocus
                    PropertyChanges {target: buttonFrame; innerBorderColor: palette.highlightShade}
                }

            ]

            Text {
                id: expandIcon
                color : box.__textColor

                anchors.fill: parent
                anchors.rightMargin: defaultSpacing.standardMargin

                font.family : "Marlett"
                font.pixelSize: parent.height / 2
                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                text : "u"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
            }
        }

        label: Text {
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            color : box.__textColor
            text: control.currentText
            renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
        }

        // drop-down customization here
        property Component __dropDownStyle: MenuStyle {
            __maxPopupHeight: 600
            __menuItemType: "comboboxitem"

            frame: Rectangle {              // background
                color: palette.mainWindowColor
                border.width: defaultSpacing.standardBorderSize
                border.color: palette.darkColor
            }

            itemDelegate.label:             // an item text
                Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: styleData.selected ? palette.textColor : palette.highlightTextColor
                text: styleData.text
            }

            itemDelegate.background: WGHighlightFrame {  // selection of an item
                visible: styleData.selected ? true : false
            }

            __scrollerStyle: ScrollViewStyle { }
        }
    }
}
