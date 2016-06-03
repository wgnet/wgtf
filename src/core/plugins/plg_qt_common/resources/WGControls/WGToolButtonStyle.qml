import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

/*! \brief Provides custom styling for WGToolButton.
    Button border is only visible when pressed or hovered.
    Generally intended NOT to have text but can be used in cases where a frame is not desired*/

WGButtonStyle {
    id: baseStyle
    objectName: "WGToolButtonStyle"

    background: WGButtonFrame{
        id: buttonFrame

        radius: control.radius

        innerBorderColor: "transparent"
        borderColor: "transparent"
        color: "transparent"

        states: [
            State {
                name: "PRESSED"
                when: control.pressed && control.enabled
                PropertyChanges {target: buttonFrame; color: palette.darkShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkerShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "CHECKED"
                when: control.checked && !control.pressed && !control.hovered && control.enabled && !control.activeFocus
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkerShade}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkerShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.lightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "HOVERED CHECKED"
                when: control.hovered && control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkerShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.lightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: "transparent"}
                PropertyChanges {target: buttonFrame; borderColor: "transparent"}
                PropertyChanges {target: baseStyle; __textColor: palette.disabledTextColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "DISABLED CHECKED"
                when: !control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                PropertyChanges {target: baseStyle; __textColor: palette.disabledTextColor}
            },
            State {
                name: "ACTIVE FOCUS"
                when: control.enabled && control.activeFocus && !control.checked
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.lightestShade}
            },
            State {
                name: "ACTIVE FOCUS CHECKED"
                when: control.enabled && control.activeFocus && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkerShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.darkerShade}
            }
        ]
    }
}
