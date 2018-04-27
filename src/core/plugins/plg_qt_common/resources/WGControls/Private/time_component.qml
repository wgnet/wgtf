import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0

WGTimeBox {
    id: timeBox
    objectName: typeof itemData.indexPath == "undefined" ? "time_component" : itemData.indexPath

    time: itemData.value
    minimumValue: itemData.minValue
    maximumValue: itemData.maxValue
    readOnly: itemData.readOnly || (typeof readOnlyComponent != "undefined" && readOnlyComponent)
    enabled: itemData.enabled
    multipleValues: itemData.multipleValues

    onTimeChanged: {
		if(!multipleValues) {
			itemData.value = timeBox.time;
		} else {
			beginUndoFrame();
			itemData.value = timeBox.time;
			endUndoFrame();
		}
    }
}

