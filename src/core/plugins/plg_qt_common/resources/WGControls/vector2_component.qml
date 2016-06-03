import QtQuick 2.3
import QtQuick.Layouts 1.1
import WGControls 1.0

/*!
 \brief A vector2 variation of the vectorN control for reflected data
*/

WGVector2 {
    id: vector2
    objectName: itemData != null ? itemData.indexPath : "vector2_component"
    value: itemData.value

    Binding {
        target: itemData
        property: "value"
        value: vector2.value
    }
}
