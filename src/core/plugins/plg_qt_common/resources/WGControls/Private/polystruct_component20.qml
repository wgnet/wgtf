import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0

WGDropDownBox {
    id: combobox
    objectName: typeof itemData.indexPath == "undefined" ? "polystruct_component" : itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
    multipleValues: itemData.multipleValues

    property var polyModel: typeof itemData.definitionModel != "undefined" ? itemData.definitionModel : null
	property var definition: typeof itemData.definition != "undefined" ? itemData.definition : null

    model: polyModel
    textRole: "display"

	Component.onCompleted: {
		updateCurrentIndex();
		currentIndexChanged.connect(updateDefinition)
	}

	onDefinitionChanged: {
		updateCurrentIndex();
	}

    function updateDefinition() {
        if (currentIndex < 0) {
            return;
        }
        var item = polyModel.item(currentIndex);
		if(!multipleValues) {
			itemData.definition = item.value;
		} else {
			beginUndoFrame();
			itemData.definition = item.value;
			endUndoFrame();
		}
    }

	function updateCurrentIndex() {
		var count = (polyModel != null) ? polyModel.count() : 0;
		var newIndex = -1;
        for (var i = 0; i < count; ++i) {
            var item = polyModel.item(i);
            if (definition == item.value)
                newIndex = i;
        }
        if (currentIndex != newIndex) {
			currentIndex = newIndex;
		}
	}
}
