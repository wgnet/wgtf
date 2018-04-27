import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Styles 2.0

/** Default styling of WGTreeViews.
\ingroup wgcontrols */
WGItemViewStyle {
    WGComponent { type: "WGTreeViewStyle20" }

    property Component group: Item {}

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

    rowHeader: Row {
        width: childrenRect.width
        height: Math.min(rowBackground.height, defaultSpacing.minimumRowHeight)

        property var hasChildren: itemData != null ? itemData.hasChildren : false
        property var expanded: itemData != null ? itemData.expanded : false

        Item {
            id: spacingBeforeExpandedIndicator
            width: itemDepth * 10
            height: 1
        }

        Text {
            id: expandedIndicator
            objectName: "ExpandIcon"
            color: hasChildren ? expanded ? palette.textColor : palette.neutralTextColor : "transparent"
            font.family : "Marlett"
            text : expanded ? "\uF036" : "\uF034"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            y: defaultSpacing.standardBorderSize
            anchors.verticalCenter: parent.verticalCenter
            width: contentWidth + defaultSpacing.doubleBorderSize

            MouseArea {
                objectName: "ExpandMouseArea"
                anchors.fill: parent
                enabled: hasChildren
                cursorShape: Qt.PointingHandCursor
                onPressed: {
                    view.viewExtension.toggle(itemData.modelIndex, (mouse.modifiers & Qt.ShiftModifier))
                }
            }
        }
    }
}
