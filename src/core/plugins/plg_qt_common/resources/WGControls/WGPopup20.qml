import QtQuick 2.5
import QtQuick.Controls 1.4
import Qt.labs.controls 1.0 as Labs
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0

import WGControls 2.0
import WGControls.Private 2.0
import WGControls.Global 2.0

/*!
    \ingroup wgcontrols
    \brief A popup window using the QtLabs/QtQuick2 Popup control that can automatically close.
    Intended to be parented to a button or MouseArea but can be opened by almost anything.

    The popup itself can be sized, otherwise it will conform to the size of the first child item.

Example:
\code{.js}
WGPushbutton {
    text: "Open Popup"
    WGPopup {
        width: 100
        height: 50
        Image {
            //will be resized to fit popup
            source: "folder/myimage.jpg"
        }
    }
}
\endcode

\code{.js}
WGPushbutton {
    text: "Open Popup"
    WGPopup {
        openPolicy: openOnHover
        padding: 5
        Rectangle {
            // popup will resize to fit this with 5px border
            width: 200
            height: 200
            color: "red"
        }
    }
}
\endcode
*/

Labs.Popup {
    id: popup
    objectName: "WGPopup"
    WGComponent { type: "WGPopup20" }

    /*! If true, the popup will close shortly after the mouse leaves the popup area. Otherwise
        it will only close based on the standard closePolicy.

        The default is true.
    */
    property bool closeOnMouseLeave: true

    /*! Determines how to the popup is opened if the parent is a Button or MouseArea (or has similar states/signals).

        Acceptable values are:
        noAutoOpen: The popup will not open unless open() is manually called somewhere
        openOnHover: The popup will open when the parent object is entered or hovered == true.
        openOnPress: The popup will open when the mouse is pressed on the parent object.
        openOnClick: The popup will open when the mouse is released on the parent object.
        openOnDoubleClick: The popup will open when the mouse is double clicked on the parent object.

        The default value is openOnPress
    */
    property var openPolicy: openOnPress

    /*! The object that opens the popup and determines its position. Generally this should be a button or mouseArea.

        The default is the popup's parent.
    */
    property var openTarget: parent

    /*! Whether the mouseArea that checks if the cursor has left the popup is on top of the contentItem or below it in z order.

        Setting this to false will allow hovered states to work in contentItem children but these mouseAreas MUST then set
        popupHovered to true when they are hovered or the popup will close prematurely.

        If true, closeOnMouseLeave will work automatically but its mouseArea will interfere with any hover states etc in the contentItem.
    */
    property bool exitMouseAreaOnTop: true

    /*! True if closeOnMouseLeave and the mouseArea contains the the cursor.

        Set this to true if contentItem children are hovered and exitMouseAreaOnTop is false.
    */
    property bool popupHovered: false

    /*! Set to true if you have other conditions where you do not want the popup to close.

      Default value is false.
    */
    property bool remainOpen: false

    /*! Internal openPolicy Enum */
    readonly property var noAutoOpen: 0
    /*! Internal openPolicy Enum */
    readonly property var openOnHover: 1
    /*! Internal openPolicy Enum */
    readonly property var openOnPress: 2
    /*! Internal openPolicy Enum */
    readonly property var openOnClick: 4
    /*! Internal openPolicy Enum */
    readonly property var openOnDoubleClick: 8

    /*! A readonly property that tries to show the space available for the popup from the controls left margin to parentFrame's right margin.
      Won't give a good result if it can't find a parentFrame that is wider than the control.
    */
    readonly property int popupXSpace: i.parentFrameWidth - ( popup.parent ? popup.parent.mapToItem(parentFrame, defaultSpacing.standardMargin, 0).x : 0 )

    /*! The best 'visual' parent frame the popup can find when it appears by walking up the tree.
    */
    property var parentFrame: parent ? parent.parent : null

    y: openTarget ? openTarget.height : 0

    // changes the popup's horizontal alignment if there is not enough popupXSpace in it's parent to the right of it.
    x: {
        if (i.parentFrameWidth >= popup.width)
        {
            return popupXSpace < popup.width ? popup.parent.width - popup.width : 0
        }
        else
        {
            // if there is no parentFrame wide enough for the popup anyway just left align. Means it won't behave weirdly if the parent is not a 'good' visual frame.
            return 0
        }
    }

    implicitWidth: Math.max(background ? background.implicitWidth : 0, contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(background ? background.implicitHeight : 0, contentHeight + topPadding + bottomPadding)

    contentWidth: contentItem.implicitWidth || (contentChildren.length > 2 ? contentChildren[2].width : 0)
    contentHeight: contentItem.implicitHeight || (contentChildren.length > 2 ? contentChildren[2].height : 0)

    focus: true
    modal: true
    visible: false

    padding: defaultSpacing.doubleBorderSize

    closePolicy: Labs.Popup.OnPressOutside | Labs.Popup.onEscape

    onRemainOpenChanged: {
        WGPopupTimer.restartTimer();
    }

    onPopupHoveredChanged: {
        if (!popupHovered && popup.visible && closeOnMouseLeave)
        {
            WGPopupTimer.restartTimer();
            WGPopupTimer.open = false
        }
        else if (popupHovered && closeOnMouseLeave)
        {
            WGPopupTimer.stopTimer();
            WGPopupTimer.triggeringObject = null
        }
    }

    onVisibleChanged: {
        popupHovered = visible

        var topParent = parentFrame
        var maxWidthParent = parentFrame

        while (typeof topParent.parent != "undefined" && topParent.parent != null)
        {
            if (topParent.parent.width > maxWidthParent.width)
            {
                maxWidthParent = topParent.parent
            }
            topParent = topParent.parent
        }
        parentFrame = maxWidthParent
    }

    QtObject {
        id: i
        property bool doubleCheck: false
        readonly property var parentFrameWidth: parentFrame == null ? 0 : parentFrame.width
    }


    Connections {
        target: openTarget
        ignoreUnknownSignals: true

        onPressedChanged: {
            if (openTarget.pressed && openPolicy == openOnPress && !popup.visible)
            {
                popup.open()
            }
        }

        onClicked: {
            if (!popup.visible)
            {
                if (openPolicy == openOnClick && !popup.visible || (openPolicy == openOnDoubleClick && i.doubleCheck))
                {
                    popup.open()
                    i.doubleCheck = false
                }
                // manual double click check for buttons without doubleClicked() signal
                if (openPolicy == openOnDoubleClick && !i.doubleCheck)
                {
                    i.doubleCheck = true
                    WGDoubleClickTimer.restartTimer()
                }
            }
        }

        onDoubleClicked: {
            if (openPolicy == openOnDoubleClick && !popup.visible)
            {
                popup.open()
            }
        }

        onEntered: {
            if (!popup.visible && openPolicy == openOnHover)
            {
                WGPopupTimer.restartTimer();
                WGPopupTimer.open = true
                WGPopupTimer.triggeringObject = openTarget
            }
            // stop the popup autoClosing if the user mouseovers the parent object again
            if (closeOnMouseLeave && popup.visible)
            {
                popupHovered = true
            }
        }

        onExited: {
            if (!popup.visible && openPolicy == openOnHover)
            {
                WGPopupTimer.stopTimer();
                WGPopupTimer.open = false
                WGPopupTimer.triggeringObject = null
            }
            if (closeOnMouseLeave && popup.visible)
            {
                popupHovered = false
            }
        }

        onHoveredChanged: {
            if (openPolicy == openOnHover)
            {
                if (openTarget.hovered && !popup.visible)
                {
                    WGPopupTimer.open = true
                    WGPopupTimer.restartTimer();
                    WGPopupTimer.triggeringObject = openTarget
                }
                else if (!openTarget.hovered && !popup.visible)
                {
                    WGPopupTimer.stopTimer();
                    WGPopupTimer.open = false
                    WGPopupTimer.triggeringObject = null
                }
            }

            if (openTarget.hovered)
            {
                if (closeOnMouseLeave && popup.visible && WGPopupTimer.running)
                {
                    popupHovered = true
                }
            }
            else
            {
                if (closeOnMouseLeave && popup.visible)
                {
                    popupHovered = false
                }
            }
        }
    }

    Connections {
        target: WGPopupTimer
        onTimerTriggered: {
            if (popup.visible)
            {
                if (closeOnMouseLeave && !popupArea.containsMouse && !popup.popupHovered && !remainOpen)
                {
                    popup.close()
                }
            }
            else if (openPolicy == openOnHover)
            {
                if (WGPopupTimer.triggeringObject == openTarget && WGPopupTimer.open)
                {
                    popup.open()
                    WGPopupTimer.triggeringObject = null
                }
            }
        }
    }

    Connections {
        target: WGDoubleClickTimer
        onTimerTriggered: {
            i.doubleCheck = false
        }
    }

    contentItem: Item {
        MouseArea {
            // area to close the popup automatically
            id: popupArea
            z: {
                if (exitMouseAreaOnTop)
                {
                    return contentChildren.length > 0 ? contentChildren[contentChildren.length - 1].z + 1 : 1
                }
                else
                {
                    return -1
                }
            }
            anchors.centerIn: parent
            height: parent.height + topPadding + bottomPadding
            width: parent.width + leftPadding + rightPadding
            enabled: popup.visible && closeOnMouseLeave
            hoverEnabled: closeOnMouseLeave

            acceptedButtons: Qt.NoButton

            onEntered: {
                popupHovered = true
            }

            onExited: {
                popupHovered = false
            }
        }
    }

    background: Rectangle {
        color: palette.mainWindowColor
        WGTextBoxFrame {
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardBorderSize
            color: palette.darkestShade
        }
    }
}
