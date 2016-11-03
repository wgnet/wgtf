import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0


import WGControls 1.0

//This a prototype version of WGSubPanel just to test features for the advanced panel mockup.
//Do not use.


Rectangle {
    id: mainPanel

    //General Options
    property string text: ""            //Panel Title
    property string subText: ""         //panel sub title, used to convey data ownership

    property Component childObject_     //object inside the panel. Use a layout for multiple controls eg. WGColumnLayout, WGFormLayout
    property Component headerObject_    //object inside the header. Use a layout for multiple controls eg. WGExpandingRowLayout
    property QtObject headerLabelObject_: headerLabel  //link to the text inside the header so it can be attached to a copyable

    property QtObject rootPanel
    property QtObject rootFrame

    property QtObject parentChunk

    property int panelDepth

    //Best for Large solid color panels
    property bool collapsible_ : true
    property bool hasIcon_ : true

    property bool expanded_ : true

    property bool pinned

    property string closedIcon_: "icons/arrow_right_16x16.png"
    property string openIcon_: "icons/arrow_down_16x16.png"

    property bool boldHeader_: true

    //sub header properties
    property bool italicSubHeader_: true
    property bool boldSubHeader_: false

    property bool transparentPanel_: false

    property color colorHeader_ : palette.darkHeaderColor
    property color colorBody_ : palette.lightPanelColor

    //best for minor group box frames

    property bool hasSeparators_ : false    //Thin lines at top and bottom of frame
    property bool toggleable_ : false       //Make header a checkbox to enable/disable contents
    property alias checked_ : headerCheck.checked
    property alias exclusiveGroup: headerCheck.exclusiveGroup

    //change to true if contains a ScrollPanel or similar childObject which needs to obscure content
    property bool clipContents_: false

    //Recommend not changing these properties:

    property int contentLeftMargin_ : defaultSpacing.leftMargin
    property int contentRightMargin_ : defaultSpacing.rightMargin
    property int contentIndent_ : 0


    property bool finishedLoading_: false

    Layout.fillWidth: true

    color: "transparent"

    radius: defaultSpacing.standardRadius

    //if radius < 2, these panels look awful. This adds a bit more spacing.
    property int squareModifier: radius < 2 ? 8 : 0

    //Can have child panels indent further if set in global settings
    anchors.leftMargin: defaultSpacing.childIndentation

    height: {
        if (expanded_){
            content.height + defaultSpacing.minimumRowHeight + (radius * 4) + (defaultSpacing.topBottomMargin * 4) + squareModifier
        } else if (!expanded_){
            defaultSpacing.minimumRowHeight + (radius * 2) + (defaultSpacing.topBottomMargin * 2) + (squareModifier / 2)
        }
    }

    Layout.preferredHeight: {
        if (expanded_){
            content.height + defaultSpacing.minimumRowHeight + (radius * 4) + (defaultSpacing.topBottomMargin * 4) + squareModifier
        } else if (!expanded_){
            defaultSpacing.minimumRowHeight + (radius * 2) + (defaultSpacing.topBottomMargin * 2) + (squareModifier / 2)
        }
    }

    visible: {
        if (rootPanel.expanded_ == 2)
        {
            true
        }
        else
        {
            if(pinned)
            {
                true
            }
            else
            {
                false
            }
        }
    }

    function setParentPanel(parentObject){
        for(var i=0; i<parentObject.children.length; i++){
            if(typeof parentObject.children[i].parentPanel != "undefined"){
                if(parentObject.children[i].parentPanel == null){
                    if(parentObject.children[i] != mainPanel){
                        parentObject.children[i].parentPanel = mainPanel
                        parentObject.children[i].panelDepth = mainPanel.panelDepth
                        parentObject.children[i].parentFrame = parentObject.children[i].parent
                        parentObject.children[i].layoutRow = i
                    }
                }
                else
                {
                    //console.log(mainPanel.text + " has child " + parentObject.children[i] + " with parent panel:" + parentObject.children[i].parentPanel)
                }
            }
            setParentPanel(parentObject.children[i])
        }
    }

    function resetPanelOrder()
    {
        var startingLength = content.item.children.length
        var currentObject

        for(var currentRow=0; currentRow<startingLength; currentRow++)
        {
            for(var i=0; i<startingLength; i++)
            {
                currentObject = content.item.children[i]
                if(typeof currentObject.layoutRow != "undefined")
                {
                    if(currentObject.layoutRow == currentRow)
                    {
                        currentObject.parent = null
                        currentObject.parent = content.item
                        i = startingLength
                    }
                }
            }
        }
    }

    function incrementLayoutRow(from)
    {
        var startingLength = content.item.children.length
        for(var i=0; i<startingLength; i++)
        {
            var currentObject = content.item.children[i]
            if(typeof currentObject.layoutRow != "undefined")
            {
                if(currentObject.layoutRow >= from)
                {
                    currentObject.layoutRow += 1
                }
            }
        }
    }

    function decrementLayoutRow(from)
    {
        var startingLength = content.item.children.length
        for(var i=0; i<startingLength; i++)
        {
            var currentObject = content.item.children[i]
            if(typeof currentObject.layoutRow != "undefined")
            {
                if(currentObject.layoutRow >= from)
                {
                    currentObject.layoutRow -= 1
                }
            }
        }
    }


    signal collectChildren()

    //delay so panels don't animate when control is created
    Component.onCompleted: {
        animationDelay.start()
        if (transparentPanel_){
            colorHeader_ = "transparent"
            colorBody_ = "transparent"
            mainPanel.radius = 0
        }
        setParentPanel(mainPanel)
    }

    Connections {
        target: parentChunk
        onPinnedChanged:{
            mainPanel.pinned = parentChunk.pinned
        }
    }

    /*
    onPinnedChanged: {
        if(pinned)
        {
            rootPanel.pinnedChildren += 1
        }
        else
        {
            rootPanel.pinnedChildren -= 1
        }
    }*/

    Timer {
        id: animationDelay
        interval: 100
        onTriggered: {
            finishedLoading_ = true
        }
    }

    Behavior on Layout.preferredHeight{
        id: popAnimation
        enabled: finishedLoading_ && collapsible_
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

        visible: collapsible_ && !palette.glowStyle

        anchors.top: headerPanel.top
        anchors.bottom: headerPanel.bottom
        anchors.topMargin: mainPanel.radius + (squareModifier / 2)
        anchors.bottomMargin: mainPanel.radius + (squareModifier / 2)

        anchors.horizontalCenter: mainColor.horizontalCenter

        width: expanded_ ? mainColor.width - 6 : mainColor.width + 6 + (squareModifier)

        Component.onCompleted: {
            if(expanded_){
                expandingOuterFrame.color = "transparent"
            }
        }

        Behavior on width{
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
                     if (!running){
                         expandAnimation.enabled = false
                         if (expanded_){
                            expandingOuterFrame.color = "transparent"
                         }
                     } else {
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
                 GradientStop { position: 0.5; color: expanded_ ? palette.highlightShade : palette.highlightColor }
                 GradientStop { position: 1.0; color: "transparent" }
             }

        visible: collapsible_ && palette.glowStyle

        anchors.top: mainColor.top
        anchors.bottom: mainColor.bottom

        anchors.horizontalCenter: mainColor.horizontalCenter

        width: mainColor.width + 2

    }

    //main panel behind body panel that forms the header and footer
    Rectangle {
        id: mainColor
        radius: mainPanel.radius
        color: colorHeader_
        anchors.fill: parent
        anchors.topMargin: defaultSpacing.topBottomMargin
        anchors.bottomMargin: defaultSpacing.topBottomMargin
    }

    Item {
        id: headerPanel
        anchors {left: parent.left; right: parent.right}

        anchors.top: mainColor.top
        height: defaultSpacing.minimumRowHeight + mainPanel.radius * 2 + (squareModifier / 2)

        Rectangle {
            //mouse over panel and activeFocus border

            id: mouseHighlight
            color: "transparent"
            anchors.fill: parent
            anchors.margins: defaultSpacing.doubleBorderSize
            radius: defaultSpacing.halfRadius

            activeFocusOnTab: collapsible_

            border.width: defaultSpacing.standardBorderSize
            border.color: activeFocus && collapsible_ ? palette.highlightShade : "transparent"

            Keys.onPressed: {
                         if (event.key == Qt.Key_Space && collapsible_){
                             if (expanded_ && collapsible_){
                                 expandAnimation.enabled = true
                                 content.opacity = 0
                                 expanded_ = false;
                             } else if (!expanded_ && collapsible_){
                                 expandAnimation.enabled = true
                                 expanded_ = true;
                                 content.visible = true
                                 content.opacity = 1;
                             }
                         }
                     }
        }

        Image {
            id: headerIcon
            visible: hasIcon_
            width: hasIcon_ ? sourceSize.width : 0
            source: expanded_ ? mainPanel.openIcon_ : mainPanel.closedIcon_
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: contentLeftMargin_
        }

        //mouse area that expands the panel. Doesn't overlap any controls in the header
        MouseArea {
            id: expandMouseArea
            anchors.left: parent.left
            // anchors.right: headerObject_ ? headerControl.left : (pinable_ ? pinButton.left : parent.right)
            anchors.right: headerObject_ ? headerControl.left : panelMenu.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            propagateComposedEvents: true
            visible: collapsible_

            activeFocusOnTab: false

            hoverEnabled: true

            onEntered: {
                if (collapsible_ && !rootPanel.choosePinned && !rootPanel.chunkDragEnabled){
                    mouseHighlight.color = palette.lighterShade
                }
            }

            onExited: {
                if (collapsible_){
                    mouseHighlight.color = "transparent"
                }
            }

            onClicked: {
                if (expanded_ && collapsible_){
                    expandAnimation.enabled = true
                    content.opacity = 0
                    expanded_ = false;
                } else if (!expanded_ && collapsible_){
                    expandAnimation.enabled = true
                    expanded_ = true;
                    content.visible = true
                    content.opacity = 1;
                }
            }
        }

        //Panel title text or checkbox if toggleable
        Item {
            id: headerBox
            anchors.left: headerIcon.right
            anchors.leftMargin: collapsible_ ? defaultSpacing.leftMargin : 0
            anchors.verticalCenter: parent.verticalCenter
            width: childrenRect.width

            Text {
                id: headerLabel
                font.bold: boldHeader_
                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                color: palette.textColor
                anchors.verticalCenter: parent.verticalCenter
                text: mainPanel.text
                visible: toggleable_ ? false : true
            }
            //SubPanel secondary title
            Text {
                id: headerSubLabel
                font.bold: boldSubHeader_
                font.italic: italicSubHeader_
                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                color: palette.textColor
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: headerLabel.right
                text: (mainPanel.subText == "") ? "" : " - " + mainPanel.subText
                visible: toggleable_ ? false : true
            }

            WGCheckBox {
                id: headerCheck
                text: mainPanel.text
                visible: toggleable_ ? true : false
                width: toggleable_ ? implicitWidth : 0
                checked: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        //separators only for group boxes
        WGSeparator {
            vertical: false

            visible: hasSeparators_

            anchors.left: headerBox.right
            anchors.right: panelMenu.left

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: defaultSpacing.standardMargin

            anchors.verticalCenter: parent.verticalCenter
        }

        //controls that can be placed in the header itself
        Loader {
            id: headerControl

            anchors.left: headerBox.right
            anchors.right: panelMenu.left
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: contentRightMargin_
            enabled: headerCheck.checked

            sourceComponent: headerObject_
        }

        WGToolButton {
            id: panelMenu
            anchors.right: parent.right

            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: defaultSpacing.standardMargin

            iconSource: "icons/menu_16x16.png"

            menu: WGMenu{
                MenuItem {
                    text: "Copy Panel Data"
                    enabled: false
                }
                MenuItem {
                    text: "Paste Panel Data"
                    enabled: false
                }
            }
        }
    }

    //loads child object.
    //Highly recommended first child object is a WGColumnLayout or similar
    Loader {
        id: content

        clip: clipContents_

        anchors {left: parent.left; right: parent.right}

        anchors.top: headerPanel.bottom
        anchors.topMargin: defaultSpacing.topBottomMargin
        z: 1

        anchors.leftMargin: contentLeftMargin_ + contentIndent_
        anchors.rightMargin: contentRightMargin_

        sourceComponent: childObject_

        enabled: (headerCheck.checked && expanded_)

        onOpacityChanged: {
            if(opacity == 0){
                visible = false
            }
        }

        Component.onCompleted: {
            if (!expanded_){
                visible = false
                opacity = 0
            }
        }

        Behavior on opacity{
            id: fadeAnimation
            enabled: finishedLoading_ && collapsible_
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

        color: parent.colorBody_

        anchors.top: headerPanel.bottom

        height: parent.expanded_ ? content.height + defaultSpacing.doubleMargin - (squareModifier / 2): 0

        Behavior on height{
            id: bodyPopAnimation
            enabled: finishedLoading_ && collapsible_
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

        visible: hasSeparators_

        anchors.left: parent.left
        anchors.right: parent.right

        anchors.leftMargin: contentLeftMargin_
        anchors.rightMargin: contentRightMargin_

        anchors.top: content.bottom
        anchors.topMargin: defaultSpacing.topBottomMargin
    }
}
