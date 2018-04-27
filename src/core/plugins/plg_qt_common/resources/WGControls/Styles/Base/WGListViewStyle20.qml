import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Styles 2.0

/** Default styling of WGListViews.
\ingroup wgcontrols */
WGItemViewStyle {
    WGComponent { type: "WGListViewStyle20" }
    
	rowBackground: Item {
		Rectangle {
			id: backgroundArea
			anchors.fill: parent
			color: palette.highlightShade
			opacity: isSelected ? 1 : 0.5
			visible: isHovered || isSelected
		}

		Rectangle {
			id: keyboardFocusArea
			anchors.fill: parent
			color: palette.highlightShade
			opacity: 0.25
			visible: isCurrent
		}
	}
}