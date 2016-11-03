import QtQuick 2.5
import WGControls 1.0

WGPushButton{
    id: button
    objectName: typeof itemData.indexPath == "undefined" ? "action_component" : itemData.indexPath
    anchors.left: parent.left
    enabled: itemData.enabled && !itemData.readOnly
    text: actionObj != null ? actionObj.actionName : itemData.display
    
    property var actionObj: itemData.object.getMetaObject(itemData.name, "Action");
    property var propertyChangedObj: itemData.object.getMetaObject(itemData.name, "OnPropertyChanged");

    onClicked: {
        actionObj.execute(itemData.object);
        // Since we aren't setting the value we need invoke the propertyChanged callback
        if(propertyChangedObj != null)
        {
            propertyChangedObj.onPropertyChanged(itemData.object);
        }
    }
}
