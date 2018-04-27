import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls 2.0

/*!
 \brief A vector2 variation of the vectorN control for reflected data
*/

WGVector2 {
    id: vector2
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "vector2_component" : itemData.indexPath
    value: itemData == null ? vector2.value : typeof itemData.value == "undefined" && itemData.multipleValues ? vector2.value : itemData.value
    readOnly: itemData != null && itemData.readOnly || (typeof readOnlyComponent != "undefined" && readOnlyComponent)
    enabled: itemData != null && itemData.enabled
    multipleValues: itemData != null && typeof itemData.value == "undefined" && itemData.multipleValues

    onValueChanged: {
        if(itemData == null)
            return;
        if(!itemData.multipleValues) {
            itemData.value = vector2.value;
        } else {
            beginUndoFrame();
            itemData.value = vector2.value;
            endUndoFrame();
        }
    }
}
