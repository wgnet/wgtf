import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import QtQuick 2.4
import QtQuick.Controls.Private 1.0
import WGControls 2.0
import WGControls.Styles 2.0
import WGControls.Views 2.0

import "wg_view_selection.js" as WGViewSelection

/** Common view bass class extending the C++ WGItemView further.
This contains common QML shared by WGTableView, WGListView, and WGTreeView.
\ingroup wgcontrols */
WGItemView {
    id: root
    WGComponent { type: "WGItemViewCommon" }

    /** Model, wrapped by a sorting/filtering adapting layer.*/
    property var proxyModel: WGSortFilterProxy {
        id: sortFilterProxy
        sortObject: QtObject {
            function lessThan(left, right) {
                var sortColumn = left.column;
                return left[getColumnRole(sortColumn)] < right[getColumnRole(sortColumn)];
            }
        }
    }

    property alias internalModel: root.model
    property alias model: sortFilterProxy.sourceModel

    internalModel: proxyModel

    /** The object used for sorting operations.*/
    property alias sortObject: sortFilterProxy.sortObject
    /** The object used for filtering logic.*/
    property alias filterObject: sortFilterProxy.filterObject
    /** The WGListView, WGTreeView, or WGTableView using this component.*/
    property var view: parent
    /** The model containing the selected items/indices.*/
    property var selectionModel: itemSelectionModel
    /** The style component to allow custom view appearance.*/
    property var style: WGItemViewStyle {}
    /** Default role for the value used in a cell component.*/
    property var columnRole: "display"
    /** Specific value role per index to expose to cell components, overriding the default columnRole.*/
    property var columnRoles: []
    /** Default cell component used for the cells in the rows of the view body.*/
    property Component columnDelegate: Text {
        id: defaultColumnDelegate
        objectName: "defaultColumnDelegate_" + text
        text: itemValue !== undefined ? itemValue : ""
        color: palette.textColor
        verticalAlignment: Text.AlignVCenter
        height: defaultSpacing.minimumRowHeight

        WGToolTip
        {
            id: tooltip
            text: itemData.display !== undefined ? itemData.display : ""
        }
    }
    /** Specific components per index for view body cells.
    These override the default cell component for each index specified.
    For unspecified indices, the default cell component (columnDelegate) will be used.*/
    property var columnDelegates: []
    /** Alternate arrangement of model columns in the view, also allowing skipping or repeating of columns.*/
    property var columnSequence: []
    /** Default initial column width if none is specified.*/
    property real initialColumnWidth: 100
    /** Specific initial column widths per column.*/
    property var initialColumnWidths: []
    /** Ideal size of columns, for example to prevent contents from cutting off.*/
    property alias implicitColumnWidths: columnsFrame.implicitColumnWidths
    /** Actual column widths per column.*/
    property alias columnWidths: columnsFrame.columnWidths
    /** The minimum a column can be reduced to. */
    property alias minimumColumnWidth: columnsFrame.minimumColumnWidth
    /** Size of the gap between columns.*/
    property real columnSpacing: defaultSpacing.separatorWidth
    /** Clamp (fix) width of the view to the containing component and adjust contents when width resized.*/
    property bool clamp: true
    /** Optional specific header cell components, overriding the default header cell component.
    These override the default cell component for each index specified.
    For unspecified indices, the default cell component (headerDelegate) will be used.*/
    property var headerDelegates: []
    /** Optional specific footer cell components, overriding the default footer cell component.
    These override the default cell component for each index specified.
    For unspecified indices, the default cell component (footerDelegate) will be used.*/
    property var footerDelegates: []
    /** Default header cell component.*/
    property Component headerDelegate: null
    /** Default footer cell component.*/
    property Component footerDelegate: null
    /** Header item created from header row component.*/
    property Component header: headerDelegates.length > 0 || headerDelegate != null ? headerComponent : null
    /** Footer item created from footer row component.*/
    property Component footer: footerDelegates.length > 0 || footerDelegate != null ? footerComponent : null
    /** Default header row component.*/
    property Component headerComponent: WGHeaderRow {
        z:2
        view: root
    }
    /** Default footer row component.*/
    property Component footerComponent: WGFooterRow {
        z:2
        view: root
    }
    /** Specifies a custom view specific extension, adding unique features.
    This is an internally used property set by the derived view.*/
    property var viewExtension: null
    /** The list of extensions shared by all views.*/
    property var commonExtensions: [viewExtension, columnExtension, imageExtension]
    /** The combined common and view extensions.*/
    extensions: view.extensions.concat(commonExtensions)

    /** Signalled when the current index changes.*/
    signal currentChanged(var current, var previous)
    /** Signalled when the selection changes.*/
    signal selectionChanged(var selected, var deselected)

    /** Returns the amount of rows in the view.*/
    function rowCount() {
        return modelRowCount;
    }

    /** Returns the amount of columns in the view. This can be different to the model's column count.*/
    function columnCount() {
        var count = columnSequence.length;
        if (count == 0) {
            count = modelColumnCount;
        }
        return count;
    }

    /** Retreats up the view, first up a row, then up a level.*/
    function movePrevious(event) {
        var newIndex = viewExtension.getPreviousIndex(itemSelectionModel.currentIndex,
                                                      itemSelectionModel.model);
        WGViewSelection.updateSelection(event, itemSelectionModel, viewExtension, newIndex);
    }

    /** Advanced down the view, first down a level, then down a row.*/
    function moveNext(event) {
        var newIndex = viewExtension.getNextIndex(itemSelectionModel.currentIndex,
                                                  itemSelectionModel.model);
        WGViewSelection.updateSelection(event, itemSelectionModel, viewExtension, newIndex);
    }

    /** Retreats up the view with custom behaviour depending on the view.
    For example:
    A WGTreeView will move up a row if it can, collapse a level if open, otherwise move up a level.
    A WGTableView will move left if possible, otherwise move up.*/
    function moveBackwards(event) {
        var newIndex = viewExtension.getBackwardIndex(itemSelectionModel.currentIndex,
                                                      itemSelectionModel.model);
        WGViewSelection.updateSelection(event, itemSelectionModel, viewExtension, newIndex);
    }

    /** Advances down the view with custom behaviour depending on the view.
    For example:
    A WGTreeView will expand a level if closed, move down a level if it can, otherwise move down a row.
    A WGTableView will move right if possible, otherwise move down.*/
    function moveForwards(event) {
        var newIndex = viewExtension.getForwardIndex(itemSelectionModel.currentIndex,
                                                     itemSelectionModel.model);
        WGViewSelection.updateSelection(event, itemSelectionModel, viewExtension, newIndex);
    }

    /** Selects an index.*/
    function select(mouse, index) {
        WGViewSelection.updateSelection(mouse, itemSelectionModel, viewExtension, index)
    }

    function getColumnRole(index) {
        var column = sourceColumn(index);
        if (column < columnRoles.length) {
            return columnRoles[column];
        }
        return columnRole;
    }

    function getColumnDelegate(index) {
        var column = sourceColumn(index);
        if (column < columnDelegates.length) {
            return columnDelegates[column];
        }
        return columnDelegate;
    }

    function getHeaderDelegate(index) {
        var column = sourceColumn(index);
        if (column < headerDelegates.length) {
            return headerDelegates[column];
        }
        return headerDelegate;
    }

    function getFooterDelegate(index) {
        var column = sourceColumn(index);
        if (column < footerDelegates.length) {
            return footerDelegates[column];
        }
        return footerDelegate;
    }

    ColumnExtension {
        id: columnExtension
    }

    ImageExtension {
        id: imageExtension
    }

    /** This controls the column sizes of the entire view.*/
    WGColumnsFrame {
        id: columnsFrame
        x: root.view.contentItem.x + root.view.originX
        y: 0
        height: root.view.height
        columnCount: root.columnCount()
        initialColumnWidth: root.initialColumnWidth
        initialColumnWidths: root.initialColumnWidths
        columnSpacing: root.columnSpacing
        availableWidth: root.view.width - Math.max(contentItem.x, 0)
        clamp: root.clamp
        handleDelegate: style.columnHandle
    }

    ItemSelectionModel {
        id: itemSelectionModel
        model: extendedModel

        /** The start index when selecting a range using the shift key.*/
        property var shiftIndex

        Component.onCompleted: {
            shiftIndex = model.index(0, 0);
        }

        // Pass signals up to parent
        onCurrentChanged: root.currentChanged(current, previous)
        onSelectionChanged: root.selectionChanged(selected, deselected)
    }
}
