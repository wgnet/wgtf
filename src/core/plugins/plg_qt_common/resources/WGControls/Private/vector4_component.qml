import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls 2.0

/*!
 \brief A vector4 component variation of the vectorN control for reflected data
*/

WGVector4 {
    id: vector4
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "vector4_component" : itemData.indexPath
    value: itemData == null ? vector4.value : typeof itemData.value == "undefined" && itemData.multipleValues ? vector4.value : itemData.value
    readOnly: itemData != null && itemData.readOnly || (typeof readOnlyComponent != "undefined" && readOnlyComponent)
    enabled: itemData != null && itemData.enabled
    multipleValues: itemData != null && typeof itemData.value == "undefined" && itemData.multipleValues

    onValueChanged: {
        if(itemData == null)
            return;
        if(!itemData.multipleValues) {
            itemData.value = vector4.value;
        } else {
            beginUndoFrame();
            itemData.value = vector4.value;
            endUndoFrame();
        }
    }
}
