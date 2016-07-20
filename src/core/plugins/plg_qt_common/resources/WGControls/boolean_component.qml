import QtQuick 2.3


WGCheckBox{
    id: checkbox
    objectName: typeof itemData.indexPath == "undefined" ? "boolean_component" : itemData.indexPath
    anchors.left: parent.left
    checked: itemData.value
    enabled: itemData.enabled && !itemData.readOnly
    multipleValues: itemData.multipleValues

    onCheckedChanged: {
        itemData.value = checked;
    }
}
