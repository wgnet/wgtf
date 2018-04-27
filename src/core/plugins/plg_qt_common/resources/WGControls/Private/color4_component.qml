import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Layouts 2.0
import WGControls.Global 2.0
import WGControls.Color 2.0

WGColor4Picker{
	id: color4_component
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "color4_component" :  itemData.indexPath

    enabled: itemData != null && itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
	hasMultipleValues: itemData != null && (typeof itemData.value == "undefined") && itemData.multipleValues
	isReadOnly: itemData == null || itemData.readOnly

	color: {
        if (itemData == null || (itemData.value == "undefined" && itemData.multipleValues))
        {
            return Qt.vector4d(0, 0, 0, 1)
        }
        else if (showAlpha)
        {
            return Qt.vector4d(itemData.value.x, itemData.value.y, itemData.value.z, itemData.value.w)
        }
        else
        {
            return Qt.vector4d(itemData.value.x, itemData.value.y, itemData.value.z, 1)
        }
    }

	onColorChanged:
    {
        if(itemData == null)
            return;
        if (itemData.multipleValues)
        {
            beginUndoFrame();
        }
        if (showAlpha)
        {
            itemData.value = Qt.vector4d(color4_component.color.x, color4_component.color.y, color4_component.color.z, color4_component.color.w)
        }
        else
        {
            itemData.value = Qt.vector4d(color4_component.color.x, color4_component.color.y, color4_component.color.z, 1)
        }
        if (itemData.multipleValues)
        {
            endUndoFrame();
        }
    }

}