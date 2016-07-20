.pragma library
.import QtQml.Models 2.2 as QtQmlModels

/*! \file Common functions for WGListView, WGTreeView and WGTableView
 */

/*! Update selection when the mouse is clicked or keyboard arrows pressed.
 */
function updateSelection(event, selectionModel, extension, rowIndex) {
    var selection;
    if ((event.modifiers & Qt.ShiftModifier) && (event.modifiers & Qt.ControlModifier)) {
        selection = extension.itemSelection(selectionModel.shiftIndex, rowIndex);
        selectionModel.select(selection, QtQmlModels.ItemSelectionModel.Select);
        selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }
    else if (event.modifiers & Qt.ShiftModifier) {
        selection = extension.itemSelection(selectionModel.shiftIndex, rowIndex)
        selectionModel.select(selection,
            QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
        selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }
    else if (event.modifiers & Qt.ControlModifier) {
        selectionModel.setCurrentIndex(rowIndex, typeof(event.key) !== "undefined" ?
            QtQmlModels.ItemSelectionModel.NoUpdate : QtQmlModels.ItemSelectionModel.Toggle);
        selectionModel.shiftIndex = rowIndex;
    }
    else {
        selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
        selectionModel.shiftIndex = rowIndex;
    }
}
