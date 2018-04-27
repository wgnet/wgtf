import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
	id: projectDataPanel

    color: palette.mainWindowColor
    title: "Project Data"
    layoutHints: { 'test': 0.1 }
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

		onTextChanged: {
            projectDataPanel.setFilter(text);
            searchBox.focus = true;
            deselect();
        }
    }

	function setFilter(text)
    {
        var filterText = "(" + text.replace(/ /g, "|") + ")";
        filterObject.filter = new RegExp(filterText, "i");
        testTreeView.view.proxyModel.invalidateFilter();
    }

    property var filterObject: QtObject {
        property var filter: /.*/

        function filterAcceptsItem(item) {
            return item.column == 0 && filter.test(item.display);
        }
    }

    WGPropertyTreeView {
        id: testTreeView
        anchors.top: searchBox.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: sourceModel
		filterObject: projectDataPanel.filterObject
    }
}
