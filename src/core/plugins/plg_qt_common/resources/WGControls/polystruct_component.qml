import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0


WGDropDownBox {
    id: combobox
    objectName:  itemData != null ? itemData.indexPath : "polystruct_component"
    anchors.left: parent.left
    anchors.right: parent.right

    Component.onCompleted: {
        currentIndex = Qt.binding( function() {
            var modelIndex = polyModel.find( itemData.definition, "value" );
            return polyModel.indexRow( modelIndex ); } )
    }

    model: polyModel
    textRole: "display"

    WGListModel {
        id: polyModel
        source: itemData.definitionModel

        ValueExtension {}
    }

    Connections {
        target: combobox
        onCurrentIndexChanged: {
            if (currentIndex < 0) {
                return;
            }
            var modelIndex = polyModel.index( currentIndex );
            itemData.definition = polyModel.data( modelIndex, "value" );
        }
    }
}
