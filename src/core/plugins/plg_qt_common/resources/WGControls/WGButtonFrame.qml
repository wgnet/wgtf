import QtQuick 2.3

/*!
    \brief A WG styled frame with a raised look intended for buttons.
*/


Rectangle {
    id: outerFrame
    objectName: "WGButtonFrame"

    /*! This property defines the color of the inner border
        The default value is \c palette.lighterShade
    */
    property color innerBorderColor: palette.lightShade

    /*! This property defines the color of the border
        The default value is \c palette.darkerShade
    */
    property color borderColor: palette.darkerShade

    /*! This property defines the color of a highlight overlay
        The default value is \c "transparent"
    */
    property color highlightColor: "transparent"

    color: palette.lightShade

    border.width: defaultSpacing.standardBorderSize
    border.color: borderColor

    //TODO: Changing this doesnt always give a desirable effect
    radius: defaultSpacing.standardRadius

    //inner border to make it look raised and different to a text box.
    Rectangle {
        id: innerFrame
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardBorderSize
        radius: parent.radius /2

        border.width: defaultSpacing.standardBorderSize
        border.color: innerBorderColor

        color: "transparent"
    }

    Rectangle {
        id: pressHighlight
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardBorderSize
        radius: parent.radius /2
        color: highlightColor
    }
}
