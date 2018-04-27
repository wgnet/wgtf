import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls 2.0

/*!
 \brief A vector3 variation of the vectorN control which is bound to reflected data
*/

WGVector3 {
    id: vector3
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "vector3_component" : itemData.indexPath
    value: itemData == null ? vector3.value : typeof itemData.value == "undefined" && itemData.multipleValues ? vector3.value : itemData.value
    readOnly: itemData != null && itemData.readOnly || (typeof readOnlyComponent != "undefined" && readOnlyComponent)
    enabled: itemData != null && itemData.enabled
    multipleValues: itemData != null && typeof itemData.value == "undefined" && itemData.multipleValues

    onValueChanged: {
        if(itemData == null)
            return;
        if(!itemData.multipleValues) {
            itemData.value = vector3.value;
        } else {
            beginUndoFrame();
            itemData.value = vector3.value;
            endUndoFrame();
        }
    }
}
