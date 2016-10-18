import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Styles 2.0

/** Default styling of WGTableViews.
\ingroup wgcontrols */
WGItemViewStyle {
    WGComponent { type: "WGTableViewStyle20" }
    
    columnBackground: Item {
        Rectangle {
            id: backgroundArea
            anchors.fill: parent
            color: palette.highlightShade
            opacity: isSelected ? 1 : 0.5
            visible: hoverArea.containsMouse || isSelected
        }

        MouseArea {
            id: hoverArea
            anchors.fill: backgroundArea
            hoverEnabled: true
        }

        Rectangle {
            id: keyboardFocusArea
            anchors.fill: parent
            color: "transparent"
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.lightestShade
            radius: defaultSpacing.halfRadius
            visible: isCurrent
        }
    }
}
