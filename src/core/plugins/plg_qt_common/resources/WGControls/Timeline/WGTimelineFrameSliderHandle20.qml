import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Private 2.0

/*!
    \ingroup wgcontrols
    \brief An invisible Slider handle intended for the WGTimelineBarSlider Control.
     Shouldn't really be needed to be used on it's own but may be useful in some other kind of slider.
*/

WGSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"
    WGComponent { type: "WGTimelineFrameSliderHandle20" }

    /*!
        This determines what type of data the keyframe represents and how the data is changed/eased. Current options are ("constant", "linear", "bezier")

        Currently this only affects the handle shape. (square, diamond, circle)
    */
    property string frameType

    /*!
        If the handle is selected or not
    */
    property bool selected: false

    /*!
        If this keyframe is explicitly being dragged by the user handleDragging == true. (If it is being dragged as part of a multi-selection this will be false.)
    */
    property bool handleDragging: false

    /*!
        A string of text that appears next to the keyframe.
    */
    property string label: ""

    /*!
        If currentFrame == this handles value this will be true.
    */
    property bool keyframeActive: false

    property Component handleStyle: Item {

        implicitWidth: defaultSpacing.minimumRowHeight
        implicitHeight: defaultSpacing.minimumRowHeight

        Rectangle {
            anchors.centerIn: parent
            width: 2
            height: parent.height
            color: parentSlider.barColor
        }

        Rectangle {
            id: keyframeWidget
            anchors.centerIn: parent

            height: parent.height - defaultSpacing.doubleMargin
            width: parent.width - defaultSpacing.doubleMargin

            border.width: 1
            border.color: selected ? palette.highlightColor : "transparent"

            color: "#CCCCCC"

            transformOrigin: Item.Center
            rotation: frameType == "linear" ? 45 : 0

            radius: frameType == "bezier" ? height : 0
            clip: true

            Rectangle {
                id: activeHighlight
                anchors.fill: parent
                anchors.margins: 2
                radius: parent.radius
                color: "red"

                visible: true
                opacity: 0

                states: [
                    State {
                        name: "INACTIVE"
                        when: !sliderHandle.keyframeActive
                        PropertyChanges { target: activeHighlight; opacity: 0.0}
                    },
                    State {
                        name: "ACTIVE"
                        when: sliderHandle.keyframeActive
                        PropertyChanges { target: activeHighlight; opacity: 1.0}
                    }

                ]
                transitions: [
                    Transition {
                        from: "ACTIVE"
                        to: "INACTIVE"
                        NumberAnimation { target: activeHighlight; properties: "opacity"; duration: 800}
                    }
                ]
            }
        }

        WGLabel {
            anchors.left: keyframeWidget.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: defaultSpacing.standardMargin

            visible: parentSlider.showLabel
            text: label

            color: selected ? palette.highlightColor : palette.textColor
        }
    }

    Connections {
        target: parentSlider

        onDragSelectedHandles: {
            if (sliderHandle.selected && !handleDragging)
            {
                sliderHandle.value = parentSlider.initialValues[__handlePosList.indexOf(sliderHandle)] + delta
            }
        }
    }
}

