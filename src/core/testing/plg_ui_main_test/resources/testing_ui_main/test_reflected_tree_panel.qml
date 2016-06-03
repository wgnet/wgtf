import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGCopyableFunctions 1.0

Rectangle {
    color: palette.mainWindowColor
    property var title: "PropertyTree Test"
    property var layoutHints: { 'test': 0.1 }
    property var sourceModel: source


    Label {
        id: searchBoxLabel
        x: testTreeView.leftMargin
        y: 2
        text: "Search:"
    }

    WGTextBox {
        id: searchBox
        y: 2
        anchors.left: searchBoxLabel.right
        anchors.right: parent.right
        Component.onCompleted: {
            WGCopyableHelper.disableChildrenCopyable(searchBox);
        }
    }

    WGFilteredTreeModel {
        id: testModel
        source: sourceModel

        filter: WGTokenizedStringFilter {
            id: stringFilter
            filterText: searchBox.text
            splitterChar: " "
        }

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: treeModelExtension
            selectionExtension: treeModelSelection
        }
        ThumbnailExtension {}
        SelectionExtension {
            id: treeModelSelection
            multiSelect: true
        }
    }

    WGTreeView {
        id: testTreeView
        anchors.top: searchBox.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: testModel
        columnDelegates: [defaultColumnDelegate, propertyDelegate]
        selectionExtension: treeModelSelection
        treeExtension: treeModelExtension
        childRowMargin: 2
        columnSpacing: 4
        lineSeparator: false

        autoUpdateLabelWidths: true

        backgroundColourMode: incrementalGroupBackgroundColours
        backgroundColourIncrements: 5

        property Component propertyDelegate: Loader {
            clip: true
            sourceComponent: itemData != null ? itemData.component : null
        }
    }
}
