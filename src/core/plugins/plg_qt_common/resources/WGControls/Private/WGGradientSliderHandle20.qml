import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

import WGControls.Private 2.0
/*!
    \ingroup wgcontrols
    \brief An arrow style slider handle for multi-handle Gradient WGColorSliders
*/

WGSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"

    property vector4d color: Qt.vector4d(1,1,1,1)

    property vector4d minColor: Qt.vector4d(1,1,1,1)

    property var tonemap: function(col) { return col; }

    property Gradient gradient: Gradient {
        id: gradient
        property color tm_Col: {
            var newCol = tonemap(Qt.vector3d(color.x, color.y, color.z))
            return Qt.rgba(newCol.x, newCol.y, newCol.z, color.w)
        }
        property color tm_MinCol: {
            var newCol = tonemap(Qt.vector3d(minColor.x, minColor.y, minColor.z))
            return Qt.rgba(newCol.x, newCol.y, newCol.z, color.w)
        }
        GradientStop {position: 0.0; color: gradient.tm_MinCol}
        GradientStop {position: 1.0; color: gradient.tm_Col}
    }

    function getIntPoint(a,b,percent)
    {
        return (a + (b - a) * percent)
    }

    function getInternalColor(pos) {

        var intCol = Qt.vector4d((getIntPoint(minColor.x, color.x, pos)),
                                 (getIntPoint(minColor.y, color.y, pos)),
                                 (getIntPoint(minColor.z, color.z, pos)),
                                 (getIntPoint(minColor.w, color.w, pos)))

        return(intCol)
    }

    handleHeight: __horizontal ? parentSlider.height - 2 : parentSlider.width - 2
    handleWidth: 15

    handleStyle:
        Item {
        implicitHeight: handleHeight
        implicitWidth: handleWidth
        Image {
            id: arrowHandleFrame
            source: "icons/arrow_handle.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -1
            anchors.bottom: parent.bottom

            Rectangle {
                id: colorSquare
                height: parent.width - 4
                width: parent.width - 4
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 2

                color: {
                    var tm_Col = tonemap(Qt.vector3d(sliderHandle.color.x, sliderHandle.color.y, sliderHandle.color.z))
                    return Qt.rgba(tm_Col.x, tm_Col.y, tm_Col.z, sliderHandle.color.w)
                }


                radius: handleIndex == parentSlider.__activeHandle && (parentSlider.activeFocus || parentSlider.colorPickerOpen) ? 5 : 0

                property real shadeMultiplier: handleIndex == parentSlider.__activeHandle && (parentSlider.activeFocus || parentSlider.colorPickerOpen) ? 1.4 : 1.2

                border.width: 1
                border.color: Qt.darker(Qt.rgba(colorSquare.color.r, colorSquare.color.g, colorSquare.color.b, 1), shadeMultiplier)

                Component.onCompleted: {
                    parentSlider.handleVerticalOffset = Math.round(defaultSpacing.minimumRowHeight / 6)
                }
            }
        }

        ColorOverlay {
            anchors.fill: arrowHandleFrame
            source: arrowHandleFrame
            color: palette.darkestShade
            visible: !parentSlider.enabled
        }
    }
}
