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
        property var minX: 0
        property var maxX: row.width - lastColumnHandleSpacer.width
		Row {
			 id: columnsLayout
			spacing: view.columnSpacing

			Repeater {
				model: columnCount()

				delegate: Item {
                    id: columnContainer
                    objectName: "HeaderColumnContainer_" + index
                    width: view.columnWidths[index]
                    height: childrenRect.height
                    clip: true

					DropArea {
						id: dropArea
						anchors.fill: column
						keys: itemRow.view.headerMimeTypes(Qt.Horizontal)

						onDropped: {
							var mimeData = {};
							for (var i = 0; i < drop.formats.length; ++i) {
								var format = drop.formats[i];
								var value = drop.getDataAsArrayBuffer(format);
								mimeData[format] = value;
							}

							if (view.dropHeaderMimeData(mimeData, drop.proposedAction, index, Qt.Horizontal)) {
								drop.acceptProposedAction();
							}
						}
					}

                    MouseArea {
                        id: columnMouseArea
                        objectName: "HeaderColumnMouseArea"
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: column.top
                        anchors.bottom: column.bottom
                        acceptedButtons: Qt.RightButton | Qt.LeftButton;
                        drag.target: columnMouseArea

                        Drag.active: drag.active
                        Drag.dragType: Drag.Automatic 
                        Drag.proposedAction: Qt.MoveAction
                        Drag.supportedActions: Qt.MoveAction | Qt.CopyAction

                        onClicked: {
                            if(mouse.button == Qt.RightButton) {
                                contextMenu.contextObject = index;
                                contextMenu.popup();
                                return;
                            }

                            if(internal.sortColumn == view.sourceColumn(index)) {
                                if(internal.order == Qt.AscendingOrder) {
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
                            Drag.hotSpot.x = mouseX;
                            Drag.hotSpot.y = mouseY;
                        }

                        Drag.onDragFinished: {
                            makeFakeMouseRelease();
                        }
                    }

                    /** Cell with styling, excluding indent gap in front of first cell.*/
                    Item {
                        id: column
                        objectName: "HeaderColumn"
                        x: Math.max(row.minX - columnContainer.x, 0)
                        width: Math.max(Math.min(row.maxX - columnContainer.x, columnContainer.width) - x, 0)
                        height: childrenRect.height
                        clip: true

                        /** Specifies minimum bound for cell.*/
                        property var minX: 0
                        /** Specifies maximum bound for cell.*/
                        property var maxX: sortIndicator.x

                        /** Cell inside styling.*/
                        Item {
                            id: itemContainer
                            objectName: "HeaderItemContainer"
                            x: column.minX
                            width: Math.max(column.maxX - x, 0)
                            height: childrenRect.height
                            clip: true

                            /** Loader of the custom header cell component.*/
                            Loader {
                                id: headerDelegate
                                sourceComponent: view.getHeaderDelegate(index)

                	            /** Provides the custom header cell component access to header roles.*/
            	                property var headerData: view.columnSequence.length <= index ? view.headerData[index] :  view.headerData[view.columnSequence[index]]
					            /** Exposes current column index to the custom cell component.*/
                                property int headerColumnIndex: index

                                onLoaded: {
                                    item.parent = itemContainer
                                    itemContainer.updateColumnImplicitWidths();
                                }
                            }

                            Connections {
                                target: headerDelegate.item
                                onImplicitWidthChanged: {
                                    if (headerDelegate.status === Loader.Ready) {
                                        itemContainer.updateColumnImplicitWidths();
                                    }
                                }
                            }

                            /** Updates the implicit widths of the row,
                            when custom cell components' implict widths change.*/
                            function updateColumnImplicitWidths()
                            {
                                var implicitWidths = view.implicitColumnWidths;
                                var oldImplicitWidth = index in implicitWidths ? implicitWidths[index] : 0;
                                var indent = index === 0 ? row.minX : 0;
                                var newImplicitWidth = headerDelegate.item.implicitWidth + indent;
                                implicitWidths[index] = Math.max(oldImplicitWidth, newImplicitWidth);
                                view.implicitColumnWidths = implicitWidths;
                            }
                        }
						Image {
                            id: sortIndicator

                            x: column.width - width
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

        Item {
            id: lastColumnHandleSpacer
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: view.clamp ? 0 : columnSpacing
        }
    }
}

