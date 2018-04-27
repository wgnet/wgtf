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

    Connections {
        target: view.model
        ignoreUnknownSignals: true

        onModelChanged: {
            columnsFrame.columnsHandleTouched = false
            columnsFrame.windowWidthChanged = false
            columnsFrame.resetImplicitColumnWidths()
        }
    }

    internalModel: proxyModel

    /** The object used for sorting operations.*/
    property alias sortObject: sortFilterProxy.sortObject
    /** The object used for filtering logic.*/
    property alias filterObject: sortFilterProxy.filterObject
    /** The WGListView, WGTreeView, or WGTableView using this component.*/
    property var view: parent
    /** The model containing the extended models selected items/indices.*/
    property var selectionModel: extendedSelectionModel
    /** The model containing the internal models selected items/indices.*/
    property var internalSelectionModel: QtObject {
        property var itemSelectionModel: ItemSelectionModel {
            model: root.model
            property var supportMultiViewSelect: root.supportMultiViewSelect
        }
        property var mapToSource: function(proxyIndex) { return proxyIndex; }
        property var mapFromSource: function(sourceIndex) { return sourceIndex; }
        property var mapSelectionFromSource: function(selection) { return selection; }
    }
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
        // extra space added to give a bit of margin in column. implicitWidth for text objects is readonly.
        text: itemValue !== undefined ? itemValue + "  " : ""
        color: palette.textColor
        verticalAlignment: Text.AlignVCenter
        height: defaultSpacing.minimumRowHeight

        WGToolTipBase {
            text: itemData !== null && itemData !== undefined && itemData.tooltip !== undefined? itemData.tooltip : itemData !== null && itemData !== undefined && itemData.display !== undefined? itemData.display : ""
            tooltipArea: colMouseArea
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
    /** Actual column widths per column.*/
    property var getColumnWidth: columnsFrame.getColumnWidth
    property var updateImplicitColumnWidth: columnsFrame.updateImplicitColumnWidth
    /** The minimum a column can be reduced to. */
    property alias minimumColumnWidth: columnsFrame.minimumColumnWidth
    property alias totalColumnsWidth: columnsFrame.totalWidth
    property alias visibleOffset: columnsFrame.visibleOffset
    property alias visibleColumns: columnsFrame.visibleColumns
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
    property var columnExtension: createExtension("ColumnExtension")
    property var imageExtension: createExtension("ImageExtension")
    /** The list of extensions shared by all views.*/
    property var commonExtensions: [viewExtension, columnExtension, imageExtension]

    /** The combined common and view extensions.*/
    extensions: view.extensions.concat(commonExtensions)

    /*Specifies if view needs to support multiple selection, default value is true*/
    property bool supportMultiSelect: true
    /*! Override this for determine if the node can be selected */
    property var allowSelect: function(rowIndexToBeSelected, exisingSelectedIndexes, modifiers) {
        return true;
    }

    /** Signalled when the current index changes.*/
    signal currentChanged()
    /** Signalled when the selection changes.*/
    signal selectionChanged()

    Component.onCompleted: {
        // print an error message if columnSequence contains an invalid index
        var count = columnCount();
        for (var i = 0; i < columnSequence.length; ++i) {
            var index = columnSequence[i];
            if (index >= count) {
                console.exception("Error: columnSequence index [" + index + "] is greater than number of columns" );
            }
            if (index < 0) {
                console.exception("Error: columnSequence index [" + index + "] is negative" );
            }
        }
    }

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
        var newIndex = viewExtension.getPreviousIndex(extendedSelectionModel.currentIndex,
                                                      extendedModel);
        var selectedIndexes = extendedSelectionModel.selectedIndexes;
        if(allowSelect(newIndex, selectedIndexes, event.modifiers)) {
            WGViewSelection.updateSelection(event, extendedSelectionModel, viewExtension, newIndex, supportMultiSelect);
        }
    }

    /** Advanced down the view, first down a level, then down a row.*/
    function moveNext(event) {
        var newIndex = viewExtension.getNextIndex(extendedSelectionModel.currentIndex,
                                                  extendedModel);
        var selectedIndexes = extendedSelectionModel.selectedIndexes;
        if(allowSelect(newIndex, selectedIndexes, event.modifiers)) {
            WGViewSelection.updateSelection(event, extendedSelectionModel, viewExtension, newIndex, supportMultiSelect);
        }
    }

    /** Retreats up the view with custom behaviour depending on the view.
    For example:
    A WGTreeView will move up a row if it can, collapse a level if open, otherwise move up a level.
    A WGTableView will move left if possible, otherwise move up.*/
    function moveBackwards(event) {
        var newIndex = viewExtension.getBackwardIndex(extendedSelectionModel.currentIndex,
                                                      extendedModel);
        var selectedIndexes = extendedSelectionModel.selectedIndexes;
        if(allowSelect(newIndex, selectedIndexes, event.modifiers)) {
            WGViewSelection.updateSelection(event, extendedSelectionModel, viewExtension, newIndex, supportMultiSelect);
        }
    }

    /** Advances down the view with custom behaviour depending on the view.
    For example:
    A WGTreeView will expand a level if closed, move down a level if it can, otherwise move down a row.
    A WGTableView will move right if possible, otherwise move down.*/
    function moveForwards(event) {
        var newIndex = viewExtension.getForwardIndex(extendedSelectionModel.currentIndex,
                                                     extendedModel);
        var selectedIndexes = extendedSelectionModel.selectedIndexes;
        if(allowSelect(newIndex, selectedIndexes, event.modifiers)) {
            WGViewSelection.updateSelection(event, extendedSelectionModel, viewExtension, newIndex, supportMultiSelect);
        }
    }

    /** Selects an index.*/
    function select(mouse, index) {
        var selectedIndexes = extendedSelectionModel.selectedIndexes;
        if(allowSelect(index, selectedIndexes, mouse.modifiers)) {
            WGViewSelection.updateSelection(mouse, extendedSelectionModel, viewExtension, index, supportMultiSelect);
        }
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

    /** This controls the column sizes of the entire view.*/
    WGColumnsFrame {
        id: columnsFrame
        itemView: root
        handleDelegate: style.columnHandle
    }

    Item {
        id: extendedSelectionModel

        property var currentIndex: null
        property var selectedIndexes: []
        property bool hasSelection: false
        property var shiftIndex: null

        QtObject {
            id: internal
            property var shiftIndex: null

            function extendedIndex(sourceIndex) {
                var internalSelectionModelIndex = internalSelectionModel.mapFromSource(sourceIndex);
                var proxyIndex = proxyModel.mapFromSource(internalSelectionModelIndex);
                return root.extendedIndex(proxyIndex);
            }

            function getExtendedIndexes(sourceSelection) {
                var internalSelectionModelSelection = internalSelectionModel.mapSelectionFromSource(sourceSelection);
                var proxySelection = proxyModel.mapSelectionFromSource(internalSelectionModelSelection);
                return root.getExtendedIndexes(proxySelection);
            }

            function select(indexes, command) {
                if (internalSelectionModel.itemSelectionModel.model == null) {
                    return;
                }

                var sourceIndexes = [];
                for (var i = 0; i < indexes.length; ++i) {
                    sourceIndexes.push(sourceIndex(indexes[i]));
                }
                internalSelectionModel.itemSelectionModel.select(root.toItemSelection(sourceIndexes), command);
            }

            function setCurrentIndex(index, command) {
                if (internalSelectionModel.itemSelectionModel.model == null) {
                    return;
                }

                internalSelectionModel.itemSelectionModel.setCurrentIndex(sourceIndex(index), command);
                if (command == (ItemSelectionModel.Clear | ItemSelectionModel.Select) ||
                    command == ItemSelectionModel.Toggle) {
                    internal.shiftIndex = root.toPersistentModelIndex(sourceIndex(index));
                }
            }

            function sourceIndex(extendedIndex) {
                var proxyIndex = root.sourceIndex(extendedIndex);
                return internalSelectionModel.mapToSource(proxyModel.mapToSource(proxyIndex));
            }

            function updateIndexes() {
                var previousIndex = extendedSelectionModel.currentIndex;
                extendedSelectionModel.currentIndex = extendedIndex(internalSelectionModel.itemSelectionModel.currentIndex);
                var currentChanged = extendedSelectionModel.currentIndex != previousIndex;

                var selection = internalSelectionModel.itemSelectionModel.selection;
                var extendedIndexes = getExtendedIndexes(selection);
                var selectionChanged = extendedIndexes.length != extendedSelectionModel.selectedIndexes.length;
                for (var i = 0; i < extendedSelectionModel.selectedIndexes.length && !selectionChanged; ++i) {
                    selectionChanged |= extendedIndexes[i] != extendedSelectionModel.selectedIndexes[i];
                }
                extendedSelectionModel.selectedIndexes = extendedIndexes;
                extendedSelectionModel.hasSelection = extendedIndexes.length > 0;

                extendedSelectionModel.shiftIndex = extendedIndex(root.toModelIndex(internal.shiftIndex));
                if (extendedSelectionModel.shiftIndex.row < 0) {
                    extendedSelectionModel.shiftIndex = extendedModel.index(0, 0);
                }

                if (typeof internalSelectionModel.itemSelectionModel.supportMultiViewSelect == 'undefined' ||
                    internalSelectionModel.itemSelectionModel.supportMultiViewSelect == false) {
                    setCurrentIndex(extendedSelectionModel.currentIndex, ItemSelectionModel.NoUpdate);
                    select(extendedSelectionModel.selectedIndexes, ItemSelectionModel.Clear | ItemSelectionModel.Select)
                }
                return {
                    currentChanged: currentChanged,
                    selectionChanged: selectionChanged
                }
            }
        }

        Component.onCompleted: {
            updateIndexes();
        }

        Connections {
            target: internalSelectionModel.itemSelectionModel
            onModelChanged: {
                extendedSelectionModel.reset();
            }
        }

        Connections {
            target: internalSelectionModel.itemSelectionModel.model
            onModelReset: {
                extendedSelectionModel.reset();
            }
        }

        Connections {
            target: extendedModel
            onModelReset: {
                internalSelectionModel.itemSelectionModel.clear();
                extendedSelectionModel.updateIndexes();
            }
            onLayoutChanged: {
                extendedSelectionModel.updateIndexes();
            }
            onRowsRemoved: {
                extendedSelectionModel.updateIndexes();
            }
            onRowsInserted: {
                extendedSelectionModel.updateIndexes();
            }
            onColumnsRemoved: {
                extendedSelectionModel.updateIndexes();
            }
            onColumnsInserted: {
                extendedSelectionModel.updateIndexes();
            }
        }

        function clear() {
            internalSelectionModel.itemSelectionModel.clear();
            updateIndexes();
        }

        function clearCurrentIndex() {
            internalSelectionModel.itemSelectionModel.clearCurrentIndex();
            updateIndexes();
        }

        function clearSelection() {
            internalSelectionModel.itemSelectionModel.clearSelection()
            updateIndexes();
        }

        function isSelected(index) {
            if (typeof selectedIndexes == 'undefined') {
                return false;
            }
            return selectedIndexes.indexOf(index) != -1;
        }

        function reset() {
            internalSelectionModel.itemSelectionModel.reset();
            internal.shiftIndex = null;
            internal.updateIndexes();
        }

        function select(selection, command) {
            var indexes = root.toModelIndexList(selection);
            internal.select(indexes, command);
            updateIndexes();
        }

        function setCurrentIndex(index, command) {
            internal.setCurrentIndex(index, command);
            updateIndexes();
        }

        function updateIndexes() {
            var res = internal.updateIndexes();

            if (res.currentChanged) {
                root.currentChanged();
                if (!res.selectionChanged &&
                    isSelected(currentIndex)) {
                    // If the selection hasnt changed, but the current index within our selection has changed,
                    // emit the selection changed signal regardless
                    root.selectionChanged();
                }
            }
            if (res.selectionChanged) {
                root.selectionChanged();
            }

            return res;
        }
    }
}
