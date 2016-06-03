import QtQuick 2.3


WGCheckBox{
    id: checkbox
    objectName: itemData != null ? itemData.indexPath : "boolean_component"
    anchors.left: parent.left
    checked: itemData.value

    Binding {
        target: itemData
        property: "value"
        value: checkbox.checked
    }
}
