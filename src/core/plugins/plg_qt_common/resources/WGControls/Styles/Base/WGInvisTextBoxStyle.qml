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
    WGComponent { type: "WGInvisTextBoxStyle" }

    textColor: palette.textColor

    /*! An undocumented (Qt) Style property required to centre the edit text properly */
    padding { top: 2 ; left: 4 ; right: 4 ; bottom: 2 }

    selectedTextColor: palette.highlightTextColor

    selectionColor: palette.highlightColor

    placeholderTextColor: palette.placeholderTextColor

    background: WGTextBoxFrame {
        id: textFrame
        color: "transparent"
        border.color: "transparent"

        states: [
            State {
                name: "READ ONLY"
                when: control.enabled && control.readOnly && !control.activeFocus
                PropertyChanges {target: baseStyle; textColor: palette.readonlyTextColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled
                PropertyChanges {target: baseStyle; textColor: palette.disabledTextColor}
            }
        ]
    }
}
