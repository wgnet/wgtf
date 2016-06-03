import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import WGControls 1.0


// Component for dislaying batch commands
ColumnLayout {
    id: batchCommand
    objectName: "batchCommand"

    // -- Begin Interface
    property variant displayObject: null
    property bool isCurrentItem: false
    property bool isApplied: false
    property int columnIndex: 0
    // -- End Interface

    // Single command instance or batch title
    WGCommandInstance {
        id: childCommand

        Layout.fillWidth: true

        displayObject: parent.displayObject
        isCurrentItem: parent.isCurrentItem
        isApplied: parent.isApplied
        columnIndex: parent.columnIndex
    }


    // Child instances
    WGListModel {
        id: batchModel

        source: displayObject.Children

        ValueExtension {}
    }


    Column {
        id: batchChildColumn

        Repeater {
            id: batchChildList
            model: batchModel

            // TODO is displaying batches of batches possible?
            delegate: WGCommandInstance {
                id: batchChild

                property variant itemData: value

                // -- Begin Interface
                displayObject: itemData
                isCurrentItem: batchCommand.isCurrentItem
                isApplied: batchCommand.isApplied
                columnIndex: batchCommand.columnIndex
                // -- End Interface
            }
        }
    }
}

