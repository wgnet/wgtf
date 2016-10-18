import QtQuick 2.5

import WGControls.Private 1.0

/*!
 \ingroup wgcontrols
*/
Item {
    id: scrollStyleBase
    objectName: "WGScrollBarStyle"
    WGComponent { type: "WGScrollBarStyle" }

    property Component scrollHandle: Rectangle {
        color: palette.highlightColor

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.darkHeaderColor
    }

    property Component scrollGroove: Rectangle {
        color: palette.lighterShade

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.darkestShade
    }
}
