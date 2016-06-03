import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1

/*!
    A slider style loads a bar based off control.barContent.

    Defaults to a colored rectangle.
*/
WGSliderStyle {
    id: sliderStyle
    objectName: "WGTimelineBarSliderStyle"

    groove: Item {

        anchors.verticalCenter: parent.verticalCenter

        implicitHeight: __horizontal ? control.height : control.width
        implicitWidth: __horizontal ? control.width : control.height
    }

    bar: Item {
        Loader
        {
            sourceComponent: control.barContent
            clip: true

            anchors.fill: parent

            Component.onCompleted: {
                control.barArea = item
            }
        }
    }
}
