import QtQuick 2.3

/*!
 \brief Generic rounded 'recessed' frame that can get focus.
 Typically used to frame text or a number of controls.

Example:
\code{.js}
WGTextBoxFrame {
    Layout.fillWidth: true
    Layout.preferredHeight: 100
    clip: true

    Rectangle {
        color: "red"
        width: 10
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
}
\endcode
*/

Rectangle {
    id: textFrame
    objectName: "WGTextBoxFrame"
    color: palette.textBoxColor
    border.width: defaultSpacing.standardBorderSize
    border.color: palette.darkestShade
    radius: defaultSpacing.halfRadius

    // Default size to prevent invisible controls when creating UI
    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.minimumRowHeight

}
