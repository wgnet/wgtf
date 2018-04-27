import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Private 2.0

/** Shared component for the list of column footers in the WGItemViews.
\ingroup wgcontrols */
Item {
    id: row
    objectName: "WGFooterRow"
    WGComponent { type: "WGFooterRow20" }

    width: view.totalColumnsWidth != 0 ? view.totalColumnsWidth : 1024
    height: columns.height != 0 ? columns.height : 1024
    clip: true

    /** This is a link to the containing WGItemView.*/
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
                objectName: "FooterColumn_" + itemIndex
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

                /** Cell inside styling.*/
                Item {
                    id: itemContainer
                    objectName: "FooterItemContainer"
                    width: column.width
                    height: childrenRect.height
                    clip: true

                    /** Loader of the custom header cell component.*/
                    Loader {
                        id: footerDelegate
                        sourceComponent: view.getFooterDelegate(itemIndex)

                        /** Provides the custom header cell component access to header roles.*/
                        property var headerData: model
                        /** Exposes current column index to the custom cell component.*/
                        property int headerColumnIndex: view.sourceColumn(itemIndex)

                        property var getImplicitWidths: function() {
                            return Qt.vector3d(item != null ? item.implicitWidth : 0, 0, -1);
                        }

						Connections {
							target: rangeProxy
							onRowsChanged: {
								if (index >= first && index <= last) {
									footerDelegate.headerDataChanged();
								}
							}
						}

                        onLoaded: {
                            item.parent = itemContainer
                            itemContainer.updateColumnImplicitWidths();
                        }
                    }

                    Connections {
                        target: footerDelegate.item
                        onImplicitWidthChanged: {
                            if (footerDelegate.status === Loader.Ready) {
                                itemContainer.updateColumnImplicitWidths();
                            }
                        }
                    }

                    /** Updates the implicit widths of the row,
                    when custom cell components' implict widths change.*/
                    function updateColumnImplicitWidths()
                    {
                        var implicitWidths = footerDelegate.getImplicitWidths();
                        view.updateImplicitColumnWidth(itemIndex,
                            implicitWidths.x,
                            implicitWidths.y,
                            implicitWidths.z);
                    }
                }
            }
        }
    }
}

