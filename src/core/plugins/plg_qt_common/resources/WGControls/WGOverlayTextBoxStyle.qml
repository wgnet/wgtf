import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*! \brief Provides custom styling for WGTextBox.*/

TextFieldStyle {
    id: baseStyle
    objectName: "WGTexBoxStyle"

    textColor: palette.overlayTextColor

    /*! An undocumented (Qt) Style property required to centre the edit text properly */
    padding { top: 2 ; left: 4 ; right: 4 ; bottom: 2 }

    selectedTextColor: palette.highlightTextColor

    selectionColor: palette.highlightShade

    placeholderTextColor: palette.overlayDarkShade

    background: WGTextBoxFrame {
        id: textFrame
        color: palette.overlayLightShade
        border.color: palette.overlayDarkerShade

        states: [
            State {
                name: "ACTIVE FOCUS"
                when: control.activeFocus && control.enabled && !control.readOnly
                PropertyChanges {target: textFrame; border.color: palette.highlightShade}
            },
            State {
                name: "ACTIVE FOCUS READ ONLY"
                when: control.activeFocus && control.enabled && control.readOnly
                PropertyChanges {target: textFrame; color: palette.lightestShade}
                PropertyChanges {target: textFrame; border.color: palette.highlightShade}
                PropertyChanges {target: baseStyle; textColor: palette.overlayDarkerShade}
            },
            State {
                name: "READ ONLY"
                when: control.enabled && control.readOnly && !control.activeFocus
                PropertyChanges {target: textFrame; color: palette.lightestShade}
                PropertyChanges {target: textFrame; border.color: palette.overlayDarkShade}
                PropertyChanges {target: baseStyle; textColor: palette.overlayDarkerShade}
            },
            State {
                name: "DISABLED"
                when: !control.enabled
                PropertyChanges {target: textFrame; color: palette.lightShade}
                PropertyChanges {target: textFrame; border.color: palette.darkestShade}
                PropertyChanges {target: baseStyle; textColor: palette.darkestShade}
            }
        ]
    }
}
