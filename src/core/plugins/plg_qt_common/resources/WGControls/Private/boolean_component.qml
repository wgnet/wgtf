import QtQuick 2.5
import WGControls 1.0


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
