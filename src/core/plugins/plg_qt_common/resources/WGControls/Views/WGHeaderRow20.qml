import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Private 2.0

/** Shared component for the list of column headers in the WGItemViews.
\ingroup wgcontrols*/
Item {
    id: row
    objectName: "WGHeaderRow"
    WGComponent { type: "WGHeaderRow20" }

    width: view.totalColumnsWidth != 0 ? view.totalColumnsWidth : 1024
    height: columns.height != 0 ? columns.height : 1024
    clip: true

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
        id: columns
        x: view.visibleOffset
        spacing: view.columnSpacing

        Repeater {
            model: WGRangeProxy {
				id: rangeProxy
                sourceModel: view.headerRowModel

                property var range: view.visibleColumns
                onRangeChanged: {
                    if (range.y >= range.x) {
                        setRange(range.x, 0, range.y, 0);
                    }
                }
            }

            delegate: Item {
                id: column
                objectName: "HeaderColumn_" + itemIndex
                width: view.getColumnWidth(itemIndex)
                height: childrenRect.height
                clip: true

                property var itemIndex: view.visibleColumns.x + index

                Connections {
                    target: view.extendedModel
                    onLayoutChanged: {
                        column.itemIndexChanged();
                    }
                }

                DropArea {
                    id: dropArea
                    anchors.fill: parent
                    keys: row.view.headerMimeTypes(Qt.Horizontal)
                    clip : true

                    onDropped: {
                        var mimeData = {};
                        for (var i = 0; i < drop.formats.length; ++i) {
                            var format = drop.formats[i];
                            var value = drop.getDataAsArrayBuffer(format);
                            mimeData[format] = value;
                        }

                        if (view.dropHeaderMimeData(mimeData, drop.proposedAction, itemIndex, Qt.Horizontal)) {
                            drop.acceptProposedAction();
                        }
                    }
                }

                MouseArea {
                    id: columnMouseArea
                    objectName: "HeaderColumnMouseArea"
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton | Qt.LeftButton;
                    drag.target: columnMouseArea

                    Drag.active: drag.active
                    Drag.dragType: Drag.Automatic
                    Drag.mimeData: view.headerMimeData([itemIndex], Qt.Horizontal)
                    Drag.proposedAction: Qt.MoveAction
                    Drag.supportedActions: Qt.MoveAction

                    onClicked: {
                        if (mouse.button == Qt.RightButton) {
                            contextMenu.contextObject = itemIndex;
                            contextMenu.popup();
                            return;
                        }

                        if (internal.sortColumn == view.sourceColumn(itemIndex)) {
                            if (internal.order == Qt.AscendingOrder) {
                                internal.order = Qt.DescendingOrder;
                            }
                            else {
                                internal.order = Qt.AscendingOrder;
                            }
                        }
                        else {
                            internal.sortColumn = view.sourceColumn(itemIndex);
                            internal.order = Qt.AscendingOrder;
                        }
                        view.proxyModel.sort(itemIndex, internal.order);
                    }

                    Drag.onDragStarted: {
                            Drag.hotSpot.x = mouseX;
                            Drag.hotSpot.y = mouseY;
                    }

                    Drag.onDragFinished: {
                        makeFakeMouseRelease();
                    }
                }

                Item {
                    id: itemContainer
                    objectName: "HeaderItemContainer"
                    width: sortIndicator.x
                    height: childrenRect.height
                    clip: true

                    /** Loader of the custom header cell component.*/
                    Loader {
                        id: headerDelegate
                        sourceComponent: view.getHeaderDelegate(itemIndex)

                        /** Provides the custom header cell component access to header roles.*/
                        property var headerData : model
                        /** Provides the custom header cell component with the column width.*/
                        property int headerWidth : sortIndicator.x
                        /** Exposes current column index to the custom cell component.*/
                        property int headerColumnIndex: view.sourceColumn(itemIndex)

                        property var getImplicitWidths: function() {
                            return Qt.vector3d(item != null ? item.implicitWidth : 0, 0, -1);
                        }

						Connections {
							target: rangeProxy
							onRowsChanged: {
								if (index >= first && index <= last) {
									headerDelegate.headerDataChanged();
								}
							}
						}

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
                        var indicatorWidth = sortIndicator.width;
                        var implicitWidths = headerDelegate.getImplicitWidths();
                        view.updateImplicitColumnWidth(itemIndex,
                            implicitWidths.x + indicatorWidth,
                            implicitWidths.y + indicatorWidth,
                            implicitWidths.z != 1 ? implicitWidths.z + indicatorWidth : 1)
                    }
                }
                Image
                {
                    id: sortIndicator

                    x: column.width - width
                    source: {
                        if (internal.sortColumn == view.sourceColumn(itemIndex)) {
                            if (internal.order == Qt.AscendingOrder) {
                                return "qrc:///WGControls/icons/sort_up_10x10.png";
                            }
                            else {
                                return "qrc:///WGControls/icons/sort_down_10x10.png";
                            }
                        }
                        else {
                            return "qrc:///WGControls/icons/sort_blank_10x10.png";
                        }
                    }

                    onWidthChanged: {
                        itemContainer.updateColumnImplicitWidths();
                    }
                }
            }
        }
    }
}
