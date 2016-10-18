import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0


import WGControls 1.0

//This is a prototype version of a panel, copied from WGSubPanel
//THIS IS NOT INTENDED FOR USE... Yet.

Rectangle {
    id: mainPanel

    //General Options
    property string text: ""            //Panel Title
    property string subText: ""         //panel sub title, used to convey data ownership

    property Component childObject_     //object inside the panel. Use a layout for multiple controls eg. WGColumnLayout, WGFormLayout
    property Component headerObject_    //object inside the header. Use a layout for multiple controls eg. WGRowLayout
    property QtObject headerLabelObject_: headerLabel  //link to the text inside the header so it can be attached to a copyable

    property QtObject controlFilter

    //Best for Large solid color panels
    property bool collapsible_ : true
    property bool hasIcon_ : true

    property int expanded_ : 2
    property int pinnableChildren : 0
    property int pinnedChildren : 0

    property bool pinned: true

    property int panelDepth

    property bool choosePinned: false
    property bool chunkDragEnabled: false

    property string closedIcon_: "icons/arrow_right_16x16.png"
    property string openIcon_: "icons/arrow_down_16x16.png"

    /* Pinning probably belongs in the pimary parent panel
    property bool pinable_ : false
    property bool pinned_ : false

    property string pinIcon_: "icons/pin_16x16.png"
    property string pinnedIcon_: "icons/pinned_16x16.png"
    */

    property bool boldHeader_: true

    //sub header properties
    property bool italicSubHeader_: false
    property bool boldSubHeader_: false

    property bool transparentPanel_: false

    property color colorHeader_ : palette.lightPanelColor
    property color colorBody_ : palette.darkerShade

    //best for minor group box frames

    property bool hasSeparators_ : false    //Thin lines at top and bottom of frame
    property bool toggleable_ : false       //Make header a checkbox to enable/disable contents

    //Clips content of child objects such as ScrollPanel or similar childObject
    property bool clipContents_: true

    //Recommend not changing these properties:

    property int contentLeftMargin_ : defaultSpacing.doubleMargin
    property int contentRightMargin_ : defaultSpacing.doubleMargin
    property int contentIndent_ : 0

    property bool finishedLoading_: false

    Layout.fillWidth: true

    //clip: true  // Clips mainPanel text and subtext

    color: "transparent"

    radius: 2

    //if radius < 2, these panels look awful. This adds a bit more spacing.
    property int squareModifier: radius < 2 ? 8 : 0

    //Can have child panels indent further if set in global settings
    anchors.leftMargin: defaultSpacing.childIndentation

    height: {
        if (expanded_ > 0){
            content.height + defaultSpacing.minimumRowHeight + (radius * 4) + (defaultSpacing.topBottomMargin * 2) + squareModifier
        } else {
            defaultSpacing.minimumRowHeight + (radius * 2) + (squareModifier / 2) - 4
        }
    }

    Layout.preferredHeight: {
        if (expanded_ > 0){
            content.height + defaultSpacing.minimumRowHeight + (radius * 4) + squareModifier
        } else {
            defaultSpacing.minimumRowHeight + (radius * 2) + (squareModifier / 2) - 4
        }
    }

    onYChanged: {
        if(y < 0)
        {
            y = 0
        }
        else if(y > 1000 - height)
        {
            y = 1000 - height
        }
    }

    onXChanged: {
        if(x < 0)
        {
            x = 0
        }
        else if(x > 1000 - width)
        {
            x = 1000 - width
        }
    }

    function setRootPanel(parentObject){
        for(var i=0; i<parentObject.children.length; i++){
            if(typeof parentObject.children[i].rootPanel != "undefined"){
                if(parentObject.children[i].rootPanel == null){
                    if(parentObject.children[i] != mainPanel){

                        parentObject.children[i].rootPanel = mainPanel
                        parentObject.children[i].rootFrame = content.item

                        pinnedChildren += 1
                        pinnableChildren += 1

                        //console.log(mainPanel.text + " Root Chunk Found " + parentObject.children[i].text)
                    }
                }
            }
            setRootPanel(parentObject.children[i])
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

    signal showPinned()
    signal expanded()
    signal collapsed()

    //delay so panels don't animate when control is created
    Component.onCompleted: {
        animationDelay.start()
        if (transparentPanel_){
            colorHeader_ = "transparent"
            colorBody_ = "transparent"
            mainPanel.radius = 0
        }
        setRootPanel(mainPanel)
    }

    Timer {
        id: animationDelay
        interval: 100
        onTriggered: {
            finishedLoading_ = true
        }
    }

    //main panel behind body panel that forms the header and footer
    Rectangle {
        id: mainColor
        radius: mainPanel.radius
        color: colorHeader_
        anchors.fill: parent
        //anchors.topMargin: defaultSpacing.topBottomMargin
        //anchors.bottomMargin: defaultSpacing.topBottomMargin
        border.width: defaultSpacing.standardBorderSize
        border.color: palette.darkColor
    }

    Item {
        id: headerPanel
        anchors {left: parent.left; right: parent.right}

        anchors.margins: defaultSpacing.doubleBorderSize

        anchors.top: mainColor.top
        height: defaultSpacing.minimumRowHeight - 4

        Rectangle {
            //mouse over panel and activeFocus border

            id: mouseHighlight
            color: "transparent"
            anchors.fill: parent
            anchors.margins: defaultSpacing.doubleBorderSize

            activeFocusOnTab: collapsible_

            border.width: defaultSpacing.standardBorderSize
            border.color: activeFocus && collapsible_ ? palette.highlightShade : "transparent"

            Keys.onPressed: {
                 if (event.key == Qt.Key_Space && collapsible_){
                     if (expanded_ > 0 && collapsible_){
                         content.opacity = 0
                         expanded_ = 0;
                     } else if (expanded_ == 0 && collapsible_){
                         expanded_ = 2;
                         content.visible = true
                         content.opacity = 1;
                     }
                 }
             }
        }

        Text {
            id: expandIcon
            font.family : "Marlett"
            font.pixelSize: 12
            renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
            color: palette.textColor
            text : {
                if(expanded_ == 2)
                {
                    return "u"
                }
                else if(expanded_ == 1)
                {
                    return "v"
                }
                else if(expanded_ == 0)
                {
                    return "t"
                }
            }

            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.left: parent.left
            anchors.leftMargin: defaultSpacing.rowSpacing
        }


        //mouse area that expands the panel. Doesn't overlap any controls in the header
        MouseArea {
            id: expandMouseArea
            anchors.left: parent.left
            // anchors.right: headerObject_ ? headerControl.left : (pinable_ ? pinButton.left : parent.right)
            // Todo: I couldnt find a reference to panelMenu.left. Possible bug?
            anchors.right: headerObject_ ? headerControl.left : panelMenu.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: collapsible_

            //drag.target: mainPanel

            //enabled: !choosePinned && !globalSettings.dragLocked

            activeFocusOnTab: false

            hoverEnabled: true
            /*
            drag.onActiveChanged: {
                if(expandMouseArea.drag.active)
                {
                    draggingPanel = true
                    mainPanel.anchors.top = undefined
                    mainPanel.anchors.bottom = undefined
                    mainPanel.anchors.left = undefined
                    mainPanel.anchors.right = undefined
                }
                else
                {
                    draggingPanel = false
                }
            }

            onReleased: {
                mainPanel.Drag.drop()
                draggingPanel = false
                findAnchorPoint(mainPanel,mapToItem(null,mouseX,mouseY))
            }*/

            onDoubleClicked: {
                if (expanded_ == 2 && collapsible_ && pinnedChildren == pinnableChildren)
                {
                    content.opacity = 0
                    expanded_ = 0;
                    collapsed()
                }
                else if (expanded_ == 2 && collapsible_ && pinnedChildren != pinnableChildren)
                {
                    expanded_ = 1
                    showPinned()
                }
                else if (expanded_ == 0 && collapsible_)
                {
                    expanded_ = 2
                    content.visible = true
                    content.opacity = 1;
                    expanded()
                }
                else if (expanded_ == 1 && collapsible_)
                {
                    content.opacity = 0
                    expanded_ = 0;
                    collapsed()
                }
            }
        }

        //Panel title text or checkbox if toggleable

        Item {
            id: headerBox
            anchors.left: expandIcon.right
            anchors.leftMargin: collapsible_ ? defaultSpacing.standardMargin : 0
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: headerControl.left
            height: headerControl.height
            clip: true

            Text {
                id: headerLabel
                font.bold: boldHeader_
                font.pointSize: 9
                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                color: palette.textColor
                anchors.verticalCenter: parent.verticalCenter
                text: mainPanel.subText != "" ? mainPanel.text + ": " : mainPanel.text
                visible: toggleable_ ? false : true
            }

            Image {
                id: headerIcon
                visible: mainPanel.subText != ""
                width: hasIcon_ ? sourceSize.width : 0
                source: "icons/file_16x16.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: headerLabel.right
            }

            //SubPanel secondary title
            Text {
                id: headerSubLabel
                font.bold: boldSubHeader_
                font.italic: italicSubHeader_
                font.pointSize: 9
                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                color: palette.textColor
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: headerIcon.right
                text: mainPanel.subText
                visible: toggleable_ ? false : true
            }
        }

        //controls that can be placed in the header itself
        Loader {
            id: headerControl

            anchors.right: headerPanel.right
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: defaultSpacing.standardMargin
            anchors.rightMargin: defaultSpacing.standardMargin

            sourceComponent: headerObject_
        }


    }

    //loads child object.
    //Highly recommended first child object is a WGColumnLayout or similar
    Loader {
        id: content

        clip: clipContents_

        anchors {left: parent.left; right: parent.right}

        anchors.top: headerPanel.bottom
        anchors.topMargin: defaultSpacing.topBottomMargin * 2
        z: 1

        anchors.leftMargin: contentLeftMargin_ + contentIndent_
        anchors.rightMargin: contentRightMargin_

        sourceComponent: childObject_

        enabled: (expanded_ > 0)

        WGToolButton {
            z: 1
            height: 18
            width: 18
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -10
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: "icons/arrow_down_16x16.png"
            visible: expanded_ == 1
            onClicked: {
                expanded_ = 2
                content.visible = true
                content.opacity = 1;
                expanded()
            }
        }

    }

    //panel that contains the content.
    Rectangle {

        id: bodyPanel
        anchors {left: parent.left; right: parent.right}


        anchors.leftMargin: defaultSpacing.doubleBorderSize
        anchors.rightMargin: defaultSpacing.doubleBorderSize

        color: parent.colorBody_

        anchors.top: headerPanel.bottom

        height: parent.expanded_ > 0 ? content.height + 8 : 0

        WGSeparator {
            anchors {left: parent.left; right: parent.right}
            anchors.top: parent.top
            visible: mainPanel.expanded_ > 0
        }
    }
}
