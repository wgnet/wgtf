import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls.Private 2.0

/** Shared component for the list of column footers in the WGItemViews.
\ingroup wgcontrols */
Item {
    id: itemRow
    objectName: "WGFooterRow"
    WGComponent { type: "WGFooterRow20" }

    width: childrenRect.width != 0 ? childrenRect.width : 1024
    height: childrenRect.height != 0 ? childrenRect.height : 1024
	
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
                    objectName: "FooterColumnContainer_" + index
                    width: view.columnWidths[index]
                    height: childrenRect.height
                    clip: true

                    /** Cell with styling, excluding indent gap in front of first cell.*/
                    Item {
                        id: column
                        objectName: "FooterColumn"
                        x: Math.max(row.minX - columnContainer.x, 0)
                        width: Math.max(Math.min(row.maxX - columnContainer.x, columnContainer.width) - x, 0)
                        height: childrenRect.height
                        clip: true

                        /** Specifies minimum bound for cell.*/
                        property var minX: 0
                        /** Specifies maximum bound for cell.*/
                        property var maxX: column.width

                        /** Cell inside styling.*/
                        Item {
                            id: itemContainer
                            objectName: "FooterItemContainer"
                            x: column.minX
                            width: Math.max(column.maxX - x, 0)
                            height: childrenRect.height
                            clip: true

                            /** Loader of the custom header cell component.*/
                            Loader {
                                id: footerDelegate
                                sourceComponent: view.getFooterDelegate(index)

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
                                var implicitWidths = view.implicitColumnWidths;
                                var oldImplicitWidth = index in implicitWidths ? implicitWidths[index] : 0;
                                var indent = index === 0 ? row.minX : 0;
                                var newImplicitWidth = footerDelegate.item.implicitWidth + indent;
                                implicitWidths[index] = Math.max(oldImplicitWidth, newImplicitWidth);
                                view.implicitColumnWidths = implicitWidths;
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

