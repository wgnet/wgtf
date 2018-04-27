import QtQuick 2.5
import QtQuick.Layouts 1.3
import WGControls 2.0
import WGControls.Layouts 2.0
import WGControls.Private 2.0
import WGControls.Global 2.0

WGFileSelectBox {
    id: fileComponent
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "file_component" : itemData.indexPath

    onFileChosen: {
        if(itemData == null)
            return;
        if(!multipleValues) {
            itemData.value = selectedFile;
        } else {
            beginUndoFrame();
            itemData.value = selectedFile;
            endUndoFrame();
        }
    }

    isValidUrl: text == "" || WGPath.filesExist(text)
    text: itemData == null ? fileComponent.text : (typeof itemData.value == "undefined") && multipleValues ? "Multiple Values" : itemData.value
    readOnly: itemData != null && itemData.readOnly || (typeof readOnlyComponent != "undefined" && readOnlyComponent)
    title: itemData != null ? itemData.urlDialogTitle: ""
    model: itemData == null ? null : itemData.urlIsAssetBrowser ? itemData.assetModel : null
    folder: itemData == null ? "" : itemData.urlDialogDefaultFolder
    enabled: itemData != null && itemData.enabled
    multipleValues: itemData != null && itemData.multipleValues

    nameFilters: {
        var filters = itemData != null ? itemData.urlDialogNameFilters : "";
        var filterArray = filters.toString().split('|');
        return filterArray;
    }

    selectedNameFilter: itemData == null ? "" : itemData.urlDialogSelectedNameFilter
}
