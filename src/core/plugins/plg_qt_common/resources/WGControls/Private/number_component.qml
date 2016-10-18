import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0

WGNumberBox {
    id: numberBox
    objectName: typeof itemData.indexPath == "undefined" ? "number_component" : itemData.indexPath

    number: itemData.value
    minimumValue: itemData.minValue
    maximumValue: itemData.maxValue
    stepSize: itemData.stepSize
    decimals: itemData.decimals
    readOnly: itemData.readOnly
    enabled: itemData.enabled
    multipleValues: itemData.multipleValues

    onNumberChanged: {
        itemData.value = numberBox.number;
    }
}

