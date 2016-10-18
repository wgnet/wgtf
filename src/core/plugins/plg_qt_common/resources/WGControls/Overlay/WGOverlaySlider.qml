import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls 1.0
import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
 \brief Slider with value spinbox.
 Purpose: Provide the user with a single value clamped between min and max value

Example:
\code{.js}
WGOverlaySlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    stepSize: 1
    value: 40
}
\endcode

 \todo Test orientation = vertical. Create vertical slider. Remove option here
*/

WGSliderControl {
    id: sliderFrame
    objectName: "WGSliderControl"
    WGComponent { type: "WGOverlaySlider" }

    style: WGOverlaySliderStyle{}

    textBoxStyle: WGOverlayTextBoxStyle {}

    buttonFrame: WGButtonFrame{
        objectName: "button"
        id: button
        radius: 0
        property bool hovered: parent.hovered
        property bool up: parent.up
        property bool pressed: parent.pressed

        color: palette.overlayLightShade
        border.color: palette.overlayDarkerShade

        Text {
            id: arrowText
            color : palette.overlayDarkerShade

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            font.family : "Marlett"
            font.pixelSize: 2 * Math.round(parent.height/2)

            renderType: Text.QtRendering
            text : button.up ? "\uF074" : "\uF075"
        }

        states: [
            State {
                name: "PRESSED"
                when: button.pressed && sliderFrame.enabled
                PropertyChanges {target: button; color: palette.darkestShade}
            },
            State {
                name: "HOVERED"
                when: button.hovered && sliderFrame.enabled
                PropertyChanges {target: button; color: palette.overlayLighterShade}
                PropertyChanges {target: arrowText; color: palette.overlayTextColor}
            },
            State {
                name: "DISABLED"
                when: !sliderFrame.enabled
                PropertyChanges {target: button; color: palette.lightestShade}
                PropertyChanges {target: arrowText; color: palette.darkestShade}
            }
        ]

    }
}
