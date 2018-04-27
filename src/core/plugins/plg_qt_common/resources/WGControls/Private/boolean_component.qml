import QtQuick 2.5
import WGControls 2.0

WGCheckBox{
    id: checkbox
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "boolean_component" : itemData.indexPath
    checked: itemData != null && typeof itemData.value != "undefined" && !multipleValues && itemData.value
    checkedState: itemData == null ? Qt.Unchecked : ((multipleValues || typeof itemData.value == "undefined") ? Qt.PartiallyChecked : (checked ? Qt.Checked : Qt.Unchecked))
    enabled: itemData != null &&
        (typeof itemData.enabled == "undefined" || itemData.enabled) &&
        (typeof itemData.readOnly == "undefined" || !itemData.readOnly) &&
        (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
    multipleValues: itemData != null && typeof itemData.multipleValues != "undefined" && itemData.multipleValues
    property int boolState: (itemData == null || typeof itemData.value == "undefined" || multipleValues) ? 0 : (itemData.value ? 1 : -1)
    x: defaultSpacing.standardMargin

    onClicked: {
        if(itemData == null)
            return;
        if(!multipleValues) {
            itemData.value = checked;
        } else {
            beginUndoFrame();
            itemData.value = checked;
            endUndoFrame();
        }
    }

    onBoolStateChanged: {
        var val = Qt.PartiallyChecked;
        if(boolState == 1) {
            val = Qt.Checked
        } else if(boolState == -1) {
            val = Qt.Unchecked
        }
        setValueHelper( checkbox, "checkedState", val);
    }
}
