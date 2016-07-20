import QtQuick 2.3


WGTextBox {
    id: textField
    objectName: typeof itemData.indexPath == "undefined" ? "string_component" : itemData.indexPath
    anchors.left: parent.left
    anchors.right: parent.right
    text: itemData.value
    readOnly: itemData.readOnly
    enabled: itemData.enabled
    multipleValues: itemData.multipleValues

    onEditAccepted: {
        itemData.value = text;
    }
}

