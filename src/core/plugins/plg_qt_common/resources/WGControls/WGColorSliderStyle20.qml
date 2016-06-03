import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1

/*!
    A slider style that contains a color gradient as the background
*/
WGSliderStyle {
    id: sliderStyle
    objectName: "WGColorSliderStyle"

    groove: Item {

        anchors.verticalCenter: parent.verticalCenter

        //changing between odd and even values causes pixel 'wiggling' as the center anchors move around.
        //can't use anchors.fill because the gradients need rotating
        implicitHeight: __horizontal ? control.height - control.height % 2 : control.width - control.width % 2
        implicitWidth: __horizontal ? control.width - control.width % 2 : control.height - control.height % 2

        WGTextBoxFrame {
            radius: defaultSpacing.halfRadius

            anchors.fill: parent

            color: "transparent"

            //grid pattern for transparent colors
            Image {
                source: "icons/bw_check_6x6.png"
                fillMode: Image.Tile
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardBorderSize
                z: -1
            }

            clip: true

            //Item that holds the gradient
            //QML can't make horizontal gradients so this is always vertical, then possibly rotated.

            Rectangle {
                objectName: "gradientFrame"
                id: gradientFrame
                anchors.centerIn: parent

                height: parent.width - 2
                width: parent.height - 2

                rotation: -90

                gradient: control.gradient
            }
        }
    }
    // There is no expanding bar that follows the handles in a ColorSlider
    bar: null
}
