import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

/*! \brief A button style for use with WGComboBoxImage*/

WGButtonStyle {
    label:
        Item {
        id: buttonLabel
        objectName: "buttonLabel"

        implicitWidth: labelFrame.implicitWidth
        implicitHeight: labelFrame.implicitHeight
        baselineOffset: labelFrame.y + labelText.y + labelText.baselineOffset

        RowLayout {
            id: labelFrame
            height: control.height - padding.top - padding.bottom
            width: parent.width
            spacing: defaultSpacing.standardMargin

            //Disabled icons are desaturated and faded.
            //For some reason having the opacity change in the image didn't work with Desaturate so added parent Item

            // Image container
            Item {
                opacity: control.enabled ? 1 : 0.35

                width: control.iconSource ? control.height : 0
                height: control.iconSource ? parent.height : 0

                Image {
                    id: icon
                    objectName: "icon"
                    source: control.iconSource
                    visible: control.enabled
                    height: parent.height
                    width: height
                }

                Desaturate {
                    anchors.fill: icon
                    cached: true
                    source: icon
                    desaturation: control.enabled ? 0 : 1
                }
            }

            Text {
                id: labelText
                text: StyleHelpers.stylizeMnemonics(control.text)

                verticalAlignment: Text.AlignVCenter

                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

                //icon only buttons are offcentre without this
                visible: control.text

                color: __textColor
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
            }

            Text {
                id: arrowText
                color : palette.neutralTextColor
                Layout.fillHeight: true

                verticalAlignment: Text.AlignVCenter

                font.family : "Marlett"
                font.pixelSize: Math.round(parent.height / 2)

                renderType: Text.QtRendering
                text : "\uF075"
            }
        }
    }
}
