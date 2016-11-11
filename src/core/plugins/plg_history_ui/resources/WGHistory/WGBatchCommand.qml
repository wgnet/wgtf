import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Layouts 2.0


ColumnLayout {
    id: batchCommand
    objectName: "batchCommand"
    WGComponent { type: "WGBatchCommand" }

    property var historyItem: null
    property bool currentItem: false
    property bool applied: false
    property int columnIndex: 0

    WGCommandInstance {
        id: childCommand
        Layout.fillWidth: true
        historyItem: batchCommand.historyItem
        currentItem: batchCommand.currentItem
        applied: batchCommand.applied
        columnIndex: batchCommand.columnIndex
    }

    Column {
        id: batchChildColumn

        Repeater {
            id: batchChildList
            model: typeof historyItem === "undefined" ? null : historyItem.Children

            delegate: WGCommandInstance {
                id: batchChild
                historyItem: itemData.value
                currentItem: batchCommand.currentItem
                applied: batchCommand.applied
                columnIndex: batchCommand.columnIndex
            }
        }
    }
}

