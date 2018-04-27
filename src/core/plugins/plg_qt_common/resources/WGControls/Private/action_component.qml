import QtQuick 2.5
import WGControls 2.0

WGPushButton{
    id: button
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "action_component" : itemData.indexPath
    enabled: itemData != null &&
        (typeof itemData.enabled == "undefined" || itemData.enabled) &&
        (typeof itemData.readOnly == "undefined" || !itemData.readOnly) &&
        (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
    text: actionObj != null ? actionObj.actionName : itemData.display

    property var actionObj: itemData != null ? itemData.object.getMetaObject(itemData.path, "Action") : undefined;

    onClicked: {
        actionObj.execute(itemData.object, itemData.path);
    }
}
