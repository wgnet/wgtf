import QtQuick 2.1
import QtQuick.Layouts 1.1
import WGControls 1.0

Slot
{
    id : outputSlot
    isInput : false

    WGSpinBox
    {
        Layout.preferredWidth: 50
        Layout.preferredHeight: defaultSpacing.minimumRowHeight
        value: 25
        minimumValue: 0
        maximumValue: 100
    }
}
