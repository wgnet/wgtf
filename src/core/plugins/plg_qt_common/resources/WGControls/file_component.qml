import QtQuick 2.3
import QtQuick.Layouts 1.1


WGFileSelectBox {
    id: fileComponent
    objectName: typeof itemData.indexPath == "undefined" ? "file_component" : itemData.indexPath

    //TODO: use WGAssetBrowserDialog when metadata says it's asset browser
    property Component assetDlg : WGNativeFileDialog{}
    property Component fileDlg : WGNativeFileDialog {}

    onFileChosen: {
        itemData.value = selectedFile
    }
    onFileRejected: {
        closeDialog()
    }

    anchors.left: parent.left
    anchors.right: parent.right
    text: itemData.value
    readOnly: true
    title: itemData.urlDialogTitle
    folder: itemData.urlDialogDefaultFolder
    modality: itemData.urlDialogModality
    enabled: itemData.enabled && !itemData.readOnly
    multipleValues: itemData.multipleValues

    dialog: itemData.urlIsAssetBrowser ? assetDlg: fileDlg

    nameFilters: {
        var filters = itemData.urlDialogNameFilters;
        if(filters.toString() === "")
        {
            return [
            "All files (*)",
            "Image files (*.jpg *.png *.bmp *.dds)",
            "Model files (*.model *.primitives *.visual)",
            "Script files (*.txt *.xml *.py)",
            "Audio files (*.fsb *.fev)",
            ]
        }
        var filterArray = filters.toString().split('|');
        return filterArray;
    }

    selectedNameFilter: {
        var filters = itemData.urlDialogNameFilters;
        if(filters.toString() === "")
        {
            return "All files (*)";
        }
        return itemData.urlDialogSelectedNameFilter;
    }
}
