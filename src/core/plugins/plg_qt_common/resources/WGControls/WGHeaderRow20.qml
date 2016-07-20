import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0


Item {
    id: itemRow
    objectName: "WGHeaderRow"

    width: childrenRect.width != 0 ? childrenRect.width : 1024
    height: childrenRect.height != 0 ? childrenRect.height : 1024
	
	property var view

    /* MOVE INTO STYLE*/
    Rectangle {
        id: backgroundArea
        anchors.fill: row
        color: palette.midDarkColor
        opacity: 1
        visible: true
    }
    /**/

	QtObject {
		id: internal
		property int sortColumn: -1
		property var order: Qt.AscendingOrder
	}

    Row {
        id: row
		spacing: view.columnSpacing

        Repeater {
            model: columnCount()

            Item {
                width: view.columnWidths[index]
                height: childrenRect.height
                clip: true
                MouseArea {
                    width: view.columnWidths[index]
                    height: row.height
                    acceptedButtons: Qt.RightButton | Qt.LeftButton;

                    onClicked: {
                        if (internal.sortColumn == index) {
							if (internal.order == Qt.AscendingOrder) {
								internal.order = Qt.DescendingOrder;
							}
							else {
								internal.order = Qt.AscendingOrder;
							}
						}
						else {
							internal.sortColumn = index;
							internal.order = Qt.AscendingOrder;
						}
						view.model.sort( internal.sortColumn, internal.order );
                    }
                }

                // header component
                Loader {
                    id: columnDelegateLoader
                    property var headerData: view.columnSequence.length <= index ? view.headerData[index] :  view.headerData[view.columnSequence[index]]
                    property var headerWidth: sortIndicator.x
                    sourceComponent: view.headerDelegates[index]
                }

				Image {
					id: sortIndicator

					x: parent.width - width
					source: {
						if (internal.sortColumn == index) {
							if (internal.order == Qt.AscendingOrder) {
								return "icons/sort_up_10x10.png";
							}
							else {
								return "icons/sort_down_10x10.png";
							}
						}
						else {
							return "icons/sort_blank_10x10.png";
						}
					}
                }
            }
        }
    }
}

