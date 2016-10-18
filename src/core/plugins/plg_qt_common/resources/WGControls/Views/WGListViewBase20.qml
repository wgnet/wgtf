import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2

import WGControls 2.0
import WGControls.Views 2.0

/** Internal WGListView specific logic, also currently used by WGTableView.
\ingroup wgcontrols */
ListView {
    id: listViewBase
    WGComponent { type: "WGListViewBase20" }
    
    headerPositioning: ListView.OverlayHeader
    footerPositioning: ListView.OverlayFooter
    contentWidth: contentItem.childrenRect.width + scrollViewError
    header: view.header
    footer: view.footer
    /** type:int Current row number.*/
    property alias currentRow: listViewBase.currentIndex
    /** Link to the common view component.*/
    property var view
    /** Switch to load asynchronously or synchronously.*/
    property bool asynchronous: false
    /** This workaround is needed until the standard QML ScrollView is fixed. It is out by one pixel.*/
    readonly property var scrollViewError: view.clamp ? 0 : 1

    /** Signals that an item received a mouse press.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemPressed(var mouse, var itemIndex, var rowIndex)
    /** Signals that this item was clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemClicked(var mouse, var itemIndex, var rowIndex)
    /** Signals that this item was double clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemDoubleClicked(var mouse, var itemIndex, var rowIndex)

    highlightRangeMode: ListView.ApplyRange

    preferredHighlightBegin: headerItem && headerPositioning == ListView.OverlayHeader ? headerItem.height: 0
    preferredHighlightEnd: footerItem && footerPositioning == ListView.OverlayFooter? listViewBase.height - footerItem.height : listViewBase.height

    delegate: WGItemRow {
        id: itemRow
        view: listViewBase.view
        asynchronous:  listViewBase.asynchronous

        Keys.forwardTo: [listViewBase]

        // Pass events up to listViewBase.
        onItemPressed: listViewBase.itemPressed(mouse, itemIndex, modelIndex)
        onItemClicked: listViewBase.itemClicked(mouse, itemIndex, modelIndex)
        onItemDoubleClicked: listViewBase.itemDoubleClicked(mouse, itemIndex, modelIndex)
    }
}
