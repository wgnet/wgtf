import QtQuick 2.3
import QtQuick.Controls 1.2

WGNumberBox {
    id: numberBox
    objectName:  itemData != null ? itemData.indexPath : "number_component"
    anchors.left: parent.left

    number: itemData.value
    minimumValue: itemData.minValue
    maximumValue: itemData.maxValue
    stepSize: itemData.stepSize
    decimals: itemData.decimals
    readOnly: itemData.isReadOnly

    Binding {
        target: itemData
        property: "value"
        value: numberBox.number
    }

}

