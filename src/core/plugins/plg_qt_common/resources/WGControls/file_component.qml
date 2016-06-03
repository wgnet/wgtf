import QtQuick 2.3
import QtQuick.Layouts 1.1


WGFileSelectBox {
    id: fileComponent
    objectName:  itemData != null ? itemData.indexPath : "file_component"

    //TODO: use WGAssetBrowserDialog when metadata says it's asset browser
    property var assetDlg : WGNativeFileDialog{}
    property var fileDlg : WGNativeFileDialog {}

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
