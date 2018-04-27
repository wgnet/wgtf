import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Color 2.0

RowLayout {
    objectName: typeof itemData.indexPath == "undefined" ? "colorkelvin_component" : itemData.indexPath
    enabled: typeof itemData != 'undefined' ? itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == 'undefined' || !readOnlyComponent) : false
    property var colorObj: itemData.object.getMetaObject(itemData.path, "KelvinColor")

    // Does not open a color picker as data does not have an RGB to kelvin function
    WGColorButton {
        color: (typeof itemData.value == "undefined") ? Qt.vector4d(0,0,0,1) : Qt.vector4d(kSlider.kelvinToRgb(itemData.value).x,kSlider.kelvinToRgb(itemData.value).y,kSlider.kelvinToRgb(itemData.value).z,1)
    }

    WGKelvinSlider {
        id: kSlider
        Layout.preferredHeight: defaultSpacing.minimumRowHeight
        Layout.fillWidth: true

        multipleValues: false
        value: (typeof itemData.value == "undefined") ? 0 : itemData.value

        stepSize: 1
        decimals: 0

        minimumValue: (typeof itemData.minValue != "undefined") ? itemData.minValue : 1000
        maximumValue: (typeof itemData.maxValue != "undefined") ? itemData.maxValue : 400000

        kelvinToRgb: function(kelvin) {
            return colorObj.convert(kelvin)
        }

        onChangeValue: {
            if (itemData.value != val){
                itemData.value = val;
            }
        }
    }
}
