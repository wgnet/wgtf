import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0
import "wg_view_selection.js" as WGViewSelection

WGItemView {
    id: root
	
	model: WGSortFilterProxy {
		id: sortFilterProxy
		sortFilter: QtObject {
			function lessThan(left, right) {
				return columnSorters[sortFilterProxy.sortColumn](left, right);
			}
		}
	}
	property alias sourceModel: sortFilterProxy.sourceModel

    property var view: parent
    property var selectionModel: itemSelectionModel
	property var style: WGItemViewStyle {}

	property var columnRole: "display"
	property var columnRoles: []
    property Component columnDelegate: Text {
        id: defaultColumnDelegate
        objectName: "defaultColumnDelegate_" + text
        text: itemValue !== undefined ? itemValue : ""
        color: palette.textColor
    }
    property var columnDelegates: []
    property var columnSequence: []
    property real columnWidth: 100
    property var columnWidths: []
	QtObject {
		id: internal
		property var columnWidths_: []
	}

	property var columnSorter: function(left, right) {
		return left[columnRoles[sortFilterProxy.sortColumn]] < right[columnRoles[sortFilterProxy.sortColumn]];
	}
	property var columnSorters: []
	
    property var implicitColumnWidths: []
    property real columnSpacing: 0
    property bool clamp: false

    property var headerDelegates: []
    property var footerDelegates: []
    property Component headerDelegate: null
    property Component footerDelegate: null
    property Component header: null
    property Component footer: null

    property Component headerComponent: WGHeaderRow {
        z:2
		view: root
    }

    property Component footerComponent: WGFooterRow {
        z:2
		view: root
    }

	property var viewExtension: null
    property var commonExtensions: [viewExtension, columnExtension, imageExtension]

    // Pass signals up to parent
    signal currentChanged(var current, var previous)
    signal selectionChanged(var selected, var deselected)

    extensions: view.extensions.concat(commonExtensions)

    function rowCount() {
        return extendedModel.rowCount();
    }

    function columnCount() {
        var count = columnSequence.length;
        if (count == 0) {
            count = extendedModel.columnCount(null /* parent */);
        }
        return count;
    }

	function movePrevious(event) {
		var newIndex = viewExtension.getPreviousIndex(itemSelectionModel.currentIndex);
        WGViewSelection.updateSelection(event, itemSelectionModel, viewExtension, newIndex);
	}

	function moveNext(event) {
		var newIndex = viewExtension.getNextIndex(itemSelectionModel.currentIndex);
        WGViewSelection.updateSelection(event, itemSelectionModel, viewExtension, newIndex);
	}

	function moveBackwards(event) {
		var newIndex = viewExtension.getBackwardIndex(itemSelectionModel.currentIndex);
        WGViewSelection.updateSelection(event, itemSelectionModel, viewExtension, newIndex);
	}

	function moveForwards(event) {
		var newIndex = viewExtension.getForwardIndex(itemSelectionModel.currentIndex);
        WGViewSelection.updateSelection(event, itemSelectionModel, viewExtension, newIndex);
	}

	function select(mouse, index) {
		WGViewSelection.updateSelection(mouse, itemSelectionModel, viewExtension, index)
	}

    ColumnExtension {
        id: columnExtension
    }

    ImageExtension {
        id: imageExtension
    }

    /*! Ensure the columnDelegates and columnWidths lists are the same length
        as the number of columns that actually loaded into the list.
        \see WGItemRow
     */
    Component.onCompleted: {
		var tmp = columnRoles;
        while (tmp.length < columnCount()) {
            tmp.push(columnRole);
        }
        columnRoles = tmp

        tmp = columnDelegates;
        while (tmp.length < columnCount()) {
            tmp.push(columnDelegate);
        }
        columnDelegates = tmp;

        tmp = headerDelegates
        if(tmp.length > 0)
        {
            while (tmp.length < columnCount()) {
                tmp.push(headerDelegate);
            }
        }
        if((tmp.length == 0) && (headerDelegate != null))
        {
            while (tmp.length < columnCount()) {
                tmp.push(headerDelegate);
            }
        }
        headerDelegates = tmp;
        if(headerDelegates.length > 0)
        {
             header = headerComponent;
        }

        tmp = footerDelegates;
        if(tmp.length > 0)
        {
            while (tmp.length < columnCount()) {
                tmp.push(footerDelegate);
            }
        }
        if((tmp.length == 0) && (footerDelegate != null))
        {
            while (tmp.length < columnCount()) {
                tmp.push(footerDelegate);
            }
        }
        footerDelegates = tmp;
        if(footerDelegates.length > 0)
        {
             footer = footerComponent;
        }

        tmp = columnWidths;
        while (tmp.length < columnCount()) {
            tmp.push(Math.max(columnWidth, 1));
        }
        internal.columnWidths_ = tmp;

		tmp = columnSorters;
		while (tmp.length < columnCount()) {
            tmp.push(columnSorter);
        }
		columnSorters = tmp;
    }

    WGColumnsFrame {
        id: columnsFrame
        x: root.view.contentItem.x + root.view.originX
        y: 0
        height: root.view.height
        columnWidths: internal.columnWidths_
        columnSpacing: root.view.columnSpacing
        availableWidth: root.view.width - Math.max(contentItem.x, 0)
        implicitColumnWidths: root.implicitColumnWidths
        clamp: root.clamp
		handleDelegate: style.columnHandle

        Component.onCompleted: {
            root.view.columnWidths = Qt.binding( function() { return columnsFrame.columnWidths } );
        }
    }

    ItemSelectionModel {
        id: itemSelectionModel
        model: extendedModel

		property var shiftIndex
		Component.onCompleted: {
			shiftIndex = model.index(0, 0);
		}

        // Pass signals up to parent
        onCurrentChanged: root.currentChanged(current, previous)
        onSelectionChanged: root.selectionChanged(selected, deselected)
    }
}
