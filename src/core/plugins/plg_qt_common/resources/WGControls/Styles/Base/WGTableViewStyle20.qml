import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Styles 2.0

/** Default styling of WGTableViews.
\ingroup wgcontrols */
WGItemViewStyle {
    WGComponent { type: "WGTableViewStyle20" }
    
    columnBackground: Rectangle {
        color: {
			if (isSelected) {
				return palette.highlightShade;
			}
			else if (isHovered) {
				return Qt.rgba(palette.highlightShade.r, palette.highlightShade.g, palette.highlightShade.b, palette.highlightShade.a * 0.5);
			}
			else {
				return "transparent";
			}
		}
		border.color: palette.lightestShade
		border.width: isCurrent ? defaultSpacing.standardBorderSize : 0
		visible: isHovered || isSelected || isCurrent
    }
}
