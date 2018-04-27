import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import WGControls 2.0

//TODO: WGSlider needs to be modified so that it can use much larger range, negating the need for min/maximumValue calculations
//TODO: How can we warn the user that no metaData has been set. A range of 2 billion is not a usable UI.

WGSliderControl {
    id: reflectedSlider
    objectName: typeof itemData.indexPath == "undefined" ? "slider_component" : itemData.indexPath
    anchors.fill: parent
    enabled: itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == "undefined" || !readOnlyComponent)

    value: itemData.value

    //When metadata MetaMinMax has not been defined the itemData.minValue and
    //maxValue get set to -2147483648, and 2147483647 respectively.
    //This might be due to an int or double validator.

    property bool maxSliderRangeExceeded: ((itemData.maxValue - itemData.minValue + 1) > 2147483647 )

    // Slider will only allow 2147483647 values of range.
    // If your slider is using these values then you need to set metadata MetaMinMax values in your .mpp files
    minimumValue: maxSliderRangeExceeded ? itemData.value - Math.floor(maxSliderRange/2) : itemData.minValue
    maximumValue: maxSliderRangeExceeded ? itemData.value + Math.floor(maxSliderRange/2) : itemData.maxValue

    onChangeValue: {
        if(!itemData.multipleValues) {
            itemData.value = val;
        } else {
            beginUndoFrame();
            itemData.value = val;
            endUndoFrame();
        }
    }
}
