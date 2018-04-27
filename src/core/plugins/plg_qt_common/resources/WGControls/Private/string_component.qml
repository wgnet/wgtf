import QtQuick 2.5

import WGControls 2.0


WGTextBox {
    id: textField
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "string_component" : itemData.indexPath
    text: itemData == null ? "" : (typeof itemData.value == "undefined") && multipleValues ? "Multiple Values" : itemData.value
    readOnly: itemData != null && itemData.readOnly || (typeof readOnlyComponent != "undefined" && readOnlyComponent)
    enabled: itemData != null && itemData.enabled
    multipleValues: itemData != null && itemData.multipleValues

    onEditAccepted: {
        if(itemData == null)
            return;
		if(!multipleValues) {
			itemData.value = text;
		} else {
			beginUndoFrame();
			itemData.value = text;
			endUndoFrame();
		}
    }
}

