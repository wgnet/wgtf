import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Layouts 2.0


WGExpandingRowLayout {
    id: commandInstance
    WGComponent { type: "WGCommandInstance" }
    Layout.preferredHeight: Math.max(defaultSpacing.minimumRowHeight, childrenRect.height)

    property var historyItem: null
    property bool currentItem: false
    property bool applied: false
    property int columnIndex: 0

    WGHistoryImage {
        id: commandImage
        objectName: "commandImage_" + commandName.text
        type: internal.historyItemTypeValid ? historyItem.Type : ""
        applied: commandInstance.applied
        visible: commandInstance.columnIndex === 0
    }
    
    WGHistoryText {
        id: commandName
        objectName: "commandName_" + text
        text: internal.historyItemNameValid ? historyItem.Name.toString() : "Unknown"
        currentItem: commandInstance.currentItem
        applied: commandInstance.applied
    }
    
    WGHistoryText {
        id: commandPreValue
        objectName: "commandPreValue_" + text
        text: internal.historyItemPreValid ? historyItem.PreValue.toString() : ""
        currentItem: commandInstance.currentItem
        applied: commandInstance.applied
        visible: internal.historyItemPreValid
    }
    
    WGHistoryText {
        id: commandPostValue
        objectName: "commandPostValue_" + text
        text: internal.historyItemPostValid ? historyItem.PostValue.toString() : ""
        currentItem: commandInstance.currentItem
        applied: commandInstance.applied
        visible: internal.historyItemPostValid
    }

    QtObject {
        id: internal
        property bool historyItemValid: typeof historyItem !== "undefined"
        property bool historyItemTypeValid: historyItemValid ? typeof historyItem.Type !== "undefined" : false
        property bool historyItemNameValid: historyItemValid ? typeof historyItem.Name !== "undefined" : false
        property bool historyItemPreValid: historyItemValid ? typeof historyItem.PreValue !== "undefined" : false
        property bool historyItemPostValid: historyItemValid ? typeof historyItem.PostValue !== "undefined" : false
    }
}

