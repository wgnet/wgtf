import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

WGItemViewStyle {
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
