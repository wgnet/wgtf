import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2


/*!
    \brief An arrow style slider handle for multi-handle Gradient WGColorSliders
*/

WGSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"

    property color color: "#FFFFFF"

    property color minColor: "#FFFFFF"

    property Gradient gradient: Gradient {
        id: gradient
        GradientStop {position: 0.0; color: sliderHandle.minColor}
        GradientStop {position: 1.0; color: sliderHandle.color}
    }

    function getIntPoint(a,b,percent)
    {
        return (a + (b - a) * percent)
    }

    function getInternalColor(pos) {

        var newCol = "#FF00FF"
        var minCol = "#000000"
        var maxCol = "#FFFFFF"

        for (var i = 0; i < gradient.stops.length; i++)
        {
            if (gradient.stops[i].position == pos)
            {
                newCol = gradient.stops[i].color
                i = gradient.stops.length
            }
            else if (gradient.stops[i].position < pos)
            {
                minCol = gradient.stops[i].color
            }
            else
            {
                maxCol = gradient.stops[i].color
            }
            newCol = Qt.rgba((getIntPoint(minCol.r, maxCol.r,pos)),
                           (getIntPoint(minCol.g, maxCol.g,pos)),
                           (getIntPoint(minCol.b, maxCol.b,pos)),
                           (getIntPoint(minCol.a, maxCol.a,pos)))
        }
        return(newCol)
    }

    handleStyle:
        Item {
        implicitHeight: __horizontal ? parentSlider.height - 2 : parentSlider.width - 2
        implicitWidth: 15
        Image {
            id: arrowHandleFrame
            source: "icons/arrow_handle.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom

            Rectangle {
                id: colorSquare
                height: parent.width - 4
                width: parent.width - 4
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 2

                color: sliderHandle.color

                radius: handleIndex == parentSlider.__activeHandle ? 5 : 0

                border.width: 1
                border.color: Qt.darker(colorSquare.color, 1.2)

                Component.onCompleted: {
                    parentSlider.handleVerticalOffset = 4
                }
            }
        }
    }
}
