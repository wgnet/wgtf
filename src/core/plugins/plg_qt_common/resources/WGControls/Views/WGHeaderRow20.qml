import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls.Private 2.0

import WGControls 2.0

/** Shared component for the list of column headers in the WGItemViews.
\ingroup wgcontrols*/
Item {
    id: itemRow
    objectName: "WGHeaderRow"
    WGComponent { type: "WGHeaderRow20" }

    width: childrenRect.width != 0 ? childrenRect.width : 1024
    height: childrenRect.height != 0 ? childrenRect.height : 1024
	
    /** This is a link to the containing WGItemView.*/
    property var view
	/** Indicator used for column dragging.*/
    property string dragKey: "headerKey"

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

	Connections {
		target: view.extendedModel
		onLayoutChanged: {
			internal.sortColumnChanged();
		}
	}

	WGContextMenu {
        id: contextMenu
        path: "WGHeaderRow"
		property var active: false

		onAboutToShow: {
			active = true
		}

		onAboutToHide: {
			active = false
		}

        WGAction {
            actionId: "WGHeaderRow|.Hide Column"
			active: contextMenu.active

            onTriggered: {
				view.hideColumn(contextMenu.contextObject);
            }
        }

		WGAction {
            actionId: "WGHeaderRow|.Show All Columns"
			active: contextMenu.active

            onTriggered: {
				view.showAllColumns();
            }
        }
	}

    Row {
        id: row
        spacing: view.columnSpacing

        Repeater {
            model: columnCount()

            DropArea {
				id: headerItem
                width: view.columnWidths[index]
                height: headerDelegateLoader.height
				keys: itemRow.view.headerMimeTypes(Qt.Horizontal)

				onDropped: {
					var mimeData = {};
					for (var i = 0; i < drop.formats.length; ++i) {
						var format = drop.formats[i];
						var value = drop.getDataAsArrayBuffer(format);
						mimeData[format] = value;
					}

					if (view.dropHeaderMimeData(mimeData, drop.proposedAction, index, Qt.Horizontal)) {
						drop.acceptProposedAction()
					}
				}

				MouseArea {
					id: mouseArea
					anchors.fill: parent
					acceptedButtons: Qt.RightButton | Qt.LeftButton;
					drag.target: mouseArea

					Drag.active: drag.active
					Drag.dragType: Drag.Automatic 
					Drag.mimeData: view.headerMimeData([index], Qt.Horizontal)
					Drag.proposedAction: Qt.MoveAction
                    Drag.supportedActions: Qt.MoveAction

					onClicked: {
						if (mouse.button == Qt.RightButton) {
							contextMenu.contextObject = index;
							contextMenu.popup();
							return;
						}

						if (internal.sortColumn == view.sourceColumn(index)) {
							if (internal.order == Qt.AscendingOrder) {
								internal.order = Qt.DescendingOrder;
							}
							else {
								internal.order = Qt.AscendingOrder;
							}
						}
						else {
							internal.sortColumn = view.sourceColumn(index);
							internal.order = Qt.AscendingOrder;
						}
						view.proxyModel.sort(internal.sortColumn, internal.order);
					}

					Drag.onDragStarted: {
						Drag.hotSpot.x = mouseX
						Drag.hotSpot.y = mouseY
					}

					Drag.onDragFinished: {
						makeFakeMouseRelease();
					}
				}
                    
	            /** Loader of the custom header cell component.*/
                Loader {
                    id: headerDelegateLoader
                    sourceComponent: view.getHeaderDelegate(index)

                	/** Provides the custom header cell component access to header roles.*/
            	    property var headerData: view.columnSequence.length <= index ? view.headerData[index] :  view.headerData[view.columnSequence[index]]
					/** Exposes current column index to the custom cell component.*/
                    property int headerColumnIndex: index

                    onLoaded: item.parent = headerItem
                }

                Image {
                    id: sortIndicator

                    x: parent.width - width
                    source: {
                        if (internal.sortColumn == view.sourceColumn(index)) {
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

