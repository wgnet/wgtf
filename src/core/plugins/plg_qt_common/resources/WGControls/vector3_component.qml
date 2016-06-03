import QtQuick 2.3
import QtQuick.Layouts 1.1
import WGControls 1.0

/*!
 \brief A vector3 variation of the vectorN control which is bound to reflected data
*/

WGVector3 {
    id: vector3
    objectName: itemData != null ? itemData.indexPath : "vector3_component"
    value: itemData.value

    Binding {
        target: itemData
        property: "value"
        value: vector3.value
    }
}
