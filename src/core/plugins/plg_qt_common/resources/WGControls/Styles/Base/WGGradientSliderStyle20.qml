import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Styles 2.0

/*!
    \ingroup wgcontrols
    \brief A slider style that contains a color gradient as the background
*/
WGSliderStyle {
    id: sliderStyle
    objectName: "WGColorSliderStyle"
    WGComponent { type: "WGGradientSliderStyle20" }

    groove: Item {

        anchors.verticalCenter: parent.verticalCenter

        //changing between odd and even values causes pixel 'wiggling' as the center anchors move around.
        //can't use anchors.fill because the gradients need rotating

        implicitHeight: __horizontal ? control.height - control.height % 2 : control.width - control.width % 2
        implicitWidth: __horizontal ? control.width - control.width % 2 : control.height - control.height % 2


        WGTextBoxFrame {
            radius: defaultSpacing.halfRadius

            anchors.fill: parent
            anchors.bottomMargin: control.handleVerticalOffset

            color: "transparent"

            clip: true

            Loader {
                objectName: "gradientFrame"
                id: gradientFrame
                sourceComponent: control.gradientFrame

                anchors.centerIn: parent

                height: parent.width % 2 == 0 ? parent.width + 2 : parent.width + 2 + (parent.width % 2)
                width: parent.height % 2 == 0 ? parent.height - 2 : parent.width - (parent.height % 2)

                rotation: -90

                //grid pattern for transparent colors
                Image {
                    source: "../icons/bw_check_6x6.png"
                    fillMode: Image.Tile
                    anchors.fill: parent

                    z: -1
                }
            }
        }
    }
    // There is no expanding bar that follows the handles in a ColorSlider
    bar: null
}
