import QtQuick 2.3
import QtQuick.Layouts 1.1
import WGControls 1.0

/*!
 \brief A vector2 variation of the vectorN control for reflected data
*/

WGVector2 {
    id: vector2
    objectName: typeof itemData.indexPath == "undefined" ? "vector2_component" : itemData.indexPath
    value: itemData.value
    readOnly: itemData.readOnly
    enabled: itemData.enabled
    multipleValues: itemData.multipleValues

	onValueChanged: {
		itemData.value = vector2.value;
	}
}
