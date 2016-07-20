import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 2.0


WGDropDownBox {
    id: combobox
    objectName: typeof itemData.indexPath == "undefined" ? "polystruct_component" : itemData.indexPath
    anchors.left: parent.left
    anchors.right: parent.right
    enabled: itemData.enabled && !itemData.readOnly
    multipleValues: itemData.multipleValues

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
