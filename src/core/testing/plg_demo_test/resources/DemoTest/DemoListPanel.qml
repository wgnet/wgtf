import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGCopyableFunctions 1.0

Rectangle {
	id: root
	property var title: "SceneBrowser"
	property var layoutHints: { 'scenebrowser': 0.1 }
	property var sourceModel: listSource
	color: palette.mainWindowColor
	
	Label {
		id: searchBoxLabel
		x: parent.x
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

	WGDataChangeNotifier {
        id: objectSelection
        source: CurrentIndexSource
        onDataChanged: {
            filteredListModelSelection.selectedIndex = demoListView.model.index(rootObjectIndex());
        }
    }

	WGDataChangeNotifier {
        id: listChangeNotifier
        source: CurrentListSource
        onDataChanged: {
            root.sourceModel = listSource;
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

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: filteredListModelSelection
			multiSelect: false
			onSelectionChanged: {
				updateRootObject(filteredListModel.indexRow(selectedIndex));
			}
		}
	}

	WGListView {
		id: demoListView
		anchors.top: searchBox.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		model: filteredListModel
		selectionExtension: filteredListModelSelection
		columnDelegates: [columnDelegate]
		Component {
			id: columnDelegate

			Item {
				Layout.fillWidth: true
				Layout.preferredHeight: demoListView.minimumRowHeight

				Text {
					clip: true
					anchors.left: parent.left
					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.margins: 4
					verticalAlignment: Text.AlignVCenter
					visible: true
					text: itemData != null ? itemData.value.name : ""
					color: palette.textColor
				}
			}
		}
	}
}
