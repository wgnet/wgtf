import QtQuick 2.3


WGTextBox {
    id: textField
    objectName:  itemData != null ? itemData.indexPath : "string_component"
    anchors.left: parent.left
    anchors.right: parent.right
    text: itemData.value

    onEditAccepted: {
        itemData.value = text;
    }
}

