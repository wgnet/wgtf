import QtQuick 2.5
import QtQuick.Layouts 1.3

/*!
 \ingroup wgcontrols
 \brief Two tone vertical or horizontal separator

\code{.js}
WGSeparator {
    vertical: true
}
\endcode
*/

Rectangle {
    id: separatorFrame
    objectName: "WGSeparator"
    WGComponent { type: "WGSeparator" }

    /*! This property toggles between a vertical and horizontal control
        The default value is \c false
    */
    property bool vertical: false

    implicitHeight: vertical ? defaultSpacing.standardMargin : defaultSpacing.separatorWidth
    implicitWidth: vertical ? defaultSpacing.separatorWidth : defaultSpacing.standardMargin

    width: vertical ? defaultSpacing.separatorWidth : undefined
    height: vertical ? undefined : defaultSpacing.separatorWidth

    Layout.fillWidth: vertical ? false : true
    Layout.fillHeight: vertical ? true : false

    Layout.maximumWidth: vertical ? defaultSpacing.separatorWidth : Number.POSITIVE_INFINITY
    Layout.maximumHeight: vertical ? Number.POSITIVE_INFINITY : defaultSpacing.separatorWidth

    color: "transparent"

    Rectangle {
        width: vertical ? defaultSpacing.separatorWidth / 2 : parent.width
        height: vertical ? parent.height : defaultSpacing.separatorWidth / 2
        color: palette.darkerShade
        anchors.left: parent.left
        anchors.top: parent.top
    }

    Rectangle {
        width: vertical ? defaultSpacing.separatorWidth / 2 : parent.width
        height: vertical ? parent.height : defaultSpacing.separatorWidth / 2
        color: palette.lighterShade
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    /*! Deprecated */
    property alias vertical_: separatorFrame.vertical
}
