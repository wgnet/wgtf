import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

/*!
 \ingroup wgcontrols
 \brief An Expandable Subpanel/GroupBox
 Prebuilt alternatives: WGInternalPanel, WGGroupBox.
 Will not work properly in a WGFormLayout, use WGColumnLayout instead.
 Consider using WGFrame for simple panels to group objects that don't need a title or to expand.

Example:
\code{.js}
WGSubPanel {
    text: "Example Title"
    childObject :
        WGColumnLayout {
            WGLodSlider {
                Layout.fillWidth: true
            }
    }
}
\endcode
*/

Rectangle {
    id: mainPanel
    objectName: "WGSubPanel"
    WGComponent { type: "WGSubPanel" }

    /*! This property holds the panel title text
        The default value is an empty string
    */
    property string text: ""

    /*!
        This property defines what objects are inside the panel.
        Use a layout for multiple controls eg. WGColumnLayout, WGFormLayout.
    */
    property Component childObject

    /*!
        This property defines what objects might appear in the header of the sub panel.
        Use a layout for multiple controls eg. WGExpandingRowLayout
        Current designs have not included this functionality.
    */
    property Component headerObject

    /*! This property toggles holds a link to the text inside the header so it can be attached to a copyable */
    property QtObject __headerLabelObject: headerLabel

    /*! This property defines whether a panel should be collapsible by the user
        The default value is \c true
    */
    property bool collapsible : true

    /*! This property determines whether a panel has an icon in its title
        The default value is \c true
    */
    property bool hasIcon : true

    /*! This property determines if a panel is expanded by default
        The default value is \c true
    */
    property bool expanded : true

    /*! This property defines the location of the icon displayed when a panel is closed */
    property string closedIcon: "icons/arrow_right_16x16.png"

    /*! This property defines the location of the icon displayed when a panel is open */
    property string openIcon: "icons/arrow_down_16x16.png"

    /*! This property determines if the sub panel header will be in bold text
        The default value is \c true
    */
    property bool boldHeader: true

    //sub header properties

    /*! This property toggles holds defines panel sub title, used to convey data ownership.
        For example, a Property panel may be displaying a chicken's properties. text would be "property", subtext would be "chicken".
        The default value is an empty string
    */
    property string subText: ""

    /*! This property toggles determines if the sub header will be in italic text.
        The default value is \c true
    */
    property bool italicSubHeader: true

    /*! This property toggles determines if the sub header will be in bold text.
        The default value is \c false
    */
    property bool boldSubHeader: false

    /*! This property toggles the visibility of the background frame that contains the sub panels contents.
        The default value is \c true
    //TODO: When set true this creates undesirable UI unless used in conjuction with collapsible:false.
    //This property needs to be made internal and a new sub class (WGStaticPanel) made with default transparentPanel:true and collapsible:false
    */property bool transparentPanel: false

    /*! This property determines the colour of the panel header.*/
    property color colorHeader : palette.darkHeaderColor

    /*! This property determines the colour of the panel body.*/
    property color colorBody : palette.lightPanelColor

    //best for minor group box frames

    /*! This property toggles the visibility of thin lines at the top and bottom of the frame
        The default value is \c false
    */
    //TODO The separator in the title does not work well when the panel has headerObject's.
    //If headerObjects are going to be used this should be fixed.
    property bool hasSeparators : false

    /*! This property adds a checkbox to a panels title bar that enables/disables the panel contents.
        The default value is \c false
    */
    property bool toggleable : false

    /*! This property sets the checked state of the header checkbox enabled by toggleable:true */
    property alias checked : headerCheck.checked

    /*! This can be used to assign an exclusive group to the checkbox for the SubPanel */
    property alias exclusiveGroup: headerCheck.exclusiveGroup

    /*! This property will clip child components such as WGScrollPanel or similar which may extend outside the bounds of the scroll panel
        The default value is \c false
    */
    property bool clipContents: false

    /*! This property determines the amount of spacing on the left side of the content item. Changing this value can easily give undesirable results. */
    property int contentLeftMargin : defaultSpacing.leftMargin

    /*! This property determines the amount of spacing on the right side of the content item. Changing this value can easily give undesirable results. */
    property int contentRightMargin : defaultSpacing.rightMargin

    /*! This property determines the amount of extra indent on the left side of the content item. Changing this value can easily give undesirable results. */
    property int contentIndent : 0

    /*! \internal */
    property bool __finishedLoading: false

    //if radius < 2, these panels look awful. This adds a bit more spacing.
    /*! \internal */
    property int __squareModifier: radius < 2 ? 8 : 0

    property alias menu: panelMenu.menu

    property alias titleFontSize: headerLabel.font.pointSize

    property alias subtitleFontSize: headerSubLabel.font.pointSize

	/** Switch to load asynchronously or synchronously.*/
    property bool asynchronous: false

    //delay so panels don't animate when control is created
    Component.onCompleted: {
        animationDelay.start()
        if (transparentPanel)
        {
            colorHeader = "transparent"
            colorBody = "transparent"
            mainPanel.radius = 0
        }
        subPanel_HeaderLaber_WGCopyable.setParentCopyable( mainPanel )
    }

    Layout.fillWidth: true

    color: "transparent"

    radius: defaultSpacing.standardRadius

    implicitHeight: defaultSpacing.minimumRowHeight + (radius * 4) + (defaultSpacing.topBottomMargin * 4) + __squareModifier
    implicitWidth: defaultSpacing.standardMargin

    height: {
        if (expanded)
        {
            content.height + defaultSpacing.minimumRowHeight + (radius * 4) + (defaultSpacing.topBottomMargin * 4) + __squareModifier
        }
        else if (!expanded)
        {
            defaultSpacing.minimumRowHeight + (radius * 2) + (defaultSpacing.topBottomMargin * 2) + (__squareModifier / 2)
        }
    }

    Layout.preferredHeight: {
        if (expanded)
        {
            content.height + defaultSpacing.minimumRowHeight + (radius * 4) + (defaultSpacing.topBottomMargin * 4) + __squareModifier
        }
        else if (!expanded)
        {
            defaultSpacing.minimumRowHeight + (radius * 2) + (defaultSpacing.topBottomMargin * 2) + (__squareModifier / 2)
        }
    }

    WGCopyable{
        id: subPanel_HeaderLaber_WGCopyable
    }

    Timer {
        id: animationDelay
        interval: 100
        onTriggered: {
            __finishedLoading = true
        }
    }

    Behavior on Layout.preferredHeight{
        id: popAnimation
        enabled: __finishedLoading && collapsible
        NumberAnimation {
            duration: 120
            easing {
                type: Easing.OutCirc
                amplitude: 1.0
                period: 0.5
            }
        }
    }

    //bulge that appears in collapsed panels if not GlowStyle
    Rectangle {
        id: expandingOuterFrame
        radius: defaultSpacing.halfRadius
        color: palette.highlightShade
        visible: collapsible && !palette.glowStyle
        anchors.top: headerPanel.top
        anchors.bottom: headerPanel.bottom
        anchors.topMargin: mainPanel.radius + (__squareModifier / 2)
        anchors.bottomMargin: mainPanel.radius + (__squareModifier / 2)
        anchors.horizontalCenter: mainColor.horizontalCenter

        width: expanded ? mainColor.width - 6 : mainColor.width + 6 + (__squareModifier)

        Component.onCompleted: {
            if(expanded)
            {
                expandingOuterFrame.color = "transparent"
            }
        }

        Behavior on width {
            id: expandAnimation
            enabled: false
            NumberAnimation {
                 duration: 120
                 easing {
                     type: Easing.OutCirc
                     amplitude: 1.0
                     period: 0.5
                 }

                 onRunningChanged: {
                     if (!running)
                     {
                         expandAnimation.enabled = false
                         if (expanded)
                         {
                            expandingOuterFrame.color = "transparent"
                         }
                     }
                     else
                     {
                         expandingOuterFrame.color = palette.highlightShade
                     }
                 }
            }
        }
    }

    //glow that appears in collapsed panels if GlowStyle
    Rectangle {
        id: glowingSideFrame
        radius: 10

        gradient: Gradient {
                 GradientStop { position: 0.0; color: "transparent" }
                 GradientStop { position: 0.5; color: expanded ? palette.highlightShade : palette.highlightColor }
                 GradientStop { position: 1.0; color: "transparent" }
             }

        visible: collapsible && palette.GlowStyle

        anchors.top: mainColor.top
        anchors.bottom: mainColor.bottom

        anchors.horizontalCenter: mainColor.horizontalCenter

        width: mainColor.width + 2

    }

    //main panel behind body panel that forms the header and footer
    Rectangle {
        id: mainColor
        radius: mainPanel.radius
        color: colorHeader
        anchors.fill: parent
        anchors.topMargin: defaultSpacing.topBottomMargin
        anchors.bottomMargin: defaultSpacing.topBottomMargin
    }

    Rectangle {
        id: headerPanel
        anchors {left: parent.left; right: parent.right}

        color: "transparent"

        anchors.top: mainColor.top
        height: defaultSpacing.minimumRowHeight + mainPanel.radius * 2 + (__squareModifier / 2)

        Rectangle {
            objectName: "panel"
            //mouse over panel and activeFocus border

            id: mouseHighlight
            color: "transparent"
            anchors.fill: parent
            anchors.margins: defaultSpacing.doubleBorderSize
            radius: defaultSpacing.halfRadius

            activeFocusOnTab: collapsible

            border.width: defaultSpacing.standardBorderSize
            border.color: activeFocus && collapsible ? palette.lighterShade : "transparent"

            Keys.onPressed: {
                if (event.key == Qt.Key_Space && collapsible)
                {
                    if (expanded && collapsible)
                    {
                        expandAnimation.enabled = true
                        content.opacity = 0
                        expanded = false;
                    }
                    else if (!expanded && collapsible)
                    {
                        expandAnimation.enabled = true
                        expanded = true;
                        content.visible = true
                        content.opacity = 1;
                    }
                }
            }
        }

        Image {
            id: headerIcon
            visible: hasIcon
            width: hasIcon ? sourceSize.width : 0
            source: expanded ? mainPanel.openIcon : mainPanel.closedIcon
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: contentLeftMargin
        }

        //mouse area that expands the panel. Doesn't overlap any controls in the header
        MouseArea {
            id: expandMouseArea
            objectName: "expandSubPanel"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: collapsible

            activeFocusOnTab: false

            hoverEnabled: true

            onEntered: {
                if (collapsible)
                {
                    mouseHighlight.color = palette.lighterShade
                }
            }

            onExited: {
                if (collapsible)
                {
                    mouseHighlight.color = "transparent"
                }
            }

            onClicked: {
                if (expanded && collapsible)
                {
                    expandAnimation.enabled = true
                    content.opacity = 0
                    expanded = false;
                }
                else if (!expanded && collapsible)
                {
                    expandAnimation.enabled = true
                    expanded = true;
                    content.visible = true
                    content.opacity = 1;
                }
            }
        }

        //Panel title text or checkbox if toggleable
        Rectangle {
            id: headerBox
            anchors.left: headerIcon.right
            anchors.leftMargin: collapsible ? defaultSpacing.leftMargin : 0
            anchors.verticalCenter: parent.verticalCenter
            width: childrenRect.width
            color: "transparent"
            Text {
                objectName: "Text"
                id: headerLabel
                font.bold: boldHeader
                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                color: palette.highlightTextColor
                anchors.verticalCenter: parent.verticalCenter
                text: mainPanel.text
                visible: toggleable ? false : true

                MouseArea {
                    anchors.fill: enabled ? parent : undefined
                    enabled: headerLabel.visible && globalSettings.wgCopyableEnabled
                    hoverEnabled: enabled
                    cursorShape: Qt.PointingHandCursor

                    onClicked:{
                        if (!subPanel_HeaderLaber_WGCopyable.enabled || !globalSettings.wgCopyableEnabled )
                        {
                            mouse.accepted = false
                            return;
                        }

                        if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
                        {
                            if (subPanel_HeaderLaber_WGCopyable.selected)
                            {
                                subPanel_HeaderLaber_WGCopyable.deSelect()
                            }
                            else
                            {
                                subPanel_HeaderLaber_WGCopyable.select()
                            }
                        }
                        else if (mouse.button == Qt.LeftButton)
                        {
                            subPanel_HeaderLaber_WGCopyable.rootCopyable.deSelect();
                            subPanel_HeaderLaber_WGCopyable.select()
                        }
                    }
                }
            }
            //SubPanel secondary title
            Text {
                id: headerSubLabel
                font.bold: boldSubHeader
                font.italic: italicSubHeader
                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                color: palette.highlightTextColor
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: headerLabel.right
                text: (mainPanel.subText == "") ? "" : " - " + mainPanel.subText
                visible: toggleable ? false : true
            }

            WGCheckBox {
                objectName: "CheckBox"
                id: headerCheck
                text: mainPanel.text
                visible: toggleable ? true : false
                enabled: visible
                width: toggleable ? implicitWidth : 0
                checked: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        //separators only for group boxes
        WGSeparator {
            vertical: false
            visible: hasSeparators
            anchors.left: headerBox.right
            anchors.right: panelMenu.left

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: defaultSpacing.standardMargin
            anchors.verticalCenter: parent.verticalCenter
        }

        //controls that can be placed in the header itself
        WGLoader {
            id: headerControl
			asynchronous: mainPanel.asynchronous
			loading: !(status == Loader.Ready) && headerObject != null
            anchors.left: headerBox.right
            anchors.right: panelMenu.left
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: contentRightMargin
            enabled: headerCheck.checked

            sourceComponent: headerObject
        }

        WGToolButton {
            id: panelMenu
            objectName: "subPanelMenuButton"
            anchors.right: parent.right

            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: defaultSpacing.standardMargin

            iconSource: "icons/menu_16x16.png"
        }
    }

    //loads child object.
    //Highly recommended first child object is a WGColumnLayout or similar
    WGLoader {
        id: content

        clip: clipContents
		asynchronous: mainPanel.asynchronous
		loading: !(status == Loader.Ready) && childObject != null
        anchors {left: parent.left; right: parent.right}

        anchors.top: headerPanel.bottom
        anchors.topMargin: defaultSpacing.topBottomMargin

        anchors.leftMargin: contentLeftMargin + contentIndent
        anchors.rightMargin: contentRightMargin

        z: 1

        sourceComponent: childObject

        enabled: (headerCheck.checked && expanded)

        onOpacityChanged: {
            if(opacity == 0){
                visible = false
            }
        }

        Component.onCompleted: {
            if (!expanded){
                visible = false
                opacity = 0
            }
        }

        Behavior on opacity{
            id: fadeAnimation
            enabled: __finishedLoading && collapsible
            NumberAnimation {
                 duration: 120
                 easing {
                     type: Easing.OutCirc
                     amplitude: 1.0
                     period: 0.5
                 }
            }
        }
    }

    //panel that contains the content.
    Rectangle {
        id: bodyPanel
        anchors {left: parent.left; right: parent.right}

        color: parent.colorBody

        anchors.top: headerPanel.bottom

        height: parent.expanded ? content.height + defaultSpacing.doubleMargin - (__squareModifier / 2): 0

        Behavior on height{
            id: bodyPopAnimation
            enabled: __finishedLoading && collapsible
            NumberAnimation {
                 duration: 120
                 easing {
                     type: Easing.OutCirc
                     amplitude: 1.0
                     period: 0.5
                 }
            }
        }
    }

    //separators only for group boxes
    WGSeparator {
        vertical: false

        visible: hasSeparators

        anchors.left: parent.left
        anchors.right: parent.right

        anchors.leftMargin: contentLeftMargin
        anchors.rightMargin: contentRightMargin

        anchors.top: content.bottom
        anchors.topMargin: defaultSpacing.topBottomMargin
    }

    /*! Deprecated */
    property alias childObject_: mainPanel.childObject

    /*! Deprecated */
    property alias headerObject_: mainPanel.headerObject

    /*! Deprecated */
    property alias headerLabelObject_: mainPanel.__headerLabelObject

    /*! Deprecated */
    property alias collapsible_ : mainPanel.collapsible

    /*! Deprecated */
    property alias hasIcon_ : mainPanel.hasIcon

    /*! Deprecated */
    property alias expanded_ : mainPanel.expanded

    /*! Deprecated */
    property alias closedIcon_: mainPanel.closedIcon

    /*! Deprecated */
    property alias openIcon_: mainPanel.openIcon

    /*! Deprecated */
    property alias boldHeader_: mainPanel.boldHeader

    /*! Deprecated */
    property alias italicSubHeader_: mainPanel.italicSubHeader

    /*! Deprecated */
    property alias boldSubHeader_: mainPanel.boldSubHeader

    /*! Deprecated */
    property alias transparentPanel_: mainPanel.transparentPanel

    /*! Deprecated */
    property alias colorHeader_ : mainPanel.colorHeader

    /*! Deprecated */
    property alias colorBody_ : mainPanel.colorBody

    /*! Deprecated */
    property alias hasSeparators_ : mainPanel.hasSeparators

    /*! Deprecated */
    property alias toggleable_ : mainPanel.toggleable

    /*! Deprecated */
    property alias checked_ : mainPanel.checked

    /*! Deprecated */
    property alias clipContents_: mainPanel.clipContents

    /*! Deprecated */
    property alias contentLeftMargin_ : mainPanel.contentLeftMargin

    /*! Deprecated */
    property alias contentRightMargin_ : mainPanel.contentRightMargin

    /*! Deprecated */
    property alias contentIndent_ : mainPanel.contentIndent

    /*! Deprecated */
    property alias finishedLoading_: mainPanel.__finishedLoading

    /*! Deprecated */
    property alias squareModifier: mainPanel.__squareModifier
}
