import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 1.0

WGPanel {
	title: "MultiColumn List Test"
	layoutHints: { 'test': 0.1 }

	property var sourceModel: source
    property var topControlsHeight: 20
	color: palette.mainWindowColor


    Button {
        id: addColumnButton
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: topControlsHeight
        text: "Add Column"

        onClicked: {
            var newList = [];
            newList.length = testListView.columnSequence.length + 1;
            for (i in newList)
            {
                newList[i] = 0;
            }
            testListView.columnSequence = newList;
        }
    }

    Button {
        id: removeColumnButton
        anchors.top: addColumnButton.bottom
        anchors.left: parent.left
        width: parent.width
        height: topControlsHeight
        text: "Remove Column"

        onClicked: {
            if ( testListView.columnSequence.length > 0 )
            {
                var newList = [];
                newList.length = testListView.columnSequence.length - 1;
                for (i in newList)
                {
                    newList[i] = 0;
                }
                testListView.columnSequence = newList;
            }
        }
    }
	
    WGListModel {
		id: listModel
		source: sourceModel

        HeaderFooterTextExtension {}
        ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: listModelSelection
			multiSelect: true
		}
	}	


	WGListView {
		id: testListView
        anchors.top: removeColumnButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        backgroundColourMode: alternatingRowBackgroundColours
	    showColumnHeaders: true
	    showColumnFooters: true
	    model: listModel
        selectionExtension: listModelSelection
		columnDelegates: [columnDelegate]
		columnSequence: [0, 0, 0]

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
