import QtQuick 2.3
import QtQuick.Layouts 1.1
import WGControls 1.0

/*!
 \brief A vector3 variation of the vectorN control which is bound to reflected data
*/

WGVector3 {
    id: vector3
    objectName: typeof itemData.indexPath == "undefined" ? "vector3_component" : itemData.indexPath
    value: itemData.value
    readOnly: itemData.readOnly
    enabled: itemData.enabled
    multipleValues: itemData.multipleValues

    onValueChanged: {
        itemData.value = vector3.value;
    }
}
