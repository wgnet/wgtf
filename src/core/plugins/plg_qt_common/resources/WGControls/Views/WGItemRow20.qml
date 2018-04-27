import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0

/** Shared component for the list of cells in a row of a WGItemView.
\ingroup wgcontrols */
Item {
    id: row
    objectName: "WGItemRow_" + index
    WGComponent { type: "WGItemRow20" }

    width: ( view && view.totalColumnsWidth != 0 ) ? view.totalColumnsWidth : 1024
    height: columns.height != 0 ? columns.height : 40
    clip: true

    /** This is a link to the containing WGItemView.*/
    property var view
    /** Specifies the level down a tree hierarchy.*/
    property var depth: 0
    /** The index for this row. This is a model index exposed by the model.*/
    property var rowIndex: modelIndex
    /** Switch to load asynchronously or synchronously.*/
    property bool asynchronous: false
    /** item row index.*/
    property int itemRowIndex: index

    /** Signals that this item received a mouse press.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.*/
    signal itemPressed(var mouse, var itemIndex)
    /** Signals that this item was clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.*/
    signal itemClicked(var mouse, var itemIndex)
    /** Signals that this item was double clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.*/
    signal itemDoubleClicked(var mouse, var itemIndex)
    /** Signals that this item was double clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.*/
    signal itemHovered(var itemIndex)

    /* Aliasing the isSelected state for automated testing.*/
    property alias isSelected: rowBackground.isSelected
	property alias isCurrent: rowBackground.isCurrent
	property alias isHovered: rowBackground.isHovered
    property var hoveredIndex: null

    Loader {
        id: rowBackground
        objectName: "RowBackground"
        anchors.fill: parent
        asynchronous: row.asynchronous
        sourceComponent: view ? view.style.rowBackground : null

        property bool isSelected: view ? view.selectionModel.isSelected(modelIndex) : false
        property bool isCurrent: view ? view.selectionModel.currentIndex === modelIndex : false
        property bool isHovered: hoveredIndex != null

        /** The depth of the item row in the model (0 if the view is not a tree).*/
        property int itemDepth: itemRow.depth

        /** Checks if the row has children if it is in a view that supports this.*/
        property bool hasChildren: typeof model.hasChildren != "undefined" ? model.hasChildren : false

        Connections {
            target: view
            onSelectionChanged: {
                rowBackground.isSelected = Qt.binding( function() { return view.selectionModel.isSelected(modelIndex); } )
            }
        }
    }

    Row {
        id: columns
        x: view ? view.visibleOffset: 0
        spacing: view ? view.columnSpacing : 0

        Repeater {
            model: WGRangeProxy {
                id: rangeProxy
                sourceModel: typeof(columnModel) != "undefined" ? columnModel : null

                property var range: view ? view.visibleColumns : Qt.vector2d( 0, 0 )
                onRangeChanged: {
                    if (range.y >= range.x) {
                        setRange(range.x, 0, range.y, 0);
                    }
                }
            }
            delegate: Item {
                id: column
                objectName: "Column_" + itemIndex
                width: view ? view.getColumnWidth(itemIndex) : 0
                height: childrenRect.height
                clip: true

                property var itemIndex: ( view ? view.visibleColumns.x : 0 ) + index
                /** Checks if this item is selected.*/
                property bool isSelected: view ? view.selectionModel.isSelected(modelIndex) : false
                /** Checks if this is the current item.*/
                property bool isCurrent: view ? view.selectionModel.currentIndex === modelIndex : false;
                /** Checks if the mouse is in this item.*/
                property bool isHovered: columnMouseArea.isHovered

                Connections {
                    target: view ? view.extendedModel : null
                    onLayoutChanged: {
                        column.itemIndexChanged();
                    }
                }

                onIsHoveredChanged: {
                    if (isHovered) {
                        row.hoveredIndex = modelIndex;
                        row.itemHovered(modelIndex)
                    }
                    else if (row.hoveredIndex == modelIndex) {
                        row.hoveredIndex = null;
                    }
                }

                Connections {
                    target: view
                    onSelectionChanged: {
                        column.isSelected = Qt.binding( function() { return view.selectionModel.isSelected(modelIndex); } )
                    }
                }

                /** Optional background behind a single cell, specified in the style component.*/
                Loader {
                    id: columnBackground
                    objectName: "ColumnBackground"
                    anchors.left: columnHeader.left
                    anchors.right: columnFooter.right
                    height: itemContainer.height
                    asynchronous: row.asynchronous
                    sourceComponent: view ? view.style.columnBackground : null

                    property bool isSelected: column.isSelected
                    property bool isCurrent: column.isCurrent
                    property bool isHovered: column.isHovered
                }

                DropArea {
                    id: dropArea
                    anchors.fill: columnBackground
                    keys: row.view ? row.view.mimeTypes() : {}

                    onDropped: {
                        var mimeData = {};
                        for (var i = 0; i < drop.formats.length; ++i) {
                            var format = drop.formats[i];
                            var value = drop.getDataAsArrayBuffer(format);
                            mimeData[format] = value;
                        }

                        if (view.dropMimeData(mimeData, drop.proposedAction, modelIndex)) {
                            drop.acceptProposedAction();
                        }
                    }
                }

                MouseArea {
                    id: columnMouseArea
                    objectName: "ColumnMouseArea"
                    anchors.fill: columnBackground
                    acceptedButtons: Qt.RightButton | Qt.LeftButton;
                    drag.target: columnMouseArea
                    hoverEnabled: true

                    property var dragActive: drag.active
                    property bool isHovered: false
                    onDragActiveChanged: {
                        if (drag.active) {
                            Drag.mimeData = view.mimeData(row.view.selectionModel.selectedIndexes)
                            Drag.active = true
                        }
                    }

                    onContainsMouseChanged: { isHovered = containsMouse }

                    Drag.dragType: Drag.Automatic
                    Drag.proposedAction: Qt.MoveAction
                    Drag.supportedActions: Qt.MoveAction | Qt.CopyAction

                    /**
                    * Signals that this item received a mouse press.
                    * \param mouse The mouse data at the time.
                    * \param modelIndex The index of the item.
                    * \note cancel isHovered as RMC with context menus do not trigger onExited
                    */
                    onPressed: {
                        isHovered = false;
                        itemPressed(mouse, modelIndex);
                    }

                    /**
                    * Signals that this item was clicked.
                    * \param mouse The mouse data at the time.
                    * \param modelIndex The index of the item.
                    * \note cancel isHovered as RMC with context menus do not trigger onExited
                    */
                    onClicked: {
                        isHovered = false;
                        itemClicked(mouse, modelIndex);
                    }

                    /**
                    * Signals that this item was double clicked.
                    * \param mouse The mouse data at the time.
                    * \param modelIndex The index of the item.
                    * \note cancel isHovered as RMC with context menus do not trigger onExited
                    */
                    onDoubleClicked: {
                        isHovered = false;
                        itemDoubleClicked(mouse, modelIndex);
                    }

                    Drag.onDragStarted: {
                        Drag.hotSpot.x = mouseX
                        Drag.hotSpot.y = mouseY
                    }

                    Drag.onDragFinished: {
                        makeFakeMouseRelease();
                    }
                }

                /** Cell inside styling.*/
                Item {
                    id: itemContainer
                    objectName: "ItemContainer"
                    anchors.left: columnHeader.right
                    anchors.right: columnFooter.left
                    height: childrenRect.height
                    clip: true

                    Loader {
                        id: itemDelegate
                        objectName: "ItemDelegate"
                        asynchronous: row.asynchronous
                        visible: status === itemDelegate.Ready
                        sourceComponent: view ? view.getColumnDelegate(itemIndex) : null

                        /** Exposes column width to the custom cell component.*/
                        property int columnWidth: parent.width
                        /** Exposes the value role to the custom cell component.
                        The role to use as the value role is specified in view.columnRoles.*/
                        property var itemValue: view ? model[view.getColumnRole(itemIndex) ] : null
                        /** Exposes all roles to the custom cell component.*/
                        property var itemData: model
                        /** Exposes the hasChildren role to the custom cell component */
                        property var hasModelChildren: rowHeader.itemData.hasModelChildren
                        /** Exposes depth in hierarchy to the custom cell component.*/
                        property var itemDepth: depth
                        /** Exposes selected check to the custom cell component.*/
                        property bool isSelected: column.isSelected
                        /** Exposes current index check to the custom cell component.*/
                        property bool isCurrent: column.isCurrent
                        /** Exposes hovered check to the custom cell component.*/
                        property bool isHovered: column.isHovered
                        /** Exposes current row index to the custom cell component.*/
                        property int itemRowIndex: row.itemRowIndex
                        /** Exposes current column index to the custom cell component.*/
                        property int itemColumnIndex: view ? view.sourceColumn(itemIndex) : -1;
                        /** Exposes current modelIndex to the custom cell component.*/
                        property var itemModelIndex: modelIndex
                        /** Exposes current rowIndex to the custom cell component.*/
                        property var itemRowModelIndex: row.rowIndex
                        /** Exposes mouse area values to the custom cell component.*/
                        property var colMouseArea: columnMouseArea;

                        /** Exposes row selected check to the custom cell component.*/
                        property bool rowIsSelected: row.isSelected
                        /** Exposes row current index check to the custom cell component.*/
                        property bool rowIsCurrent: row.isCurrent
                        /** Exposes row hovered check to the custom cell component.*/
                        property bool rowIsHovered: row.isHovered

                        property var getImplicitWidths: function() {
                            return Qt.vector3d(item != null ? item.implicitWidth : 0, 0, -1);
                        }

                        Connections {
                            target: rangeProxy
                            onRowsChanged: {
                                if (index >= first && index <= last) {
                                    itemDelegate.itemDataChanged();
                                }
                            }
                        }

                        onLoaded: {
                            item.parent = itemContainer
                            itemContainer.updateColumnImplicitWidths();
                        }
                    }

                    Connections {
                        target: itemDelegate.item
                        onImplicitWidthChanged: {
                            if (itemDelegate.status === Loader.Ready) {
                                itemContainer.updateColumnImplicitWidths();
                            }
                        }
                    }

                    /** Updates the implicit widths of the row,
                    when custom cell components' implict widths change.*/
                    function updateColumnImplicitWidths()
                    {
                        var headerWidth = columnHeader.width + columnHeader.x;
                        var footerWidth = columnFooter.width
                        var implicitWidths = itemDelegate.getImplicitWidths();
                        view.updateImplicitColumnWidth(itemIndex,
                            implicitWidths.x + (headerWidth + footerWidth),
                            implicitWidths.y + (headerWidth + footerWidth),
                            implicitWidths.z != -1 ? implicitWidths.z + (headerWidth + footerWidth) : -1);
                    }
                }

                /** Styling in front of the cell.*/
                Loader {
                    id: columnHeader
                    objectName: "ColumnHeader"
                    asynchronous: row.asynchronous
                    x: Math.max(rowHeader.width - (columns.x + column.x), 0)
                    height: itemContainer.height
                    width: sourceComponent ? sourceComponent.width : 0
                    sourceComponent: view ? view.style.columnHeader : null

                    onXChanged: {
                        itemContainer.updateColumnImplicitWidths();
                    }

                    onWidthChanged: {
                        itemContainer.updateColumnImplicitWidths();
                    }

                    /** Exposes roles to custom style component.*/
                    property var itemData: model
                    /** Exposes depth in hierarchy to custom style component.*/
                    property var itemDepth: depth

                    Connections {
                        target: rangeProxy
                        onRowsChanged: {
                            if (index >= first && index <= last) {
                                columnHeader.itemDataChanged();
                            }
                        }
                    }
                }

                /** Styling at end of the cell.*/
                Loader {
                    id: columnFooter
                    objectName: "ColumnFooter"
                    asynchronous: row.asynchronous
                    x: Math.min(rowFooter.x - (columns.x + column.x), column.width) - width
                    height: itemContainer.height
                    width: sourceComponent ? sourceComponent.width : 0
                    sourceComponent: view ? view.style.columnFooter : null

                    onWidthChanged: {
                        itemContainer.updateColumnImplicitWidths();
                    }

                    /** Exposes roles to custom style component.*/
                    property var itemData: model
                    /** Exposes depth in hierarchy to custom style component.*/
                    property var itemDepth: depth

                    Connections {
                        target: rangeProxy
                        onRowsChanged: {
                            if (index >= first && index <= last) {
                                columnFooter.itemDataChanged();
                            }
                        }
                    }
                }
            }
        }
    }

    /** This loads the part in front of the row.*/
    Loader {
        id: rowHeader
        objectName: "RowHeader"
        asynchronous: row.asynchronous
        height: columns.height
        width: sourceComponent ? sourceComponent.width : 0
        sourceComponent: view ? view.style.rowHeader : null

        /** Exposes roles to custom style component.*/
        property var itemData: model
        /** Exposes depth in hierarchy to custom style component.*/
        property var itemDepth: depth
    }

    /** This loads the part behind the row.*/
    Loader {
        id: rowFooter
        objectName: "RowFooter"
        asynchronous: row.asynchronous
        x: row.width - width
        height: columns.height
        width: sourceComponent ? sourceComponent.width : 0
        sourceComponent: view ? view.style.rowFooter : null

        /** Exposes roles to custom style component.*/
        property var itemData: model
        /** Exposes depth in hierarchy to custom style component.*/
        property var itemDepth: depth
    }
}

