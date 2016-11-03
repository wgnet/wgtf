import QtQuick 2.5

/*!
 \ingroup wgcontrols
 \brief Empty box with placeholder text.

Example:
\code{.js}
WGTextBox {
    placeholderText: "Placeholder Text"
    Layout.preferredWidth: 300
    Layout.fillHeight: true
}
\endcode
*/

Rectangle {
    objectName: "WGPlaceholder"
    WGComponent { type: "WGPlaceholder" }
    
    color: "transparent"
    border.width: defaultSpacing.standardBorderSize
    border.color: palette.lighterShade

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.standardMargin

    /*!
        This property determines the text to display in the placeholder
    */
    property alias text: placeholderText.text

    WGMultiLineText{
        id: placeholderText
        anchors.fill: parent

        font.pointSize: 12

        anchors.topMargin: parent.height/3
        anchors.bottomMargin: parent.height/3
        anchors.leftMargin: parent.width/3
        anchors.rightMargin: parent.width/3

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
