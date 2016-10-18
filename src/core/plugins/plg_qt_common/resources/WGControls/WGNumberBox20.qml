import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
 \brief Text entry intended for numbers.
 Single clicked increment/decrement via stepSize property.
 Click and drag increment/decrement based on vertical linear mouse distance moved.
 Multiplier on speed of number change via keyboard toggle (Ctrl) whilst dragging.
 Releasing Ctrl drops number change rate back to default.
 User can drag up (increment) and then down (decrement) within a single click and hold event allowing for correction of overshoot.
 MouseWheel up/down will increment/decrement based on stepSize property.
 Control must be active (selected) before MouseWheel events are enabled.
 Left/Middle clicked in the Blue area will cause the text to be selected so that any keyboard entry will wipe the existing data.
 Double left click inserts a cursor at the location of the double click allowing for modification of the existing data.
 Right click in the Blue area will cause the text to be selected and bring up a context menu.
 Right clicking the up and down arrows will set the value to minimumValue.

Example:
\code{.js}
WGNumberBox {
    width: 120
    value: 25
    minimumValue: 0
    maximumValue: 100
}
\endcode

*/

WGSpinBox {
    id: spinBox
    objectName: "WGNumberBox"
    WGComponent { type: "WGNumberBox20" }

    /*!
        This property contains the value represented by the control
         The default value is \c 0
    */
    property real number: 0

    onEditingFinished: {
        setValueHelper( spinBox, "number", value );
    }

    onNumberChanged: {
        value = number;
    }
}
