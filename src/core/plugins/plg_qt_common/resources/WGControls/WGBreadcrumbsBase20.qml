import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import WGControls 2.0
import WGControls.Layouts 2.0

Row {
	id: breadcrumbsBase
    objectName: "WGBreadCrumbsBase"
    WGComponent { type: "WGBreadcrumbsBase20" }

    spacing: 0

    property var model: null
    property var modelIndex: null

    QtObject {
        id: internal

        property var parentIndex: model.parent(modelIndex)
        property var subModel: WGSubProxy {
            sourceParent: modelIndex
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

                "__onBreadcrumbSelected": function(modelIndex) { breadcrumbsBase.breadcrumbSelected(modelIndex) },
            })
        }
    }

    WGLabel {
        text: internal.isRoot ? "" : model.data(modelIndex)
        elide: Text.ElideRight
        font.bold: true
        font.pointSize: 11
        verticalAlignment: Text.AlignVCenter
        color: breadcrumbMouseArea.containsMouse ? palette.textColor : palette.neutralTextColor;

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

        menu: WGMenu {
            id: subMenu

            Instantiator {
                id: subMenuInstantiator

                model: internal.isRoot ? breadcrumbsBase.model : internal.subModel

                delegate: MenuItem {
                    text: display
                    onTriggered: {
                        var modelIndex = subMenuInstantiator.model.index(index, 0);
                        if (!internal.isRoot) {
                            modelIndex = subMenuInstantiator.model.mapToSource(modelIndex);
                        }
                        breadcrumbSelected(modelIndex);
                    }
                }

                onObjectAdded: subMenu.insertItem(index, object);
                onObjectRemoved: subMenu.removeItem(object);
            }
        }
    }
}
