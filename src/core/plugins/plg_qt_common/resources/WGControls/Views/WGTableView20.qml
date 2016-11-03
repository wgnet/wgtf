import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2

import WGControls 2.0
import WGControls.Styles 2.0
import WGControls.Views 2.0

/** Specialized item view aimed at viewing models with items arranged in rows and columns.
\ingroup wgcontrols */
WGListViewBase {
    id: tableView
    objectName: "WGTableView"
    WGComponent { type: "WGTableView20" }
    
    boundsBehavior : Flickable.StopAtBounds
    contentItem.x: -originX
    contentItem.y: -originY
    clip: true
    view: itemView
    internalModel: itemView.extendedModel

    /** The style component to allow custom view appearance.*/
    property alias style: itemView.style
    /** Default role for the value used in a cell component.*/
    property alias columnRole: itemView.columnRole
    /** Specific value roles per index to expose to cell components, overriding the default columnRole.*/
    property alias columnRoles: itemView.columnRoles
    /** Default cell component used in the rows of the view body.*/
    property alias columnDelegate: itemView.columnDelegate
    /** Specific components per index for view body cells, overriding the default cell component.*/
    property alias columnDelegates: itemView.columnDelegates
    /** Alternate arrangement of model columns in the view, also allowing skipping or repeating of columns.*/
    property alias columnSequence: itemView.columnSequence
    /** Default column width if none is specified.*/
    property alias columnWidth: itemView.initialColumnWidth
    /** Specific column widths per column.*/
    property alias columnWidths: itemView.initialColumnWidths
    /** The minimum a column can be reduced to. */
    property alias minimumColumnWidth: itemView.minimumColumnWidth
    /** Size of the gap between columns.*/
    property alias columnSpacing: itemView.columnSpacing
    /** The adapted model used internally.*/
    property alias internalModel: tableView.model
    /** The data model providing the view with information to display.*/
    property alias model: itemView.model
    /** The object used for sorting operations.*/
    property alias sortObject: itemView.sortObject
    /** The object used for filtering logic.*/
    property alias filterObject: itemView.filterObject
    /** Optional specific header cell components, overriding the default header cell component.*/
    property alias headerDelegates: itemView.headerDelegates
    /** Optional specific footer cell components, overriding the default footer cell component.*/
    property alias footerDelegates: itemView.footerDelegates
    /** Default header cell component.*/
    property alias headerDelegate: itemView.headerDelegate
    /** Default footer cell component.*/
    property alias footerDelegate: itemView.footerDelegate
    /** Clamp (fix) width of the view to the containing component and adjust contents when width resized.*/
    property alias clamp: itemView.clamp
    /** A replacement for ListView's currentIndex that uses a QModelIndex from the selection model.*/
    property var currentIndex: itemView.selectionModel.currentIndex
    /** The combined common and view extensions.*/
    property var extensions: []

    onCurrentIndexChanged: {
        if (typeof( currentIndex ) == "number") {
            currentRow = currentIndex
        }
        else {
            currentRow = itemView.getRow( currentIndex );
            itemView.selectionModel.setCurrentIndex( currentIndex, ItemSelectionModel.NoUpdate );
        }
    }
    Connections {
        target: itemView.selectionModel
        onCurrentChanged: {
            if (current != previous) {
                currentIndex = current;
            }
        }
    }

    Keys.onUpPressed: {
        itemView.movePrevious(event);
    }
    Keys.onDownPressed: {
        itemView.moveNext(event);
    }
    Keys.onLeftPressed: {
        itemView.moveBackwards(event);
    }
    Keys.onRightPressed: {
        itemView.moveForwards(event);
    }

    property bool __skippedPress: false
    onItemPressed: {
        var selected = itemView.selectionModel.selectedIndexes;
        for ( var i in selected ) {
            if ( selected[i] === itemIndex ) {
                __skippedPress = true;
                return;
            }
        }

        itemView.select(mouse, itemIndex);
    }
    onItemClicked: {
        if ( __skippedPress ) {
            itemView.select(mouse, itemIndex);
        }
        __skippedPress = false;
    }

    /** Common view code. */
    WGItemViewCommon {
        id: itemView
        style: WGTableViewStyle {}
        viewExtension: tableExtension

        TableExtension {
            id: tableExtension
        }
    }
}

