import QtQuick 2.5
import QtQuick.Controls.Styles 1.4
import WGControls 1.0

/*! 
    \ingroup wgcontrols
    \brief Provides custom styling for WGTextBox.
*/

TextFieldStyle {
    id: baseStyle
    objectName: "WGTexBoxStyle"
    WGComponent { type: "WGTextBoxStyle" }

    textColor: palette.textColor

    /*! An undocumented (Qt) Style property required to centre the edit text properly */
    padding { top: 2 ; left: 4 ; right: 2 ; bottom: 2 }

    selectedTextColor: palette.highlightTextColor

    selectionColor: palette.highlightColor

    placeholderTextColor: palette.placeholderTextColor

    renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

    background: WGTextBoxFrame {
        id: textFrame
        color: palette.textBoxColor
        border.color: palette.darkestShade

        states: [
            State {
                name: "ACTIVE FOCUS"
                when: control.activeFocus && control.enabled && !control.readOnly
                PropertyChanges {target: textFrame; border.color: palette.lighterShade}
            },
            State {
                name: "ACTIVE FOCUS READ ONLY"
                when: control.activeFocus && control.enabled && control.readOnly
                PropertyChanges {target: textFrame; color: "transparent"}
                PropertyChanges {target: textFrame; border.color: palette.lighterShade}
                PropertyChanges {target: baseStyle; textColor: palette.neutralTextColor}
            },
            State {
                name: "READ ONLY"
                when: control.enabled && control.readOnly && !control.activeFocus
                PropertyChanges {target: textFrame; color: "transparent"}
                PropertyChanges {target: textFrame; border.color: palette.darkestShade}
                PropertyChanges {target: baseStyle; textColor: palette.neutralTextColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled
                PropertyChanges {target: textFrame; border.color: palette.darkestShade}
                PropertyChanges {target: baseStyle; textColor: palette.disabledTextColor}
            }
        ]
    }
}
