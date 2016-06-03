import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0


WGDropDownBox {
    id: combobox
    objectName:  itemData != null ? itemData.indexPath : "enum_component"
    anchors.left: parent.left
    anchors.right: parent.right

    Component.onCompleted: {
        currentIndex = Qt.binding( function() {
            var modelIndex = enumModel.find( itemData.value, "value" );
            return enumModel.indexRow( modelIndex ); } )
    }

    // just for the purpose of dynamically generating dropdown list when users click on the dropdownbox
    onPressedChanged: {
        if( pressed )
        {
            enumModel.source = itemData.enumModel
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
