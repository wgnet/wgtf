import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
    id: root
    title: "SceneBrowser"
    layoutHints: { 'scenebrowser': 0.1 }
    color: palette.mainWindowColor
	property var sourceModel: getListModel

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

        onTextChanged: setFilter(text)
    }

    function setFilter(text)
    {
        var filterText = "(" + text.replace(/ /g, "|") + ")";
        filterObject.filter = new RegExp(filterText, "i");
        listScroller.demoListView.view.proxyModel.invalidateFilter();
    }

    property var filterObject: QtObject {
        property var filter: /.*/

        function filterAcceptsItem(item) {
            return filter.test(item.value);
        }
    }

    function updateRow(row) {
        if (listScroller.demoListView.currentRow != row) {
            var index = listScroller.demoListView.model.index(row, 0);
            var flags = 1 + 2; // Clear | Select

            listScroller.demoListView.currentRow = row;
            listScroller.demoListView.selectionModel.setCurrentIndex(index, flags);
        }
    }

	Connections {
		target: self
		onSelectedIndexChanged: {
			updateRow(selectedIndex);
		}
	}

    WGScrollView {
        id: listScroller
        anchors.top: searchBox.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        property alias demoListView: demoListView

        WGListView {
            id: demoListView
            model: typeof sourceModel != "undefined" ? sourceModel : null
            filterObject: root.filterObject
            columnDelegates: [columnDelegate]

            onCurrentRowChanged: {
				if(selectedIndex != currentRow) {
					selectObject(currentRow);
				}
			}

            Component {
                id: columnDelegate

                Item {
                    width: parent.width
                    height: defaultSpacing.minimumRowHeight
                    implicitWidth: displayText.implicitWidth

                    Text {
                        id: displayText
                        clip: true
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 4
                        verticalAlignment: Text.AlignVCenter
                        text: itemData != null ? itemData.value.name : ""
                        color: palette.textColor
                    }
                }
            }
        }
    }
}
