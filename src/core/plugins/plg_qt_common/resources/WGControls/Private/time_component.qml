import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0

WGTimeBox {
    id: timeBox
    objectName: typeof itemData.indexPath == "undefined" ? "time_component" : itemData.indexPath

    time: itemData.value
    minimumValue: itemData.minValue
    maximumValue: itemData.maxValue
    readOnly: itemData.readOnly
    enabled: itemData.enabled
    multipleValues: itemData.multipleValues

    onTimeChanged: {
        itemData.value = timeBox.time;
    }
}

