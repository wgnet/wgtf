import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

/*!
 \brief Base class matches WGTreeViewBase's structure.
*/
ListView {
    id: listViewBase

	property alias currentRow: listViewBase.currentIndex

    property var view
	property bool asynchronous: false
    headerPositioning: ListView.OverlayHeader
    footerPositioning: ListView.OverlayFooter
    contentWidth: contentItem.childrenRect.width + scrollViewError

    // This workaround is needed until the standard QML ScrollView is fixed.
    readonly property var scrollViewError: view.clamp ? 0 : 1

    /*! Propogates events from children to parents.
        \param mouse the MouseEvent that triggered the signal.
        \param itemIndex index of items inside the WGItemRow.
        \param rowIndex index of the WGItemRow.
     */
    signal itemPressed(var mouse, var itemIndex, var rowIndex)
    signal itemClicked(var mouse, var itemIndex, var rowIndex)
    signal itemDoubleClicked(var mouse, var itemIndex, var rowIndex)

    header: view.header
    footer: view.footer

    delegate: WGItemRow {
        id: itemRow
		view: listViewBase.view
		asynchronous:  listViewBase.asynchronous
        /*! Pass events up to listViewBase.
            \param mouse passed as an argument by WGItemRow.
            \param itemIndex passed as an argument by WGItemRow.
            \param rowIndex comes from C++.
                   Call to modelIndex() automatically looks up the
                   "C++ model index" from the row.
                   Column is always 0 in WGListView.
                   Index of the WGItemRow.
         */
        onItemPressed: listViewBase.itemPressed(mouse, itemIndex, modelIndex)
        onItemClicked: listViewBase.itemClicked(mouse, itemIndex, modelIndex)
        onItemDoubleClicked: listViewBase.itemDoubleClicked(mouse, itemIndex, modelIndex)
    }
}
