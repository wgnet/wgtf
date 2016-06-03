import QtQuick 2.3

/*!
 \brief A scrollbar used within WGScrollPanel.
 The scrollbar acts as both a flickable position indicator and a conventional click and drag scrollbar.
 When the mouse cursor is positioned over the flickable indicator a conventional scrollbar is revealed.
*/

Item {
    id: scrollBar
    objectName: "WGScrollBar"

    /*! The properties that define the scrollbar's state.
        position and pageSize are in the range 0.0 - 1.0.  They are relative to the
        height of the page, i.e. a pageSize of 0.5 means that you can see 50%
        of the height of the view.
    */
    property real position

    /*! The properties that define the scrollbar's state.
        position and pageSize are in the range 0.0 - 1.0.  They are relative to the
        height of the page, i.e. a pageSize of 0.5 means that you can see 50%
        of the height of the view.
    */
    property real pageSize

    /*! This property orientation can be either Qt.Vertical or Qt.Horizontal */
    property variant orientation : Qt.Vertical

    /*! This property determines whether the scrollbar will expand and shrink when interacted with. It also
        enables and disables hoverEnabled for the mousearea. */
    property bool expandableScrollBar: true

    // Is the scrollbar in the flickable indicator state or conventional scrollbar state
    /*! \internal */
    property bool __expanded: !expandableScrollBar

    property QtObject scrollFlickable

    property QtObject scrollBarStyle: WGScrollBarStyle{}

    /*! \internal */
    property int __scrollBarWidth: __expanded ? __expandedWidth : __collapsedWidth

    /*! \internal */
    property int __expandedWidth: defaultSpacing.scrollBarSize * 3

    /*! \internal */
    property int __collapsedWidth: defaultSpacing.scrollBarSize

    //keep the draghandle within the proper bounds
    function resetBounds(){
        if (orientation == Qt.Vertical)
        {
            if (dragHandle.y < 0)
            {
                dragHandle.y = 0
            }
            if (dragHandle.y > background.height - dragHandle.height)
            {
                dragHandle.y = background.height - dragHandle.height
            }
            if (scrollFlickable.contentY < 0)
            {
                scrollFlickable.contentY = 0
            }
            if (scrollFlickable.contentY > (scrollFlickable.contentHeight - scrollFlickable.height))
            {
                scrollFlickable.contentY = scrollFlickable.contentHeight - scrollFlickable.height
            }
        }
        else
        {
            if (dragHandle.x < 0)
            {
                dragHandle.x = 0
            }
            if (dragHandle.x > background.width - dragHandle.width)
            {
                dragHandle.x = background.width - dragHandle.width
            }
            if(scrollFlickable.contentX < 0)
            {
                scrollFlickable.contentX = 0
            }
            if(scrollFlickable.contentX > (scrollFlickable.contentWidth - scrollFlickable.width))
            {
                scrollFlickable.contentX = scrollFlickable.contentWidth - scrollFlickable.width
            }
        }
    }

    //short grow/shrink animation for scrollbar
    Behavior on __scrollBarWidth{
        id: barAnimation
        NumberAnimation {
            duration: 40
            easing {
                type: Easing.OutCirc
                amplitude: 1.0
                period: 0.5
            }
        }
    }

    Loader {
        id: background

        sourceComponent: scrollBarStyle.scrollGroove

        anchors.top: orientation == Qt.Vertical ? parent.top : undefined
        anchors.bottom: parent.bottom

        anchors.left: orientation == Qt.Vertical ? undefined : parent.left
        anchors.right: parent.right

        anchors.rightMargin: orientation == Qt.Vertical ? defaultSpacing.standardBorderSize : 0
        anchors.bottomMargin: orientation == Qt.Vertical ? 0 : defaultSpacing.standardBorderSize

        width: orientation == Qt.Vertical ? __scrollBarWidth : undefined
        height: orientation == Qt.Vertical ? undefined : __scrollBarWidth
    }

    // Size the bar to the required size, depending upon the orientation.
    Loader {
        id: handle

        sourceComponent: scrollBarStyle.scrollHandle

        x: orientation == Qt.Vertical ? 0 : (scrollBar.position * (scrollBar.width))
        y: orientation == Qt.Vertical ? (scrollBar.position * (scrollBar.height)) : 0

        width: orientation == Qt.Vertical ? undefined: (scrollBar.pageSize * scrollBar.width)
        height: orientation == Qt.Vertical ? (scrollBar.pageSize * scrollBar.height) : undefined

        anchors.left: orientation == Qt.Vertical ? background.left : undefined
        anchors.right: orientation == Qt.Vertical ? background.right : undefined

        anchors.top: orientation == Qt.Vertical ? undefined : background.top
        anchors.bottom: orientation == Qt.Vertical ? undefined : background.bottom
    }

    //mouse area for clicking above/below handle and dragging the handle itself.
     MouseArea {
         id: scrollBarArea
         anchors.fill: parent
         anchors.leftMargin: orientation == Qt.Vertical && !__expanded ? -5 : 0
         anchors.topMargin: orientation != Qt.Vertical && !__expanded ? -5 : 0

         hoverEnabled: expandableScrollBar

         onEntered: {
             if (expandableScrollBar)
             {
                 __expanded = true
             }
         }

         onExited: {
             if (!scrollBarArea.drag.active && expandableScrollBar)
             {
                 shrinkDelay.restart()
             }
         }

        //short delay so bar doesn't shrink instantly
         Timer{
             id: shrinkDelay
             interval: 600
             onTriggered: {
                 if (!scrollBarArea.drag.active && !scrollBarArea.containsMouse && expandableScrollBar)
                 {
                     __expanded = false
                 }
             }
         }

         //if click above or below handle, jump 1 handle's height up or down.
         onPressed: {
             var mouseBarPos = scrollBarArea.mapToItem(background, mouse.x, mouse.y)

             if (orientation == Qt.Vertical)
             {
                 if (mouseBarPos.y < handle.y)
                 {
                     if (scrollFlickable.contentY > (scrollFlickable.contentHeight - scrollFlickable.height) * (scrollFlickable.height / scrollFlickable.contentHeight))
                     {
                         scrollFlickable.contentY -= (scrollFlickable.contentHeight - scrollFlickable.height) * (scrollFlickable.height / scrollFlickable.contentHeight)
                     }
                     else
                     {
                         scrollFlickable.contentY = 0
                     }
                 }
                 else if (mouseBarPos.y > handle.y + handle.height)
                 {
                     if (scrollFlickable.contentY < (scrollFlickable.contentHeight - scrollFlickable.height) * (1 - (scrollFlickable.height / scrollFlickable.contentHeight)))
                     {
                         scrollFlickable.contentY += (scrollFlickable.contentHeight - scrollFlickable.height) * (scrollFlickable.height / scrollFlickable.contentHeight)
                     }
                     else
                     {
                         scrollFlickable.contentY = (scrollFlickable.contentHeight - scrollFlickable.height)
                     }
                 }
             }
             else if (orientation == Qt.Horizontal)
             {
                 if(mouseBarPos.x < handle.x)
                 {
                     if(scrollFlickable.contentX > (scrollFlickable.contentWidth - scrollFlickable.width) * (scrollFlickable.width / scrollFlickable.contentWidth))
                     {
                         scrollFlickable.contentX -= (scrollFlickable.contentWidth - scrollFlickable.width) * (scrollFlickable.width / scrollFlickable.contentWidth)
                     }
                     else
                     {
                         scrollFlickable.contentX = 0
                     }
                 }
                 else if (mouseBarPos.x > handle.x + handle.width)
                 {
                     if (scrollFlickable.contentX < (scrollFlickable.contentWidth - scrollFlickable.width) * (1 - (scrollFlickable.width / scrollFlickable.contentWidth)))
                     {
                         scrollFlickable.contentX += (scrollFlickable.contentWidth - scrollFlickable.width) * (scrollFlickable.width / scrollFlickable.contentWidth)
                     }
                     else
                     {
                          scrollFlickable.contentX = (scrollFlickable.contentWidth - scrollFlickable.width)
                     }
                 }
             }
         }

         onReleased:
         {
             if (!scrollBarArea.containsMouse && expandableScrollBar)
             {
                 shrinkDelay.restart()
             }
         }

         //grab the fake drag handle
         drag.target: dragHandle
         drag.axis: orientation == Qt.Vertical ? Drag.YAxis : Drag.XAxis

         drag.onActiveChanged: {
             if (scrollBarArea.drag.active)
             {
                 if(orientation == Qt.Vertical)
                 {
                     dragHandle.anchors.verticalCenter = undefined
                 }
                 else
                 {
                     dragHandle.anchors.horizontalCenter = undefined
                 }
             }
             else
             {
                 dragHandle.anchors.verticalCenter = handle.verticalCenter
                 dragHandle.anchors.horizontalCenter = handle.horizontalCenter
             }
         }
     }

    //Fake drag handle as the actual handle needs to get its position from the flickable which causes binding issues.
    Rectangle {
        id: dragHandle
        objectName: "dragHandle"
        color: "transparent"
        anchors.verticalCenter: handle.verticalCenter
        anchors.horizontalCenter: handle.horizontalCenter

        height: handle.height
        width: handle.width

        Drag.active: scrollBarArea.drag.active

        Drag.onActiveChanged: {
            if (!Drag.active)
            {
                resetBounds()
            }
        }

        onYChanged:{
            if (Drag.active)
            {
                resetBounds()

                //keep the content within the proper bounds
                if (scrollFlickable.contentY >= 0 && scrollFlickable.contentY <= (scrollFlickable.contentHeight - scrollFlickable.height))
                {
                    //make the relative position of the content match the relative position of the draghandle
                    scrollFlickable.contentY = (dragHandle.y / (background.height - handle.height)) * (scrollFlickable.contentHeight - scrollFlickable.height)
                }
                else
                {
                    resetBounds()
                }
            }
        }

        onXChanged: {
            if (Drag.active)
            {
                resetBounds()

                //keep the content within the proper bounds
                if (scrollFlickable.contentX >= 0 && scrollFlickable.contentX <= (scrollFlickable.contentWidth - scrollFlickable.width))
                {
                    //make the relative position of the content match the relative position of the draghandle
                    scrollFlickable.contentX = (dragHandle.x / (background.width - handle.width)) * (scrollFlickable.contentWidth - scrollFlickable.width)
                }
            }
        }
    }

    /*! Deprecated */
    property alias expanded: scrollBar.__expanded

    /*! Deprecated */
    property alias expandedWidth: scrollBar.__expandedWidth

    /*! Deprecated */
    property alias scrollBarWidth: scrollBar.__scrollBarWidth

    /*! Deprecated */
    property alias collapsedWidth: scrollBar.__collapsedWidth
}
