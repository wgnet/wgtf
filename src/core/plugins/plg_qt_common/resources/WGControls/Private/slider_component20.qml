import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import WGControls 2.0

//TODO: WGSlider needs to be modified so that it can use much larger range, negating the need for min/maximumValue calculations
//TODO: How can we warn the user that no metaData has been set. A range of 2 billion is not a usable UI.

WGSliderControl {
    id: reflectedSlider
    objectName: typeof itemData.indexPath == "undefined" ? "slider_component" : itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
    multipleValues: (typeof itemData.value == "undefined") && itemData.multipleValues
    stepSize: itemData.stepSize
    decimals: itemData.decimals

    value: {
        if (!multipleValues && isMapped) {
            return changeRange(itemData.value, minValue, maxValue, mappedMinValue, mappedMaxValue);
        }
        return multipleValues ? 0 : itemData.value;
    }

    function setValue(v) {
        if(!multipleValues && isMapped) {
            itemData.value = changeRange(v, mappedMinValue, mappedMaxValue, minValue, maxValue);
        }
        else {
            itemData.value = v;
        }
    }

    /** Converts a value from one range of values to another */
    function changeRange(v, currentInner, currentOuter, newInner, newOuter) {
        return ((v - currentInner) * ((newOuter - newInner) /
            (currentOuter - currentInner))) + newInner;
    }

    //When metadata MetaMinMax has not been defined the itemData.minValue and
    //maxValue get set to -2147483648, and 2147483647 respectively.
    //This might be due to an int or double validator.
    property var isMapped: typeof itemData.minMappedValue != "undefined" && typeof itemData.maxMappedValue != "undefined" 
    property var mappedMinValue: typeof itemData.minMappedValue != "undefined" ? itemData.minMappedValue : 0.0
    property var mappedMaxValue: typeof itemData.maxMappedValue != "undefined" ? itemData.maxMappedValue : 0.0
    property var minValue: typeof itemData.minValue != "undefined" ? itemData.minValue : 0.0
    property var maxValue: typeof itemData.maxValue != "undefined" ? itemData.maxValue : 0.0
    property var displayMinValue: isMapped ? mappedMinValue : minValue
    property var displayMaxValue: isMapped ? mappedMaxValue : maxValue
    property bool maxSliderRangeExceeded: ((displayMaxValue - displayMinValue + 1) > 2147483647 )

    // Slider will only allow 2147483647 values of range.
    // If your slider is using these values then you need to set metadata MetaMinMax values in your .mpp files
    minimumValue: maxSliderRangeExceeded ? value - Math.floor(maxSliderRange/2) : displayMinValue
    maximumValue: maxSliderRangeExceeded ? value + Math.floor(maxSliderRange/2) : displayMaxValue

    onChangeValue: {
        if (!multipleValues) {
            if (itemData.value != val) {
                setValue(val);
            }
        }
        else {
            beginUndoFrame();
            setValue(val);
            endUndoFrame();
        }
    }
}
