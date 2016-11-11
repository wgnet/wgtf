import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Layouts 2.0


WGFileSelectBox {
    id: fileComponent
    objectName: typeof itemData.indexPath == "undefined" ? "file_component" : itemData.indexPath

    property Component assetDlg : WGAssetBrowserDialog {
		model: itemData.assetModel
	}
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
    enabled: itemData.enabled && !itemData.readOnly
    multipleValues: itemData.multipleValues

    dialog: itemData.urlIsAssetBrowser ? assetDlg: fileDlg

    nameFilters: {
        var filters = itemData.urlDialogNameFilters;
        var filterArray = filters.toString().split('|');
        return filterArray;
    }

    selectedNameFilter: itemData.urlDialogSelectedNameFilter
}
