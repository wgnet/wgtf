import QtQuick 2.3
import QtQuick.Controls 1.2

WGNumberBox {
    id: numberBox
    objectName: typeof itemData.indexPath == "undefined" ? "number_component" : itemData.indexPath
    anchors.left: parent.left

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

