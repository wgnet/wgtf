import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGCopyableFunctions 1.0

WGPanel {
	title: "ListFilter Test"
	layoutHints: { 'test': 0.1 }

	property var sourceModel: source
	color: palette.mainWindowColor
	
	Label {
		id: searchBoxLabel
		x: testListView.leftMargin
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
	
	WGFilteredListModel {
		id: filteredListModel
		source: sourceModel

		filter: WGTokenizedStringFilter {
			id: stringFilter
			filterText: searchBox.text
			splitterChar: " "
			itemRole: "value"
		}

		HeaderFooterTextExtension {}
		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: filteredListModelSelection
			multiSelect: true
		}
	}

	WGListView {
		id: testListView
		anchors.top: searchBox.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		backgroundColourMode: alternatingRowBackgroundColours
	    showColumnHeaders: true
	    showColumnFooters: true
	    model: filteredListModel
		selectionExtension: filteredListModelSelection
		columnDelegates: [columnDelegate]

		Component {
			id: columnDelegate

			Item {
				Layout.fillWidth: true
				Layout.preferredHeight: testListView.minimumRowHeight
				
				Text {
					clip: true
					anchors.left: parent.left
					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.margins: 4
					verticalAlignment: Text.AlignVCenter
					text: itemData.value
					color: palette.textColor
				}
			}
		}
	}
}
