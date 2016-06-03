import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 1.0


// Component for dislaying command instances
WGExpandingRowLayout {
    id: commandInstance

    // -- Begin Interface
    property variant displayObject: null
    property bool isCurrentItem: false
    property bool isApplied: false
    property int columnIndex: 0
    // -- End Interface

    Layout.preferredHeight: Math.max( defaultSpacing.minimumRowHeight, childrenRect.height )

    WGHistoryImage {
        id: commandImage
        objectName: "commandImage_" + commandName.text
        type: parent.displayObject ? (parent.displayObject.Type ? parent.displayObject.Type : "") : ""
        isApplied: parent.isApplied
        visible: (parent.columnIndex == 0)
    }


    WGHistoryText {
        id: commandName
        objectName: "commandName_" + text
        text: typeof parent.displayObject.Name != 'undefined' ?
            parent.displayObject.Name.toString() :
            "Unknown"
        isCurrentItem: parent.isCurrentItem
        isApplied: parent.isApplied
    }


    WGHistoryText {
        id: commandPreValue
        objectName: "commandPreValue_" + text
        text: (typeof parent.displayObject.PreValue != 'undefined') ?
            parent.displayObject.PreValue.toString() :
            ""
        isCurrentItem: parent.isCurrentItem
        isApplied: parent.isApplied
        visible: (typeof parent.displayObject.PreValue != 'undefined')
    }


    WGHistoryText {
        id: commandPostValue
        objectName: "commandPostValue_" + text
        text: (typeof parent.displayObject.PostValue != 'undefined') ?
            parent.displayObject.PostValue.toString() :
            ""
        isCurrentItem: parent.isCurrentItem
        isApplied: parent.isApplied
        visible: (typeof parent.displayObject.PostValue != 'undefined')
    }
}

