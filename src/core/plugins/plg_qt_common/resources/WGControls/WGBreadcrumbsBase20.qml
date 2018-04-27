import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import Qt.labs.controls 1.0 as Labs

import WGControls 2.0
import WGControls.Layouts 2.0
import WGControls.Views 2.0

Row {
    id: breadcrumbsBase
    objectName: "WGBreadCrumbsBase"
    WGComponent { type: "WGBreadcrumbsBase20" }

    spacing: 0

    readonly property var defaultFontSize: 11
    property var fontSize: defaultFontSize
    property var model: null
    property var modelIndex: null

    QtObject {
        id: internal

        property var parentIndex: model.parent(modelIndex)

        property var subModel: WGSubProxy {
            // this check is technically briefly wrong, but it prevents an error in output because the modelIndex isn't ready.
            sourceParent: modelIndex != null ? modelIndex : model.parent(modelIndex)
        }

        property var isRoot: parentIndex == modelIndex
    }

    property var __onBreadcrumbSelected: function(modelIndex) {}

    signal breadcrumbSelected(var modelIndex)

    onBreadcrumbSelected: __onBreadcrumbSelected(modelIndex)

    Loader {
        active: !internal.isRoot
        visible: active

        Component.onCompleted: {
            setSource("WGBreadcrumbsBase20.qml", {
                "model": Qt.binding( function() { return breadcrumbsBase.model } ),
                "modelIndex": Qt.binding( function() { return internal.parentIndex } ),
                "fontSize": Qt.binding( function() { return breadcrumbsBase.fontSize } ),

                "__onBreadcrumbSelected": function(modelIndex) { breadcrumbsBase.breadcrumbSelected(modelIndex) },
            })
        }
    }

    WGLabel {
        text: modelIndex == null ? "" : (internal.isRoot ? "" : model.data(modelIndex))
        elide: Text.ElideRight
        font.bold: true
        font.pointSize: fontSize
        verticalAlignment: Text.AlignVCenter
        color: breadcrumbMouseArea.containsMouse ? palette.textColor : palette.neutralTextColor;
        height: parent.height

        MouseArea {
            id: breadcrumbMouseArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onPressed: {
                breadcrumbSelected(breadcrumbsBase.modelIndex);
            }
        }
    }

    WGToolButton {
        width: 16
        showMenuIndicator: false
        visible: model.hasChildren(modelIndex)
        iconSource: "icons/arrow_right_small_16x16.png"

        WGPopup {
            id: menuPopup
            openPolicy: openOnClick
            closePolicy: Labs.Popup.OnPressOutside | Labs.Popup.onEscape
            padding: 0

            readonly property var maximumHeight: 150
            width: menuListView.listWidth + (defaultSpacing.doubleMargin * 4)
            height: Math.min(menuListView.listHeight + (defaultSpacing.doubleMargin * 2), maximumHeight)

            exitMouseAreaOnTop: false

            Rectangle {
                id: menuRect
                anchors.fill: parent
                color: palette.mainWindowColor
                border.width: defaultSpacing.doubleBorderSize
                border.color: palette.darkColor

                WGScrollView {
                    id: menuScrollView
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.doubleBorderSize * 2

                    WGListView {
                        id: menuListView
                        columnDelegate: contentDelegate
                        model: menuPopup.visible ? (internal.isRoot ? breadcrumbsBase.model : internal.subModel) : null
                        clamp: true

                        property var listHeight: menuListView.contentHeight
                        property var listWidth: 0

                        onHoveredIndexChanged: {
                            if (hoveredIndex != null)
                            {
                                menuPopup.popupHovered = true
                            }
                        }

                        onItemClicked: {
                            var modelIndex = model.index(rowIndex.row, 0);
                            if (!internal.isRoot) {
                                modelIndex = model.mapToSource(modelIndex);
                            }
							menuPopup.close();
                            breadcrumbSelected(modelIndex);
                        }

                        onModelChanged: {
                            currentIndex = -1
                        }

                        property Component contentDelegate: Text {
                            id: textItem
                            anchors.left: parent ? parent.left : undefined
                            anchors.right: parent ? parent.right : undefined
                            color: palette.textColor

                            Component.onCompleted: {
                                menuListView.listWidth = Math.max(menuListView.listWidth, textWidth.width);
                            }

                            TextMetrics {
                                id: textWidth
                                text: textItem.text
                            }

                            text: {
                                if(itemData == null || typeof(itemData) == "undefined") {
                                    return "";
                                }
                                var display = itemData.display;
                                if(typeof(display) != "undefined") {
                                    return display;
                                }
                                var value = itemData.value;
                                if(typeof(value) != "undefined") {
                                    return value;
                                }
                                return "";
                            }
                        }
                    }
                }
            }
        }
    }
}
