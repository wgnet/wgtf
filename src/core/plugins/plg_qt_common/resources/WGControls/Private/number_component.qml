import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0

WGNumberBox {
    property var previousValue: null

    id: numberBox
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "number_component" : itemData.indexPath

    value: itemData == null ? 0 : typeof itemData.value == "undefined" && itemData.multipleValues ? 0 : itemData.value
    minimumValue: itemData != null ? itemData.minValue : numberBox.minimumValue
    maximumValue: itemData != null ? itemData.maxValue : numberBox.maximumValue
    stepSize: itemData != null ? itemData.stepSize : numberBox.stepSize
    decimals: itemData != null ? itemData.decimals : numberBox.decimals
    readOnly: itemData != null && itemData.readOnly || (typeof readOnlyComponent != "undefined" && readOnlyComponent)
    enabled: itemData != null && itemData.enabled
    multipleValues: itemData != null && typeof itemData.value == "undefined" && itemData.multipleValues

    onEditingFinished: {
        if(previousValue == numberBox.value) {
            return;
        }

        previousValue = numberBox.value;
        if(!itemData.multipleValues) {
            itemData.value = numberBox.value;
        } else {
            beginUndoFrame();
            itemData.value = numberBox.value;
            endUndoFrame();
        }
    }

    onBatchCommandValueUpdated:
    {
        if(!itemData.multipleValues) {
            itemData.value = numberBox.value;
        } else {
            beginUndoFrame();
            itemData.value = numberBox.value;
            endUndoFrame();
        }
    }
}

