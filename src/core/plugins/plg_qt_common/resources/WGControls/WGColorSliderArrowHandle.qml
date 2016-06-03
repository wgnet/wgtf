import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import WGControls 1.0


/*!
    \brief An arrow style slider handle for multi-handle Gradient WGColorSliders

    DEPRECATED - This control has been replaced by WGGradientSliderHandle in WGControls 2.0
*/

WGColorSliderHandle {
    objectName: "WGColorSliderArrowHandle"
    handleStyle:
        Item {
        implicitHeight: parentSlider.__horizontal ? parentSlider.height - 2 : 11
        implicitWidth: parentSlider.__horizontal ? 11 : parentSlider.width - 2
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

                color: parentSlider.colorData[handleIndex]

                radius: handleIndex == parentSlider.__activeHandle ? 5 : 0

                border.width: 1
                border.color: Qt.darker(colorSquare.color, 1.2)

                Component.onCompleted: {
                    parentSlider.handleVerticalOffset = 4
                }

                Connections {
                    target: parentSlider
                    onUpdateColorBars : {
                        colorSquare.color = parentSlider.colorData[handleIndex]
                    }
                }
            }
        }
    }
}
