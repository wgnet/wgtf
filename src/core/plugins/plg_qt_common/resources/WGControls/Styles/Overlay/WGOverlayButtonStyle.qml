import QtQuick 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import WGControls 1.0

/*! 
    \ingroup wgcontrols
    \brief Button style intended to be used over a 3D vieport with pressed, hovered, focused and checked states.
*/

ButtonStyle {
    id: baseStyle
    objectName: "WGOverlayButtonStyle"
    WGComponent { type: "WGOverlayButtonStyle" }

    /*! \internal */
    // Variable for the amount of offset the button will move when pressed. Is zero in neutral state.
    property int __pushOffset: 0

    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: palette.overlayDarkerShade

    /*! \internal */
    // Ideally a button should not have a menu and no icon, a WGDropDownBox is preferrable in that instance
    // If it does this extra padding fixes it.
    property int __menuPadding:{
        if(control.menu !== null && control.iconSource == "")
        {
            5
        }
        else
        {
            0
        }
    }

    padding {
        top: defaultSpacing.standardRadius
        left: defaultSpacing.standardRadius + (control.iconSource != "" ? 0 : defaultSpacing.standardRadius)
        right: defaultSpacing.standardRadius + (control.text != "" ? defaultSpacing.standardRadius : 0) + __menuPadding
        bottom: defaultSpacing.standardRadius
    }

    label:
        Item {
        id: buttonLabel

        implicitWidth: labelFrame.implicitWidth
        implicitHeight: labelFrame.implicitHeight
        baselineOffset: labelFrame.y + labelText.y + labelText.baselineOffset

            Row {
            id: labelFrame
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: defaultSpacing.standardMargin

            //Disabled icons are desaturated and faded.
            //For some reason having the opacity change in the image didn't work with Desaturate so added parent Item

            anchors.verticalCenterOffset: 0 + __pushOffset

            Item {
                anchors.verticalCenter: parent.verticalCenter
                opacity: control.enabled ? 1 : 0.35

                width: {
                    if (control.iconSource)
                    {
                        icon.width
                    } else
                    {
                        0
                    }
                }

                height: {
                    if (control.iconSource)
                    {
                        icon.height
                    } else
                    {
                        0
                    }
                }

                Image{
                    id: icon
                    anchors.verticalCenter: parent.verticalCenter
                    source: control.iconSource
                    visible: control.enabled
                }

                Desaturate {
                    anchors.fill: icon
                    cached: true
                    source: icon
                    desaturation: control.enabled ? 0 : 1
                }

                Image{
                    id: dropDownArrow
                    anchors.verticalCenter: parent.verticalCenter
                    source: "../icons/drop_down_arrow_16x16.png"
                    z: 1
                    visible: control.enabled && (control.menu != null) && control.iconSource != ""
                }
            }

            Text {
                id: labelText
                text: StyleHelpers.stylizeMnemonics(control.text)

                //horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                color: __textColor
                font.bold: true

                //icon only buttons are offcentre without this
                visible: control.text

                // Ideally a button should not have a menu and no icon, a WGDropDownBox is preferrable in that instance
                // If it does... this arrow is displayed here.
                Image{
                    id: textDropDownArrow
                    anchors.horizontalCenter: parent.right
                    source: "../icons/drop_down_arrow_16x16.png"
                    z: 1
                    visible: control.menu != null && control.iconSource == ""
                }
            }
        }
    }

    background: WGButtonFrame{
        id: buttonFrame

        radius: control.radius
        color: palette.overlayLightShade
        border.color: palette.overlayDarkerShade

        states: [
            State {
                name: "PRESSED"
                when: control.pressed && control.enabled && !control.checked
                PropertyChanges {target: baseStyle; __pushOffset: 1}
                PropertyChanges {target: buttonFrame; color: palette.darkestShade}
            },

            State {
                name: "PRESSED CHECKED"
                when: control.pressed && control.enabled && control.checked
                PropertyChanges {target: baseStyle; __pushOffset: 1}
                PropertyChanges {target: buttonFrame; color: palette.highlightShade}
            },

            State {
                name: "CHECKED"
                when: control.checked && !control.pressed && !control.hovered && control.enabled && !control.activeFocus
                PropertyChanges {target: buttonFrame; color: palette.highlightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.highlightTextColor}
            },
            State {
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: palette.overlayLighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.overlayTextColor}
            },
            State {
                name: "HOVERED CHECKED"
                when: control.hovered && control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.highlightTextColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: palette.lightestShade }
                PropertyChanges {target: baseStyle; __textColor: palette.darkestShade}
            },
            State {
                name: "DISABLED CHECKED"
                when: !control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.darkestShade }
                PropertyChanges {target: baseStyle; __textColor: palette.lightestShade}
            },
            //TODO There is nothing visual that illustrates an active focus button.
            State {
                name: "ACTIVE FOCUS"
                when: control.enabled && control.activeFocus && !control.checked
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.highlightShade}
            },
            State {
                name: "ACTIVE FOCUS CHECKED"
                when: control.enabled && control.activeFocus && control.checked
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.highlightColor}
                PropertyChanges {target: buttonFrame; color: palette.highlightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.highlightTextColor}
            }

        ]
    }

    /*! Deprecated */
    property alias pushOffset: baseStyle.__pushOffset
}
