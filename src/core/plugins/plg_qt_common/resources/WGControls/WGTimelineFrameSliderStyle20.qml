import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1

/*!
    A slider style to hold keyframe handles
*/
WGSliderStyle {
    id: sliderStyle
    objectName: "WGTimelineBarSliderStyle"

    groove: Item {

        anchors.verticalCenter: parent.verticalCenter

        implicitHeight: __horizontal ? control.height : control.width
        implicitWidth: __horizontal ? control.width : control.height
    }

    bar: null
}
