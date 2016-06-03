import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

/*!
 \brief Base class matches WGTreeViewBase's structure.
*/
ListView {
    id: listViewBase

    property var view
    headerPositioning: ListView.OverlayHeader
    footerPositioning: ListView.OverlayFooter
    contentWidth: contentItem.childrenRect.width 

    /*! Stores which item is currently in focus by the keyboard.
        Often this will correspond to the selected item, but not always.
        E.g. pressing ctrl+up will move the current index, but not the selected index.
        The default value is the same as the selection (modelIndex).
        To be initialized by the parent.
    */
    property var keyboardHighlightModelIndex: null

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
        columnDelegates: view.columnDelegates
        columnSequence: view.columnSequence
        columnWidths: view.columnWidths
        columnSpacing: view.columnSpacing
        isSelected: view.selectionModel.isSelected(modelIndex)
        isKeyboardHighlight: (keyboardHighlightModelIndex === modelIndex)

        Connections {
            target: view.selectionModel
            onSelectionChanged: {
                itemRow.isSelected = view.selectionModel.isSelected(modelIndex)
            }
        }

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

        onImplicitColumnWidthsChanged: {
            var viewWidths = view.implicitColumnWidths;

            while (viewWidths.length < implicitColumnWidths.length)
            {
                viewWidths.push(0);
            }

            for (var i = 0; i < implicitColumnWidths.length; ++i)
            {
                viewWidths[i] = Math.max(viewWidths[i], implicitColumnWidths[i]);
            }

            view.implicitColumnWidths = viewWidths;
        }
    }
}
