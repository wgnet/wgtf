import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls 1.0

/*!
 \brief A vector4 component variation of the vectorN control for reflected data
*/

WGVector4 {
    id: vector4
    objectName: typeof itemData.indexPath == "undefined" ? "vector4_component" : itemData.indexPath
    value: itemData.value
    readOnly: itemData.readOnly
    enabled: itemData.enabled
    multipleValues: itemData.multipleValues

    onValueChanged: {
        itemData.value = vector4.value;
    }
}
