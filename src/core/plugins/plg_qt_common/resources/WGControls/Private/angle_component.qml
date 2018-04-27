import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import Qt.labs.controls 1.0 as Labs

import WGControls 2.0

RowLayout
{
    id: control
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "angle_component" : itemData.indexPath
    enabled: itemData != null &&
        (typeof itemData.enabled == "undefined" || itemData.enabled) &&
        (typeof itemData.readOnly == "undefined" || !itemData.readOnly) &&
        (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: numberBox.width

    QtObject {
        id: internal
        readonly property var radiansMultiplier: 180 / Math.PI
        readonly property bool storeRadians: metaAngle && metaAngle.storeRadians
        readonly property bool showRadians: metaAngle && metaAngle.showRadians
        readonly property bool shouldConvert: storeRadians != showRadians
        readonly property real max: itemData.maxValue
        readonly property real min: itemData.minValue
        readonly property real fullRotationValue: showRadians ? Math.PI * 2 : 360.0
        readonly property real angleValue: itemData.multipleValues ? 0.0 : limitToRange(toShownMeasure(itemData.value))
        readonly property var metaAngle: itemData != null ? itemData.object.getMetaObject(itemData.path, "Angle") : null;
        readonly property var externalValue: itemData != null ? itemData.value : 0

        onExternalValueChanged: {
            if (!angleDialPopUp.visible && !internal.modifying) {
                angleDial.value = angleValue
                numberBox.value = angleValue
            }
        }

        function toUnitOfMeasure(value, toRadians)
        {
            if (!shouldConvert)
            {
                return value;
            }

            if (toRadians)
            {
                return value / internal.radiansMultiplier;
            }

            return value * internal.radiansMultiplier;
        }

        function toShownMeasure(storedValue)
        {
            return toUnitOfMeasure(storedValue, showRadians);
        }

        function toStoreMeasure(displayValue)
        {
            return toUnitOfMeasure(displayValue, storeRadians);
        }

        function limitToRange(displayValue)
        {
            var displayMin = toShownMeasure(min, showRadians)
            var displayMax = toShownMeasure(max, showRadians)
            if (displayValue >= displayMin && displayValue <= displayMax)
                return displayValue;

            var modulo = Math.max(fullRotationValue, Math.floor(Math.max(Math.abs(displayMin), Math.abs(displayMax)) / fullRotationValue) * fullRotationValue)

            displayValue %= modulo

            while(displayValue < displayMin)
                displayValue += fullRotationValue
            while(displayValue > displayMax)
                displayValue -= fullRotationValue

            if(displayValue < displayMin)
                displayValue = displayMin

            return displayValue;
        }

        function updateValue(displayValue) {
            displayValue = limitToRange(displayValue)
            if (itemData == null)
            {
                return;
            }

            if (angleDial.value != displayValue) {
                angleDial.value = displayValue
            }

            if (numberBox.value != displayValue) {
                numberBox.value = displayValue
            }

            // If no change has occurred ignore the update
            if(angleValue.toFixed(numberBox.decimals) == displayValue.toFixed(numberBox.decimals))
                return;

            if (itemData.multipleValues) {
                beginUndoFrame();
                itemData.value = toStoreMeasure(displayValue);
                endUndoFrame();
            } else {
                itemData.value = toStoreMeasure(displayValue);
            }
        }
    }

    WGNumberBox
    {
        id: numberBox
        objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "angle_number_box" : itemData.indexPath + "_numberBox"

        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3 + defaultSpacing.doubleMargin

        minimumValue : -Math.pow(2, 53)
        maximumValue : Math.pow(2, 53)

        stepSize: itemData == null ? numberBox.stepSize : itemData.stepSize
        decimals: itemData == null ? numberBox.decimals : itemData.decimals
        readOnly: itemData == null ? numberBox.readOnly : itemData.readOnly
        enabled: itemData == null ? numberBox.enabled : itemData.enabled

        // Suffixes and prefixes currently don't work with the validator
        //suffix: "°"

        multipleValues: itemData != null && itemData.multipleValues

        onEditingFinished:
        {
            if (internal.angleValue != numberBox.value)
            {
                internal.updateValue(numberBox.value)
            }
        }
        onBatchCommandValueUpdated:
        {
            if (internal.angleValue != numberBox.value)
            {
                internal.updateValue(numberBox.value)
            }
        }
        Item {
            anchors.fill: parent
            property bool hovered: numberBox.hovered

            WGPopup {
                id: angleDialPopUp
                openPolicy: openOnHover
                padding: defaultSpacing.standardMargin
                closePolicy: Labs.Popup.OnEscape | Labs.Popup.OnPressOutsideParent

                WGDial {
                    id: angleDial
                    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "angle_number_box" : itemData.indexPath + "_dial"

                    width: defaultSpacing.minimumRowHeight * 3
                    height: defaultSpacing.minimumRowHeight * 3

                    minimumValue : numberBox.minimumValue
                    maximumValue : numberBox.maximumValue

                    // Set the zeroValue so the dial starts with zero to the right
                    zeroValue: 90

                    stepSize: itemData == null ? angleDial.stepSize : itemData.stepSize
                    decimals: itemData == null ? angleDial.decimals : itemData.decimals

                    unitString: internal.showRadians ? " rad" : "°"

                    showRotations: true
                    showValue: true

                    minRotation: 0
                    maxRotation: internal.fullRotationValue

                    loopAtBounds: minimumValue == minRotation && maximumValue == maxRotation

                    onChangeValue:
                    {
                        if (internal.angleValue != val)
                        {
                            internal.updateValue(val)
                        }
                    }
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: defaultSpacing.minimumRowHeight
    }
}
