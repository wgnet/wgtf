import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls.Private 1.0

/*!
 \ingroup wgcontrols
 \brief Layout of expanding controls. Needs a width or Left/Right Anchors to work properyly.

Example:
\code{.js}
WGExpandingRowLayout {
    WGNumberBox {
        width: 120
        value: 25
        minimumValue: 0
        maximumValue: 100
    }

    WGPerformanceBar {
        id: perfBar3
        showValue: true
        ranges:[10,25,100]
    }
}
\endcode
*/

RowLayout {
    id: expandingrowlayout
    objectName: "WGExpandingRowLayout"
    WGComponent { type: "WGExpandingRowLayout" }

    /*! This property defines the row height.
        The default value is defined by defaultSpacing.minimumRowHeight
    */
    property int rowHeight: defaultSpacing.minimumRowHeight

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    spacing: defaultSpacing.rowSpacing

    /*! Deprecated */
    property alias label_: expandingrowlayout.label

    /*! Deprecated */
    property alias rowHeight_: expandingrowlayout.rowHeight
}
