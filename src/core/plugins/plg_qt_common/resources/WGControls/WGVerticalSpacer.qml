import QtQuick 2.5

/*!
 \ingroup wgcontrols
 \brief An invisible box that can have height set by number of lineSpaces_

Example:
\code{.js}
WGVerticalSpacer{
    anchors{left: parent.left; right:parent.right;}
}
\endcode
*/

Rectangle {
    id: spacerFrame
    objectName: "WGVerticalSpacer"
    WGComponent { type: "WGVerticalSpacer" }

    anchors {left: parent.left; right: parent.right}
    color: "transparent"

    /*!
        This property defines how high the spacer should be in line spaces
        The default value is an empty 1
    */
    property int lineSpaces: 1

    implicitHeight: defaultSpacing.minimumRowHeight * lineSpaces

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! Deprecated */
    property alias lineSpaces_: spacerFrame.lineSpaces

    /*! Deprecated */
    property alias label_: spacerFrame.label
}
