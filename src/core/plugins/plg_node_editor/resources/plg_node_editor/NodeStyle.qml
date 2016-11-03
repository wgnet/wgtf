import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import WGControls 2.0
import WGControls.Layouts 2.0

/*! \brief Default node style for a Node Graph with selected states. */

Item
{
    id: nodeStyle
    WGComponent { type: "NodeStyle" }
    
    property QtObject node: parent

    property Component background: Rectangle {
        // the main background frame of the node
        id: nodeFrame
        color: palette.darkerShade
        radius: defaultSpacing.standardRadius

        border.width: node.selected ? defaultSpacing.doubleBorderSize : defaultSpacing.standardBorderSize
        border.color: node.selected ? palette.HighlightShade : palette.darkestShade
    }

    property Component titleBar: Rectangle {
        // the header at the top of the node

        id: titleBar
        color: Qt.rgba(palette.textBoxColor.r, palette.textBoxColor.g, palette.textBoxColor.b, 1.0)

        height: defaultSpacing.minimumRowHeight

        LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(parent.width * 0.8, 0)
            opacity: 0.8
            source: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: node.enabled ? node.nodeColor : "#999999"}
                GradientStop { position: 1.0; color: "#00000000" }
            }
        }

        radius: defaultSpacing.halfRadius
        z: -1
    }

    property Component label: WGExpandingRowLayout {
        // An icon, button or other object to the left of the node title
        Item {
            Layout.preferredHeight: 20
            Layout.preferredWidth: 20

            Image
            {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: -defaultSpacing.rowSpacing
                source: node.nodeIcon
                visible: node.nodeIcon != ""
            }
        }

        WGLabel
        {
            id : header
            Layout.fillHeight : true
            Layout.alignment : Qt.AlignHCenter | Qt.AlignVCenter
            text : node.nodeTitle
            font.bold: true
            font.italic: !node.enabled

            // Text.QtRendering looks a bit fuzzier sometimes but is much better for arbitrary scaling
            renderType: Text.QtRendering
        }

        // An icon, button or other object to the right of the node title
        Item {
            Layout.preferredHeight: 20
            Layout.preferredWidth: 20

            // This could be replaced by a popup menu button?
            WGToolButton
            {
                id: nodeExpandedBox
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: defaultSpacing.rowSpacing
                width: 20
                height: 20
                iconSource: node.nodeIsExpanded ? "images/arrow_down_small_16x16.png" : "images/arrow_right_small_16x16.png"
                onClicked: {
                    node.nodeIsExpanded = !node.nodeIsExpanded
                }
            }

        }
    }

    property Component subHeader: WGMultiLineText
    {
        id : subHeader
        text : node.nodeSubTitle
        color: palette.DisabledTextColor

        visible: node.nodeIsExpanded && node.nodeSubTitle != ""

        // Text.QtRendering looks a bit fuzzier but is much better for arbitrary scaling
        renderType: Text.QtRendering
    }
}
