import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

WGItemView {
    id: root

    property var view: parent
    property var selectionModel: itemSelectionModel

    property Component columnDelegate: Text {
        id: defaultColumnDelegate
        objectName: "defaultColumnDelegate_" + text
        text: itemData.display
        color: palette.textColor
    }
    property var columnDelegates: []
    property var columnSequence: []
    property real columnWidth: 0
    property var columnWidths: []
    property var implicitColumnWidths: []
    property real columnSpacing: 0

    property var headerDelegates: []
    property var footerDelegates: []
    property Component headerDelegate: null
    property Component footerDelegate: null
    property Component header: null
    property Component footer: null

    property var __sortIndicators: []
    property Component sortIndicator: Item {
        id: indicator
        height: sortArrowImage.height
        Image {
            id: sortArrowImage
            anchors.centerIn: parent
            source: "icons/sort_blank_10x10.png"
        }
    }

    property Component headerComponent: WGHeaderRow {
        z:2
        columnDelegates: root.headerDelegates
        columnSequence: root.columnSequence
        columnWidths: root.columnWidths
        columnSpacing: root.columnSpacing
        headerData: root.headerData
        sortIndicators: root.__sortIndicators
    }

    property Component footerComponent: WGHeaderRow {
        z:2
        columnDelegates: root.footerDelegates
        columnSequence: root.columnSequence
        columnWidths: root.columnWidths
        columnSpacing: root.columnSpacing
        headerData: root.headerData
    }

    property var commonExtensions: [columnExtension, imageExtension]

    // Pass property up to parent
    property alias currentIndex: itemSelectionModel.currentIndex

    // Pass signals up to parent
    signal currentChanged(var current, var previous)
    signal selectionChanged(var selected, var deselected)

    extensions: commonExtensions

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
        var tmp = columnDelegates;
        while (tmp.length < columnCount()) {
            tmp.push(columnDelegate);
        }
        columnDelegates = tmp;

        tmp = headerDelegates
        if(tmp.length > 0)
        {
            while (tmp.length < columnCount()) {
                tmp.push(headerDelegate);
                __sortIndicators.push(sortIndicator);
            }
        }
        if((tmp.length == 0) && (headerDelegate != null))
        {
            while (tmp.length < columnCount()) {
                tmp.push(headerDelegate);
                __sortIndicators.push(sortIndicator);
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

        columnWidths = tmp;
    }

    WGColumnsFrame {
        id: columnsFrame
        x: root.view.contentItem.x + root.view.originX
        y: 0
        height: root.view.height
        columnWidths: root.view.columnWidths
        columnSpacing: root.view.columnSpacing
        availableWidth: root.view.width - Math.max(contentItem.x, 0)

        Component.onCompleted: {
            root.view.columnWidths = Qt.binding( function() { return columnsFrame.columnWidths } );
        }
    }

    ItemSelectionModel {
        id: itemSelectionModel
        model: extendedModel

        // Pass signals up to parent
        onCurrentChanged: root.currentChanged(current, previous);
        onSelectionChanged: root.selectionChanged(selected, deselected)
    }
}
