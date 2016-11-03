import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls.Styles 2.0
import WGControls.Private 2.0

/*!
    \ingroup wgcontrols
    \brief A slider style to hold keyframe handles
*/
WGSliderStyle {
    id: sliderStyle
    objectName: "WGTimelineBarSliderStyle"
    WGComponent { type: "WGTimelineFrameSliderStyle20" }

    groove: Item {

        anchors.verticalCenter: parent.verticalCenter

        implicitHeight: __horizontal ? control.height : control.width
        implicitWidth: __horizontal ? control.width : control.height
    }

    bar: null
}
