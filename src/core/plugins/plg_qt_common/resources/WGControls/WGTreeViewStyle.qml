import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

WGItemViewStyle {
	rowBackground: Item {
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
			color: palette.highlightShade
			opacity: 0.25
			visible: isCurrent
		}
	}

	rowHeader: Row {
        width: childrenRect.width
        height: childrenRect.height

        // Reserve space.
        Item {
            width: itemDepth * 10
            height: 1
        }

        // Expanded/collapsed arrow for tree views.
        Text {
			objectName: "ExpandIcon"
            color: itemData.hasChildren ? itemData.expanded ? palette.textColor : palette.neutralTextColor : "transparent"
            font.family : "Marlett"
            text : itemData.expanded ? "\uF036" : "\uF034"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            MouseArea {
				objectName: "ExpandMouseArea"
                anchors.fill: parent
                enabled: itemData.hasChildren
                onPressed: {
                    itemData.expanded = !itemData.expanded;
                }
            }
        }
	}
}