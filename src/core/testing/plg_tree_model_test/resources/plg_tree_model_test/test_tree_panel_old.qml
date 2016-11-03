import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGCopyableFunctions 1.0
import WGControls.Views 1.0

WGPanel {
    color: palette.mainWindowColor
    title: "TreeModel Test"
    layoutHints: { 'test': 0.1 }
    property var sourceModel: source
    property var useModel: 1
    property var topControlsHeight: 20

    Button {
        id: switchModelButton
        anchors.top: parent.top
        anchors.left: parent.left
        width: 150
        height: topControlsHeight
        text: useModel ? "Switch Model Off" : "Switch Model On"

        onClicked: {
            useModel = useModel == 0 ? 1 : 0;
        }
    }

    Label {
        id: searchBoxLabel
        y: 2
        anchors.left: switchModelButton.right
        text: "Search:"
    }

    WGTextBox {
        id: searchBox
        anchors.top: parent.top
        anchors.left: searchBoxLabel.right
        anchors.right: parent.right
        height: topControlsHeight
        Component.onCompleted: {
            WGCopyableHelper.disableChildrenCopyable(searchBox);
        }
    }

    WGFilteredTreeModel {
        id: testModel
        source: useModel ? sourceModel : null

        filter: WGTokenizedStringFilter {
            id: stringFilter
            filterText: searchBox.text
            splitterChar: " "
        }

        HeaderFooterTextExtension {}
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
        selectionExtension: treeModelSelection
        treeExtension: treeModelExtension
        childRowMargin: 2
        lineSeparator: false
        showColumnsFrame: true
        showColumnHeaders: true
        showColumnFooters: true
        backgroundColourMode: incrementalGroupBackgroundColours
    }
}
