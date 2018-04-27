import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
    id: root
    color: palette.mainWindowColor
    title: "Demo"
    layoutHints: { 'test': 0.1 }
    property var sourceModel: getTreeModel

    WGTextBox {
        id: searchBox
        anchors.left: parent.left
        anchors.right: parent.right
        placeholderText: "Search (Ctrl + P)"
        onTextChanged: setFilter(text)

        WGToolButton {
            id: clearSearchBox
            iconSource: "icons/close_sml_16x16.png"
            visible: searchBox.text != ""
            tooltip: "Clear Search String"
            anchors.right: parent.right
            anchors.top: parent.top

            onClicked: clearFilter();
        }
    }

    function clearFilter() {
        searchBox.text = "";
    }

    function setFilter(text)
    {
        var filterText = "(" + text.replace(/ /g, "|") + ")";
        filterObject.filter = new RegExp(filterText, "i");
        treeScroller.treeView.view.proxyModel.invalidateFilter();
    }

    property var filterObject: QtObject {
        property var filter: /.*/

        function filterAcceptsItem(item) {
            return item.column == 0 && filter.test(item.display);
        }
    }

    Connections {
        target: self
        onSelectedSceneChanged: {
            clearFilter();
        }
        onSelectedIndexChanged: {
            clearFilter();
            treeView.columnSpacing = 4
            treeView.clamp = true;
        }
    }

    WGScrollView {
        id: treeScroller
        anchors.top: searchBox.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 8 // leaves just enought space for conventional slider

        property alias treeView: treeView

        WGPropertyTreeView {
            id: treeView
            model: typeof sourceModel != "undefined" ? sourceModel : null
            columnSpacing: 0
            filterObject: root.filterObject
            clamp: false
        }
    }
}
