import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Styles 2.0

/** Default styling of WGTreeViews.
\ingroup wgcontrols */
WGTreeViewStyle {
    WGComponent { type: "WGPropertyTreeViewStyle20" }

    rowBackground: Rectangle {
        id: outerFrame
        radius: 0

        states: [
            State {
                name: "TOPPARENT"
                when: itemDepth == 0 && hasChildren && !isCurrent && !isSelected && !isHovered
                PropertyChanges {target: outerFrame; border.width: 1}
                PropertyChanges {target: outerFrame; border.color: palette.darkerShade}
                PropertyChanges {target: outerFrame; color: palette.lightShade}
                PropertyChanges {target: innerFrame; visible: true}
                PropertyChanges {target: innerFrame; border.width: defaultSpacing.standardBorderSize}
                PropertyChanges {target: innerFrame; border.color: palette.lightShade}
            },
            State {
                name: "TOPPARENT_CURRENT"
                when: itemDepth == 0 && hasChildren && isCurrent && !isSelected && !isHovered
                PropertyChanges {target: outerFrame; border.width: 1}
                PropertyChanges {target: outerFrame; border.color: palette.darkerShade}
                PropertyChanges {target: outerFrame; color: palette.lightShade}
                PropertyChanges {target: innerFrame; visible: true}
                PropertyChanges {target: innerFrame; border.width: defaultSpacing.standardBorderSize}
                PropertyChanges {target: innerFrame; border.color: palette.highlightShade}
            },
            State {
                name: "TOPPARENT_HOVERED"
                when: itemDepth == 0 && hasChildren && isHovered && !isSelected
                PropertyChanges {target: outerFrame; border.width: 1}
                PropertyChanges {target: outerFrame; border.color: palette.darkerShade}
                PropertyChanges {target: outerFrame; color: palette.lightShade}
                PropertyChanges {target: innerFrame; visible: true}
                PropertyChanges {target: innerFrame; opacity: 0.5}
                PropertyChanges {target: innerFrame; color: palette.highlightShade}
                PropertyChanges {target: innerFrame; border.width: defaultSpacing.standardBorderSize}
                PropertyChanges {target: innerFrame; border.color: palette.lightShade}
            },
            State {
                name: "TOPPARENT_SELECTED"
                when: itemDepth == 0 && hasChildren && isSelected
                PropertyChanges {target: outerFrame; border.width: 1}
                PropertyChanges {target: outerFrame; border.color: palette.darkerShade}
                PropertyChanges {target: outerFrame; color: palette.lightShade}
                PropertyChanges {target: innerFrame; visible: true}
                PropertyChanges {target: innerFrame; color: palette.highlightShade}
                PropertyChanges {target: innerFrame; border.width: defaultSpacing.standardBorderSize}
                PropertyChanges {target: innerFrame; border.color: palette.lightShade}
            },
            State {
                name: "CHILD_CURRENT"
                when: (itemDepth > 0 || !hasChildren) && isCurrent && !isSelected && !isHovered
                PropertyChanges {target: innerFrame; visible: true}
                PropertyChanges {target: innerFrame; border.width: defaultSpacing.standardBorderSize}
                PropertyChanges {target: innerFrame; border.color: palette.lightShade}
            },
            State {
                name: "CHILD_HOVERED"
                when: (itemDepth > 0 || !hasChildren) && isHovered && !isSelected
                PropertyChanges {target: innerFrame; visible: true}
                PropertyChanges {target: innerFrame; opacity: 0.5}
                PropertyChanges {target: innerFrame; color: palette.highlightShade}
            },
            State {
                name: "CHILD_SELECTED"
                when: (itemDepth > 0 || !hasChildren) && isSelected
                PropertyChanges {target: innerFrame; visible: true}
                PropertyChanges {target: innerFrame; color: palette.highlightShade}
            }
        ]


        border.width: 0
        border.color: "transparent"

        color: Qt.rgba(palette.darkShade.r,palette.darkShade.g,palette.darkShade.b, palette.darkShade.a + Math.min(itemDepth * 0.1, 0.5))

        Rectangle {
            id: innerFrame
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardBorderSize
            visible: false
            color: "transparent"

            border.width: 0
            border.color: "transparent"
        }
    }

    rowHeader: Row {
        width: childrenRect.width
        height: Math.min(rowBackground.height, defaultSpacing.minimumRowHeight)

        property var hasChildren: itemData != null ? itemData.hasChildren : false
        property var expanded: itemData != null ? itemData.expanded : false

        Item {
            id: spacingBeforeExpandedIndicator
            width: Math.max(itemDepth * defaultSpacing.doubleMargin, defaultSpacing.rowSpacing)
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
            font.pointSize: itemDepth == 0 ? 10 : 9

            anchors.verticalCenterOffset: defaultSpacing.standardBorderSize
            anchors.verticalCenter: parent.verticalCenter
            width: contentWidth + defaultSpacing.doubleBorderSize

            MouseArea {
                objectName: "ExpandMouseArea"
                anchors.fill: parent
                enabled: hasChildren
                onPressed: {
                    view.viewExtension.toggle(itemData.modelIndex, (mouse.modifiers & Qt.ShiftModifier))
                }
            }
        }
    }
}
