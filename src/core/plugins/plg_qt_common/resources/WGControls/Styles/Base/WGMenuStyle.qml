import QtQuick 2.5
import QtQuick.Controls.Styles 1.4
import WGControls 1.0

/*! 
  \ingroup wgcontrols
  \brief Provides custom styling for WGMenu.
*/

MenuStyle {
    objectName: "WGMenuStyle"
    WGComponent { type: "WGMenuStyle" }
    
    frame: Rectangle {
        color: palette.darkHeaderColor
        WGSeparator {
            vertical: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: defaultSpacing.minimumRowHeight
            height: parent.height - defaultSpacing.doubleBorderSize
        }
    }

    itemDelegate.background: Rectangle {
        color: "transparent"
        height: defaultSpacing.minimumRowHeight
        WGHighlightFrame {
            anchors.fill: parent
            visible: styleData.selected
        }
    }

    itemDelegate.label: WGLabel {
        text: "   " + styleData.text
        color: {
            if (styleData.enabled && styleData.selected)
            {
                palette.highlightTextColor
            }
            else if (styleData.enabled && !styleData.selected)
            {
                palette.textColor
            }
            else if (!styleData.enabled)
            {
                palette.disabledTextColor
            }
        }
    }

    scrollIndicator: Text {
        text: styleData.scrollerDirection ==  Qt.DownArrow ? "\uF036" : "\uF035"
        font.family : "Marlett"
        color: palette.textColor
        renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    itemDelegate.submenuIndicator: Text {
        text: "\uF038"
        font.family : "Marlett"
        color: {
            if (styleData.enabled && styleData.selected)
            {
                palette.highlightTextColor
            }
            else if (styleData.enabled && !styleData.selected)
            {
                palette.textColor
            }
            else if (!styleData.enabled)
            {
                palette.disabledTextColor
            }
        }
        renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
        verticalAlignment: Text.AlignVCenter
        y: 3
    }

    itemDelegate.checkmarkIndicator: WGTextBoxFrame {
        id: checkboxFrame
        implicitWidth: 14
        implicitHeight: 14

        color: styleData.enabled ? palette.textBoxColor : "transparent"

        border.color: styleData.enabled ? palette.darkestShade : palette.darkerShade

        Rectangle {
            visible: styleData.checked
            color: {
                if (styleData.enabled && styleData.checked)
                {
                    palette.highlightColor
                }
                else if (styleData.enabled && !styleData.checked)
                {
                    palette.highlightShade
                }
                else if (!styleData.enabled)
                {
                    palette.lightShade
                }
            }
            radius: defaultSpacing.halfRadius
            anchors.fill: parent
            anchors.margins: styleData.checked ? 1 : 2 //smaller dull blue square if partially checked

            Text {
                id : tickMark
                color : styleData.enabled ? palette.highlightTextColor : palette.lightestShade
                font.family : "Marlett"
                font.pixelSize: checkboxFrame.height + defaultSpacing.standardRadius
                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                text : "\uF061"
                visible : styleData.checked //invisible if partially checked
                anchors.fill: parent
                anchors.leftMargin: defaultSpacing.halfRadius
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    separator: Item {
        height: defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize

        WGSeparator {
            vertical: false
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.leftMargin: defaultSpacing.minimumRowHeight + (defaultSpacing.separatorWidth / 2)
        }
    }
}
