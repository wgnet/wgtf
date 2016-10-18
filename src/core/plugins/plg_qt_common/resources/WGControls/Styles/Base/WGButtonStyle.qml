import QtQuick 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import WGControls 1.0

/*! 
    \ingroup wgcontrols
    \brief Default button style with pressed, hovered, focused and checked states.
*/

ButtonStyle {
    id: baseStyle
    objectName: "WGButtonStyle"
    WGComponent { type: "WGButtonStyle" }

    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: palette.neutralTextColor

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
        objectName: "buttonLabel"

        implicitWidth: labelFrame.implicitWidth
        implicitHeight: labelFrame.implicitHeight
        baselineOffset: labelFrame.y + labelText.y + labelText.baselineOffset

            Row {
            id: labelFrame
            anchors.centerIn: parent
            spacing: defaultSpacing.standardMargin

            //Disabled icons are desaturated and faded.
            //For some reason having the opacity change in the image didn't work with Desaturate so added parent Item

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
                    objectName: "icon"
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
                    visible: control.enabled && (control.menu != null) && control.iconSource != "" && control.showMenuIndicator
                }
            }

            Text {
                id: labelText
                text: StyleHelpers.stylizeMnemonics(control.text)

                //horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

                //icon only buttons are offcentre without this
                visible: control.text

                color: __textColor

                // Ideally a button should not have a menu and no icon, a WGDropDownBox is preferrable in that instance
                // If it does... this arrow is displayed here.
                Image{
                    id: textDropDownArrow
                    anchors.horizontalCenter: parent.right
                    source: "../icons/drop_down_arrow_16x16.png"
                    z: 1
                    visible: control.menu != null && control.iconSource == "" && control.showMenuIndicator
                }
            }
        }
    }

    background: WGButtonFrame{
        id: buttonFrame

        radius: control.radius

        states: [
            State {
                name: "PRESSED"
                when: control.pressed && control.enabled
                PropertyChanges {target: buttonFrame; color: palette.darkShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "CHECKED"
                when: control.checked && !control.pressed && !control.hovered && control.enabled && !control.activeFocus
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "HOVERED CHECKED"
                when: control.hovered && control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: "transparent"}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                PropertyChanges {target: baseStyle; __textColor: palette.disabledTextColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "DISABLED CHECKED"
                when: !control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                PropertyChanges {target: baseStyle; __textColor: palette.disabledTextColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
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
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.darkerShade}
            }

        ]

    }
}
