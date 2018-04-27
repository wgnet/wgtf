import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Styles 2.0

/** Default styling of WGTreeViews.
\ingroup wgcontrols */
WGTreeViewStyle {
    WGComponent { type: "WGToolboxTreeViewStyle" }
    id: baseStyle

    property bool showTabIcons: true
    property bool showTabLabels: false
    property int iconSize: 32
    property int selectedIconSize: 40
    property int labelWidth: 200

    property var customView: undefined

    property Component group: Item {
        property var polyModel: itemData != null ? itemData.definitionModel : null
        property var enableTabs: polyModel != null && itemDepth == 0

        visible: enableTabs
        height: enableTabs ? Math.max(groupHeight, tabs.height) + defaultSpacing.rowSpacing : 0
        width: enableTabs ? groupWidth : 0

        Component.onCompleted: {
            if (enableTabs)
            {
                var index = -1;
                var count = (polyModel != null) ? polyModel.count() : 0;
                for (var i = 0; i < count; ++i) {
                    var item = polyModel.item(i);
                    if (itemData.definition == item.value) {
                        index = i;
                        break;
                    }
                }
                if (index == -1 && count > 0) {
                    var item = polyModel.item(0);
                    itemData.definition = item.value;
                }

                itemData.expanded = true
            }
        }

        ListView {
            id: tabs
            model: polyModel

            interactive: false

            width: {
                var tWidth = defaultSpacing.doubleMargin
                if (baseStyle.showTabIcons)
                {
                    tWidth += baseStyle.selectedIconSize
                }
                if (baseStyle.showTabLabels)
                {
                    tWidth += baseStyle.labelWidth
                }
                if (baseStyle.showTabLabels && baseStyle.showTabIcons)
                {
                    tWidth += defaultSpacing.standardMargin
                }

                return tWidth
            }
            height: contentHeight

            delegate: Item {
                anchors.right: parent.right
                width: {
                    var item = polyModel.item(index);
                    return tabs.width - (itemData.definition == item.value ? 0 : (baseStyle.selectedIconSize - baseStyle.iconSize))
                }
                height: {
                    var item = polyModel.item(index);
                    return (itemData.definition == item.value ? baseStyle.selectedIconSize : baseStyle.iconSize) + defaultSpacing.doubleMargin
                }

                Rectangle {
                    anchors.fill: parent

                    anchors.leftMargin: defaultSpacing.doubleBorderSize
                    anchors.topMargin: defaultSpacing.doubleBorderSize
                    anchors.bottomMargin: defaultSpacing.doubleBorderSize
                    anchors.rightMargin: -defaultSpacing.standardRadius

                    radius: defaultSpacing.standardRadius

                    color: {
                        var item = polyModel.item(index);
                        if (itemData.definition == item.value)
                        {
                            return palette.darkColor
                        }
                        else if (tabMouseArea.containsMouse)
                        {
                            return palette.darkerShade
                        }
                        else
                        {
                            return palette.mainWindowColor
                        }
                    }


                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Item {
                            Layout.preferredWidth: defaultSpacing.standardMargin
                            Layout.fillHeight: true

                            Rectangle {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 2

                                anchors.verticalCenter: parent.verticalCenter

                                height: {
                                    var item = polyModel.item(index);
                                    return itemData.definition == item.value ? baseStyle.selectedIconSize : baseStyle.iconSize
                                }

                                color: palette.highlightColor
                                visible: {
                                    var item = polyModel.item(index);
                                    if (itemData.definition == item.value)
                                    {
                                        return true
                                    }
                                    else
                                    {
                                        return false
                                    }
                                }
                            }
                        }

                        Image {
                            id: tabIcon
                            property int scaledIconSize: {
                                var item = polyModel.item(index);
                                return itemData.definition == item.value ? baseStyle.selectedIconSize : baseStyle.iconSize
                            }

                            Layout.minimumHeight: scaledIconSize
                            Layout.maximumHeight: scaledIconSize

                            Layout.minimumWidth: scaledIconSize
                            Layout.maximumWidth: scaledIconSize
                            fillMode: Image.PreserveAspectCrop

                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                            property bool defaultIcon: false

                            visible: baseStyle.showTabIcons

                            source: {
                                var item = polyModel.item(index);
                                var icon = item.decoration;
                                defaultIcon = (typeof icon == "undefined")
                                if (defaultIcon)
                                {
                                    icon = "icons/tool_default_64x64.png"
                                }
                                return icon;
                            }
                            Item {
                                Layout.preferredWidth: baseStyle.showTabIcons && baseStyle.showTabLabels ? defaultSpacing.standardMargin : 0
                                Layout.fillHeight: true
                            }

                            WGLabel {
                                anchors.centerIn: parent
                                font.pixelSize: tabIcon.scaledIconSize - defaultSpacing.standardMargin
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                text: tabLabel.text.slice(0,1)
                                font.bold: true
                                font.capitalization: Font.AllUppercase
                                style: Text.Outline
                                styleColor: "black"

                                visible: tabIcon.defaultIcon && baseStyle.showTabIcons
                            }
                        }

                        WGLabel {
                            id: tabLabel
                            Layout.preferredWidth: baseStyle.showTabLabels ? contentWidth : 0
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            verticalAlignment: Text.AlignVCenter
                            visible: baseStyle.showTabLabels
                            text: {
                                var item = polyModel.item(index);
                                return item.display
                            }
                        }
                    }
                }

                WGToolTipBase {
                    text: tabLabel.text
                    tooltipArea: tabMouseArea
                }

                MouseArea {
                    id: tabMouseArea
                    anchors.fill: parent
                    anchors.leftMargin: -(baseStyle.selectedIconSize - baseStyle.iconSize)
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        var item = polyModel.item(index);
                        itemData.definition = item.value;
                    }
                }
            }
        }

        Rectangle {
            color: palette.darkColor
            anchors.left: tabs.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            radius: defaultSpacing.standardRadius
            anchors.bottomMargin: defaultSpacing.rowSpacing
        }
    }

    rowBackground: Item {
            Item {
            id: outerFrame
            anchors.fill: parent
            anchors.leftMargin: {
                var indent = defaultSpacing.doubleMargin
                if (baseStyle.showTabIcons)
                {
                    indent += baseStyle.selectedIconSize
                }
                if (baseStyle.showTabLabels)
                {
                    indent += baseStyle.labelWidth
                }
                if (baseStyle.showTabLabels && baseStyle.showTabIcons)
                {
                    indent += defaultSpacing.standardMargin
                }
                return indent
            }
            states: [
                State {
                    name: "ROW"
                    when: !isCurrent && !isSelected && !isHovered
                    PropertyChanges {target: innerFrame; visible: false}
                },
                State {
                    name: "ROW_CURRENT"
                    when: isCurrent && !isSelected && !isHovered
                    PropertyChanges {target: innerFrame; visible: true}
                    PropertyChanges {target: innerFrame; border.width: defaultSpacing.standardBorderSize}
                    PropertyChanges {target: innerFrame; border.color: palette.lightShade}
                },
                State {
                    name: "ROW_HOVERED"
                    when: isHovered && !isSelected
                    PropertyChanges {target: innerFrame; visible: true}
                    PropertyChanges {target: innerFrame; opacity: 0.5}
                    PropertyChanges {target: innerFrame; color: palette.highlightShade}
                },
                State {
                    name: "ROW_SELECTED"
                    when: isSelected
                    PropertyChanges {target: innerFrame; visible: true}
                    PropertyChanges {target: innerFrame; color: palette.highlightShade}
                    PropertyChanges {target: innerFrame; border.width: defaultSpacing.standardBorderSize}
                    PropertyChanges {target: innerFrame; border.color: palette.lightShade}
                }
            ]

            Rectangle {
                id: innerFrame
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardBorderSize
                visible: false
                color: "transparent"
                radius: defaultSpacing.halfRadius
                border.width: 0
                border.color: "transparent"
            }
        }
    }

    rowHeader: Row {
        width: childrenRect.width
        height: Math.min(rowBackground.height, defaultSpacing.minimumRowHeight)

        property var hasChildren: itemData != null ? itemData.hasChildren : false
        property var expanded: itemData != null ? itemData.expanded : false

        property var polyModel: itemData != null ? itemData.definitionModel : null
        property var enableTabs: polyModel != null

        Item {
            id: spacingBeforeExpandedIndicator
            width: {
                var indent = itemDepth * defaultSpacing.doubleMargin * 2
                if (baseStyle.showTabIcons)
                {
                    indent += baseStyle.selectedIconSize
                }
                if (baseStyle.showTabLabels)
                {
                    indent += baseStyle.labelWidth
                }
                if (baseStyle.showTabLabels && baseStyle.showTabIcons)
                {
                    indent += defaultSpacing.standardMargin
                }
                return indent
            }
            height: 1
        }

        Text {
            id: expandedIndicator
            objectName: "ExpandIcon"
            color: hasChildren ? expanded ? palette.textColor : palette.neutralTextColor : "transparent"
            font.family : "Marlett"
            text : !enableTabs && itemDepth > 0 ? (expanded ? "\uF036" : "\uF034") : ""
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: itemDepth == 0 ? 10 : 9

            anchors.verticalCenterOffset: defaultSpacing.standardBorderSize
            anchors.verticalCenter: parent.verticalCenter
            width: 16

            MouseArea {
                objectName: "ExpandMouseArea"
                anchors.fill: parent
                enabled: hasChildren
                onPressed: {
                    if (typeof customView === "undefined")
                    {
                        view.viewExtension.toggle(itemData.modelIndex, (mouse.modifiers & Qt.ShiftModifier))
                    }
                    else
                    {
                        customView.viewExtension.toggle(itemData.modelIndex, (mouse.modifiers & Qt.ShiftModifier))
                    }
                }
            }
        }
    }
}
