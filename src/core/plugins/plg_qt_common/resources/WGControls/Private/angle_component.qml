import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0

WGExpandingComponent {
    id: control
    objectName: typeof itemData.indexPath == "undefined" ? "angle_component" : itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly

    replace: false

    property var angleValue: itemData.value

    onAngleValueChanged: {
        itemData.value = angleValue;
    }

    mainComponent: WGNumberBox {
        id: numberBox
        objectName: typeof itemData.indexPath == "undefined" ? "angle_number_box" : itemData.indexPath + "_numberBox"

        width: defaultSpacing.minimumRowHeight * 3

        number: control.angleValue
        minimumValue: itemData.minimumValue
        maximumValue: itemData.maximumValue

        stepSize: itemData.stepSize
        decimals: itemData.decimals
        readOnly: itemData.readOnly
        enabled: itemData.enabled

        suffix: "°"

        multipleValues: itemData.multipleValues

        onValueChanged: {
            setValueHelper( control, "angleValue", number );
            angleDial.value = control.angleValue
        }

        onEditingFinished: {
            setValueHelper( control, "angleValue", number );
            angleDial.value = control.angleValue
        }
    }

    expandedComponent: WGDial {
        id: angleDial
        objectName: typeof itemData.indexPath == "undefined" ? "angle_number_box" : itemData.indexPath + "_dial"

        width: defaultSpacing.minimumRowHeight * 3
        height: defaultSpacing.minimumRowHeight * 3

        minimumValue: itemData.minimumValue
        maximumValue: itemData.maximumValue

        stepSize: itemData.stepSize
        decimals: itemData.decimals

        value: control.angleValue

        onValueChanged: {
            setValueHelper( control, "angleValue", value );
            numberBox.value = angleDial.value
        }
    }
}

