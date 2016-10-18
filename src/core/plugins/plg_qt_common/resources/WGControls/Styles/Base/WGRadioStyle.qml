import QtQuick 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import WGControls 1.0
import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
 \brief Provides custom styling for WGRadioButton
*/

WGCheckBoxStyle {
    id: radioStyle
    objectName: "WGRadioStyle"
    WGComponent { type: "WGRadioStyle" }

    /*! \internal */
    // helper property for text color so states can all be in the indicator object
    property color __textColor: palette.neutralTextColor

    label: Text {
        text: control.text
        color: __textColor
        renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

        //coloured border around the label when the control has focus
        Rectangle {
            height: parent.height + defaultSpacing.doubleBorderSize
            width: parent.width + defaultSpacing.standardMargin
            anchors.centerIn: parent
            visible: control.activeFocus && control.text != ""
            color: "transparent"
            radius: defaultSpacing.halfRadius
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.lighterShade
        }
    }

    indicator: WGTextBoxFrame {
        id: radioFrame
        implicitWidth: Math.round(TextSingleton.implicitHeight) - defaultSpacing.rowSpacing
        implicitHeight: width
        radius: height

        color: palette.textBoxColor

        border.color: control.activeFocus && control.text == "" ? palette.lighterShade : palette.darkestShade

        states: [
            State {
                name: "UNCHECKED DISABLED"
                when: !control.checked && !control.enabled
                PropertyChanges {target: radioStyle; __textColor: palette.disabledTextColor}
                PropertyChanges {target: radioFrame; color: "transparent"}
                PropertyChanges {target: radioFrame; border.color: palette.darkerShade}
            },
            State {
                name: "CHECKED"
                when: control.checked && control.enabled
                PropertyChanges {target: dotContainer; visible: true}
            },
            State {
                name: "CHECKED DISABLED"
                when: control.checked && !control.enabled
                PropertyChanges {target: radioStyle; __textColor: palette.disabledTextColor}
                PropertyChanges {target: radioFrame; color: "transparent"}
                PropertyChanges {target: radioFrame; border.color: palette.darkerShade}
                PropertyChanges {target: dotContainer; color: palette.lightShade}
                PropertyChanges {target: dotContainer; visible: true}
            }
        ]

        Rectangle {
            id: dotContainer
            visible: false
            color: palette.highlightColor
            radius: height
            anchors.fill: parent
            anchors.margins:2
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.darkerShade
        }
    }
}
