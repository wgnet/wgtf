import QtQuick 2.3
import QtQuick.Layouts 1.1

/*!
    \brief Stripped back dark or light frame. Not expandable, not toggleable, no title.

    These will match WGSubPanel

    If at lvl 0 recommend:
    dark: false
    shade: false

    If at lvl 1 recommend:
    dark: true
    shade: false

    Use shade_: true sparingly for a subtle darker or lighter frame to group objects

Example:
\code{.js}
WGFrame {
    dark: false
    shade: true

    label: "File Options:"

    childObject:
    WGColumnLayout {

        WGCheckBox {
            text: "File Option 1"
        }
        WGCheckBox {
            text: "File Option 2"
        }
        WGCheckBox {
            enabled: false
            text: "Disabled File Option"
        }
    }
}
\endcode
*/

Item {
    id: baseFrame
    objectName: "WGFrame"

    /*! This property sets the frame colour to dark
        The default value is \c true
    */
    property bool dark: true

    /*! This property alters the frame colour
        The default value is \c false
    */
    property bool shade: false

    // TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property alias frameColor: frame.color

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label: ""

    //This could be broken out into two separate style files but for now its so simple I created
    //it as a simple swicth.
    /*! This property determines if the Frame should use a lined box in place of a shaded rectangle */
    property bool lineFrame: false

    /*! This property defines the text used as a title for the frame. Note, text is only visible when
      lineFrame style is used */
    property string text: ""

    /*! This property determines if the contents of the frame can be disabled. Note, the checkbox is only
        visible if lineFrame is used*/
    property bool toggleable: true

    /*! This property sets the checked state of the controls checkbox. */
    property alias enableControls: toggleableBox.checked

    property Component childObject

    implicitHeight: defaultSpacing.doubleMargin + (defaultSpacing.topBottomMargin * 2)
    implicitWidth: defaultSpacing.standardMargin

    property bool __hasHeader: lineFrame && (text != "" || toggleable) ? true : false

    height: lineFrame ? content.height + defaultSpacing.doubleMargin + defaultSpacing.topBottomMargin + lineFrameHeader.height
                         : (content.height + defaultSpacing.doubleMargin + (defaultSpacing.topBottomMargin * 2))

    Layout.preferredHeight: height

    Layout.fillWidth: true

    Rectangle {
        id: frame
        visible: !lineFrame
        color: {
            if (shade)
            {
                if (dark)
                {
                    palette.darkShade
                }
                else
                {
                    palette.lightShade
                }
            }
            else
            {
                if (dark)
                {
                    palette.mainWindowColor
                }
                else
                {
                    palette.lightPanelColor
                }
            }
        }

        radius: defaultSpacing.standardRadius
        height: parent.height - (defaultSpacing.topBottomMargin * 2)
        Layout.preferredHeight: parent.height - (defaultSpacing.topBottomMargin * 2)
        anchors {left: parent.left; right: parent.right}
        anchors.verticalCenter: parent.verticalCenter

    }

    Item {
        id: lineFrameHeader
        height: defaultSpacing.minimumRowHeight
        anchors {left: parent.left; right: parent.right}
        anchors.top: parent.top
        visible: lineFrame

        Text {
            id: frameLabel
            color: palette.highlightTextColor
            anchors.left: parent.left
            anchors.top: parent.top
            text: baseFrame.text
            visible: toggleable ? false : true
        }

        WGCheckBox {
            id: toggleableBox
            objectName: "ToggleFrameCheckBox"
            text: baseFrame.text
            visible: toggleable ? true : false
            enabled: visible
            width: toggleable ? implicitWidth : 0
            checked: true
            anchors.left: parent.left
            anchors.top: parent.top
        }

        WGSeparator {
            id: topSeparator
            vertical: false
            anchors.left: __hasHeader ? (toggleable ? toggleableBox.right : frameLabel.right) : parent.left
            anchors.right: lineFrameHeader.right
            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: defaultSpacing.standardMargin
            anchors.verticalCenter: toggleable ? toggleableBox.verticalCenter : frameLabel.verticalCenter
        }
    }

    // Frame content
    Item {
        id: contentHolder
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: lineFrame ? bottomSeparator.top : parent.bottom
        anchors.top: lineFrame ? lineFrameHeader.bottom : parent.top

        Loader {
            id: content
            anchors {left: parent.left; right: parent.right}
            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: defaultSpacing.rightMargin
            anchors.verticalCenter: parent.verticalCenter
            enabled: enableControls

            sourceComponent: childObject
            z: 1
        }
    }

    WGSeparator {
        id: bottomSeparator
        vertical: false
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: defaultSpacing.leftMargin
        anchors.rightMargin: defaultSpacing.standardMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: defaultSpacing.standardMargin
        visible: lineFrame
    }

    /*! Deprecated */
    property alias dark_: baseFrame.dark

    /*! Deprecated */
    property bool shade_: baseFrame

    /*! Deprecated */
    property alias color_: baseFrame.frameColor

    /*! Deprecated */
    property alias label_: baseFrame.label

    /*! Deprecated */
    property alias childObject_: baseFrame.childObject
}

