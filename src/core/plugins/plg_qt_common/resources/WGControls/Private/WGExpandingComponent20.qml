import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0

/*!
    \brief A parent component for WGPropertyTree delegates that can expand to show an 'advanced' version,
    that can be positioned underneath it or replace the collapsed version entirely.

    TODO: Improve the click check detection in C++

Example:
\code{.js}

// Component that replaces the collapsed component with the expanded one
WGExpandingComponent {
    replace: true

    mainComponent: WGNumberBox {
        number: itemData.value
        minimumValue: itemData.minValue
        maximumValue: itemData.maxValue

        onValueChanged: {
            itemData.value = value
        }
    }

    expandedComponent: WGDial {
        width: defaultSpacing.minimumRowHeight * 4
        height: defaultSpacing.minimumRowHeight * 4
        value: itemData.value

        stepSize: itemData.stepSize
        decimals: itemData.decimals

        onValueChanged: {
            itemData.value = value
        }
    }
}

// Component that uses a single Thumbnail that resizes.
WGExpandingComponent {

    collapsedHeight: defaultSpacing.minimumRowHeight
    collapsedWidth: defaultSpacing.minimumRowHeight

    expandedHeight: defaultSpacing.minimumRowHeight * 4
    expandedWidth: defaultSpacing.minimumRowHeight * 4

    mainComponent: WGThumbnail {
        id: thumbnail
        objectName: "thumbnail"
        source: itemData.image
    }
}


\endcode
*/

Item {
    id: control
    objectName: "WGExpandingComponent"

    /*! The QtObject when the delegate is collapsed
    */
    property QtObject mainComponent: Rectangle {height: defaultSpacing.minimumRowHeight; width: height; color: "red"}

    /*! The QtObject when the delegate is expanded. If this is not explicitly set, the mainComponent will be resized to expandedHeight and expandedWidth instead of
        using a separate component.
    */
    property QtObject expandedComponent: mainComponent

    /*! Determines whether the main component is replaced (hidden) or if the expanded component appears underneath it.
        Only necessary if the expanded component is explicitly set.

      The default is false.
    */
    property bool replace: true

    /*! The gap between the main component and expanded component. Only relevant if replace == false
    */
    property alias verticalSpacing: mainLayout.spacing

    /*! Determines whether the component is expanded or not.
    */
    property bool expanded: false

    /*! The time in ms it takes for the component to grow and shrink on expanding.

      The default value is 120 ms.
    */
    property int expandGrowTime: 120

    /*! The delay in ms before the component expands or contracts

      The default value is 500 ms.
    */
    property int expandDelay: 400

    /*! Whether the component expands on hover or not.
    */
    property bool hoverExpand: true

    /*! The height of the mainComponent when collapsed. This is only needed if the expandedComponent is not explicitly set or does not have a height.

      The default value is the height of the mainComponent.
    */
    property int collapsedHeight: mainComponent.height

    /*! The width of the mainComponent when collapsed. This is only needed if the expandedComponent is not explicitly set or does not have a width.

      The default value is the width of the mainComponent.
    */
    property int collapsedWidth: mainComponent.width

    /*! The height of the mainComponent when expanded. This is only needed if the expandedComponent is not explicitly set or does not have a height.

      The default value is the height of the expandedComponent.
    */
    property int expandedHeight: expandedComponent.height

    /*! The width of the mainComponent when expanded. This is only needed if the expandedComponent is not explicitly set or does not have a width.

      The default value is the width of the expandedComponent.
    */
    property int expandedWidth: expandedComponent.width

    signal componentExpanding(var expanding)

    readonly property int totalExpHeight: mainComponent === expandedComponent ? expandedHeight
                                                                              : (replace ? expandedHeight : mainComponent.height + verticalSpacing + expandedHeight)
    height: expanded ? totalExpHeight + expanderBox.height : mainComponent.height + expanderBox.height
    width: parent.width

    function expand(expanding, sendSignal)
    {
        expanded = expanding
        if (mainComponent === expandedComponent)
        {
            if (expanded)
            {
                mainComponent.parent = expandedFrame
            }
            else
            {
                mainComponent.parent = mainFrame
            }
        }
        if (sendSignal) componentExpanding(expanding)
    }

    Behavior on height {
        id: popBehavior
        enabled: false
        NumberAnimation {
            id: popAnimation
            duration: expandGrowTime
            easing {
                type: Easing.OutCirc
                amplitude: 1.0
                period: 0.5
            }
        }
    }

    Component.onCompleted: {
        mainComponent.parent = mainFrame
        mainComponent.anchors.fill = Qt.binding(function() { return mainComponent.parent });
        if (expandedComponent !== mainComponent)
        {
            expandedComponent.parent = expandedFrame
        }
    }

    WGButtonFrame {
        id: expanderBox
        anchors.bottom: parent.bottom
        width: parent.width
        height: defaultSpacing.standardMargin
        radius: 0

        highlightColor: fillMouseOver.containsMouse ? palette.lightShade : "transparent"

        property int barProgress: 0
        property bool recentlyExpanded: false

        function easeInQuad (time, startVal, inc, duration)
        {
            time /= duration
            return inc * time * time + startVal
        }

        Rectangle {
            id: progressBar
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: defaultSpacing.doubleBorderSize
            anchors.bottomMargin: defaultSpacing.doubleBorderSize
            anchors.leftMargin: defaultSpacing.doubleBorderSize

            color: palette.lightestShade

            height: defaultSpacing.standardBorderSize
            width: 0
        }

        Text {
            id: arrowText
            color : palette.darkestShade

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            font.family : "Marlett"
            font.pixelSize: parent.height + defaultSpacing.doubleBorderSize + defaultSpacing.doubleBorderSize

            renderType: Text.QtRendering
            text : expanded ? "\uF074" : "\uF075"
        }

        MouseArea {
            id: fillMouseOver
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: control.height
            hoverEnabled: hoverExpand

            cursorShape: Qt.PointingHandCursor

            onPressed: {
                if (expandGrowTime > 0) popBehavior.enabled = true
                if (hoverEnabled) expanderBox.recentlyExpanded = true
                if (!popAnimation.running)
                {
                    expandTimer.stop()
                    progressBar.width = 0
                    expanderBox.barProgress = 0
                    expand(!expanded, true)
                }
            }

            onPositionChanged: {
                if (expandGrowTime > 0) {popBehavior.enabled = true}
                expanderBox.recentlyExpanded = false
            }
        }

        Timer {
            id: expandTimer
            interval: 10
            repeat: true
            running: (fillMouseOver.containsMouse || (!fillMouseOver.containsMouse && expanderBox.barProgress > 0)) && !expanderBox.recentlyExpanded && !expanded

            onTriggered: {
                if (fillMouseOver.containsMouse)
                {
                    expanderBox.barProgress += 10
                }
                else
                {
                    expanderBox.barProgress -= 10
                }

                progressBar.width = Math.min(expanderBox.easeInQuad(expanderBox.barProgress, 0, expanderBox.width - 2, expandDelay), expanderBox.width - 2)
            }
        }

        onBarProgressChanged: {
            if (progressBar.width >= expanderBox.width - 2 && !expanded)
            {
                expanderBox.recentlyExpanded = true
                progressBar.width = 0
                expanderBox.barProgress = 0
                expand(true, true)
            }
            if (progressBar.width >= expanderBox.width - 2 && expanded)
            {
                expanderBox.recentlyExpanded = true
                progressBar.width = 0
                expanderBox.barProgress = 0
                expand(false, true)
            }
        }
    }

    ColumnLayout {
        id: mainLayout
        spacing: replace ? 0 : defaultSpacing.rowSpacing
        width: expanded ? expandedWidth : collapsedWidth

        Item {
            id: mainFrame
            Layout.preferredWidth: collapsedWidth
            Layout.preferredHeight: collapsedHeight

            Layout.alignment: Qt.AlignTop | Qt.AlignLeft

            visible: replace && expanded ? false : true
        }

        Item {
            id: expandedFrame
            Layout.preferredWidth: expandedWidth
            Layout.preferredHeight: expandedHeight

            Layout.alignment: Qt.AlignTop | Qt.AlignLeft

            visible: control.expanded
        }
    }
}


