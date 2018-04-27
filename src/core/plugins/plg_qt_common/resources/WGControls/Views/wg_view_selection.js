.pragma library
.import QtQml.Models 2.2 as QtQmlModels

/*! \file Common functions for WGListView, WGTreeView and WGTableView
 */

/*! Update selection when the mouse is clicked or keyboard arrows pressed.
 */
function updateSelection(event, selectionModel, extension, rowIndex, supportMultiSelect) {
    var selection;

    if (event !== null && supportMultiSelect) {
        if ((event.button === Qt.RightButton)) {
            if (event.modifiers & Qt.ControlModifier) {
                // Do nothing
                return;
            }
            else if (selectionModel.isSelected(rowIndex)) {
                selectionModel.setCurrentIndex(rowIndex, QtQmlModels.ItemSelectionModel.NoUpdate);
                return;
            }
            // Default behavior
        }
        else { // Left button or keyboard
            if ((event.modifiers & Qt.ShiftModifier) && (event.modifiers & Qt.ControlModifier)) {
                selection = extension.itemSelection(selectionModel.shiftIndex, rowIndex);
                selectionModel.setCurrentIndex(rowIndex, QtQmlModels.ItemSelectionModel.NoUpdate);
                selectionModel.select(selection, QtQmlModels.ItemSelectionModel.Select);
                return;
            }
            else if (event.modifiers & Qt.ShiftModifier) {
                selection = extension.itemSelection(selectionModel.shiftIndex, rowIndex)
                selectionModel.setCurrentIndex(rowIndex, QtQmlModels.ItemSelectionModel.NoUpdate);
                selectionModel.select(selection, QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
                return;
            }
            else if (event.modifiers & Qt.ControlModifier) {
                if (typeof(event.key) !== "undefined") {
                    selectionModel.setCurrentIndex(rowIndex, QtQmlModels.ItemSelectionModel.NoUpdate);
                }
                else {
                    selectionModel.setCurrentIndex(rowIndex, QtQmlModels.ItemSelectionModel.Toggle);
                }
                return;
            }
        }
    }

    // Default
    selectionModel.setCurrentIndex(rowIndex, QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
    return;
}

