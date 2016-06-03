import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGCopyableFunctions 1.0

Rectangle {
    id: root
    color: palette.mainWindowColor
    property var title: "Demo"
    property var layoutHints: { 'test': 0.1 }
    property var sourceModel: treeSource

    WGTextBox {
        id: searchBox
        anchors.left: parent.left
        anchors.right: parent.right
        placeholderText: "Search (Ctrl+p)"
		Component.onCompleted: {
            WGCopyableHelper.disableChildrenCopyable(searchBox);
        }

        WGToolButton {
            id: clearSearchBox
            iconSource: "icons/close_sml_16x16.png"
            visible: searchBox.text != ""
            tooltip: "Clear Search String"
            anchors.right: parent.right
            anchors.top: parent.top

            onClicked: {
                searchBox.text = ""
            }
        }
    }

    WGDataChangeNotifier {
        id: objectSelection
        source: CurrentIndexSource
        onDataChanged: {
            root.sourceModel = treeSource;
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
        rightMargin: 8 // leaves just enought space for conventional slider
        columnDelegates: [defaultColumnDelegate, propertyDelegate]
        selectionExtension: treeModelSelection
        treeExtension: treeModelExtension
        childRowMargin: 2
        columnSpacing: 4
        lineSeparator: false

        backgroundColourMode: incrementalGroupBackgroundColours
        backgroundColourIncrements: 5

        property Component propertyDelegate: Loader {
            clip: true
            sourceComponent: itemData != null ? itemData.component : null
        }
    }
}
