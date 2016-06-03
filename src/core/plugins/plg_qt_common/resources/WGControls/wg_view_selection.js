.pragma library
.import QtQml.Models 2.2 as QtQmlModels

/*! \file Common functions for WGListView and WGTreeView.
 */

/*! Update selection when the keyboard highlight moves.
 */
function updateKeyboardSelection(event, newIndex, view, extension) {
    // When Shift is pressed, the selected area increases with the keyboard highlight
    if (event.modifiers & Qt.ShiftModifier) {
        // Add new item to selection
        var selection = extension.itemSelection(view.selectionModel.currentIndex, newIndex);
        view.selectionModel.select(selection,
            QtQmlModels.ItemSelectionModel.Select);

        // Move keyboard highlight to the item selected last
        view.selectionModel.setCurrentIndex(newIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }

    // When Ctrl is pressed, move keyboard highlight, but do not modify selection
    else if (event.modifiers & Qt.ControlModifier) {
        view.selectionModel.setCurrentIndex(newIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }

    // When no modifiers are pressed, selection moves with the keyboard highlight
    else {
        view.selectionModel.setCurrentIndex(newIndex,
            QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
    }

}


/*! Update selection when the mouse is clicked.
 */
function itemPressed(mouse, view, extension, rowIndex) {
    var selection;
    if ((mouse.modifiers & Qt.ShiftModifier) && (mouse.modifiers & Qt.ControlModifier)) {
        selection = extension.itemSelection(view.selectionModel.currentIndex, rowIndex);
        view.selectionModel.select(selection, ItemSelectionModel.Select);
        view.selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }
    else if (mouse.modifiers & Qt.ShiftModifier) {
        selection = extension.itemSelection(view.selectionModel.currentIndex, rowIndex)
        view.selectionModel.select(selection,
            QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
        view.selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }
    else if (mouse.modifiers & Qt.ControlModifier) {
        view.selectionModel.setCurrentIndex(rowIndex, QtQmlModels.ItemSelectionModel.Toggle);
    }
    else {
        view.selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
    }
}
