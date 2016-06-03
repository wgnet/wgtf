import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0

/*!
 \An invisible Slider handle intended for the WGTimelineBarSlider Control.

 Shouldn't really be needed to be used on it's own but may be useful in some other kind of slider.
*/

WGRangeSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"

    handleOffset: maxHandle ? -2 : -defaultSpacing.doubleMargin + 2

    hoverCursor: Qt.SizeHorCursor
    dragCursor: Qt.SizeHorCursor

    property bool selected: false

    property bool handleDragging: false

    property Component handleStyle: Item {
        implicitWidth: defaultSpacing.doubleMargin
        implicitHeight: defaultSpacing.minimumRowHeight
    }
}

