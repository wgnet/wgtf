import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0

WGDropDownBox {
    id: combobox
    objectName: typeof itemData.indexPath == "undefined" ? "enum_component" : itemData.indexPath
    anchors.left: parent.left
    anchors.right: parent.right
    enabled: itemData.enabled && !itemData.readOnly
    multipleValues: itemData.multipleValues

    Component.onCompleted: {
        currentIndex = Qt.binding( function() {
            var modelIndex = enumModel.find( itemData.value, "value" );
            return enumModel.indexRow( modelIndex ); } )
    }

    // just for the purpose of dynamically generating dropdown list when users click on the dropdownbox
    onPressedChanged: {
        if( pressed && !popup.visible )
        {
            //fake undo frame to stop model change creating a history item
            beginUndoFrame()
            var oldCurrentIndex = combobox.currentIndex
            enumModel.source = itemData.enumModel
            model = []
            model = enumModel
            combobox.currentIndex = oldCurrentIndex
            abortUndoFrame()
        }
    }

    model: enumModel
    textRole: "display"

    WGListModel {
        id: enumModel
        source: itemData.enumModel

        ValueExtension {}
    }

    Connections {
        target: combobox
        onCurrentIndexChanged: {
            if (currentIndex < 0) {
                return;
            }
            var modelIndex = enumModel.index( currentIndex );
            itemData.value = enumModel.data( modelIndex, "value" );
        }
    }
}
