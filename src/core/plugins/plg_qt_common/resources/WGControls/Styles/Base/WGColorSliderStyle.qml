import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls 1.0
import WGControls.Styles 1.0

/*!
    \ingroup wgcontrols
    \brief A slider style that contains a color gradient as the background
*/
WGSliderStyle {
    id: sliderStyle
    objectName: "WGColorSliderStyle"
    WGComponent { type: "WGColorSliderStyle" }

    groove: Item {

        anchors.verticalCenter: parent.verticalCenter

        //changing between odd and even values causes pixel 'wiggling' as the center anchors move around.
        //can't use anchors.fill because the gradients need rotating

        implicitHeight: __horizontal ? control.height - control.height % 2 : control.width - control.width % 2
        implicitWidth: __horizontal ? control.width - control.width % 2 : control.height - control.height % 2

        WGTextBoxFrame {
            radius: defaultSpacing.halfRadius

            anchors.centerIn: parent
            anchors.verticalCenterOffset: (control.handleVerticalOffset / 2) * -1

            height: parent.width
            width: parent.height - control.handleVerticalOffset

            rotation: -90

            color: "transparent"

            //grid pattern for transparent colors

            clip: true

            Image {
                source: "../icons/bw_check_6x6.png"
                fillMode: Image.Tile
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardBorderSize
                z: -1
            }

            //Item that holds the gradient
            //QML can't make horizontal gradients so this is always vertical, then possibly rotated.

            Item {
                objectName: "gradientFrame"
                id: gradientFrame

                anchors.fill: parent
                anchors.leftMargin: defaultSpacing.standardBorderSize
                anchors.rightMargin: defaultSpacing.standardBorderSize

                height:__horizontal ? parent.width - 2 : parent.height - 2
                width: __horizontal ? parent.height - 2 : parent.width - 2

                clip: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Repeater {
                        model: control.__colorBarModel

                        Rectangle
                        {
                            id: colorBar

                            property color minColor: typeof control.colorData[minColorVal] != "undefined" ? control.colorData[minColorVal] : "white"
                            property color maxColor: typeof control.colorData[maxColorVal] != "undefined" ? control.colorData[maxColorVal] : "white"

                            Connections {
                                target: control
                                onUpdateColorBars: {
                                    colorBar.minColor = control.colorData[minColorVal]
                                    colorBar.maxColor = control.colorData[maxColorVal]
                                }
                            }

                            Layout.fillWidth: true

                            property real minPos: {
                                if(control.linkColorsToHandles) {
                                    if (index == 0 || !control.__barLoaded || control.__handlePosList.children.length == 0)
                                    {
                                        0
                                    }
                                    else
                                    {
                                        Math.floor(control.__handlePosList.children[index - 1].range.position)
                                    }
                                }
                                else
                                {
                                    0
                                }
                            }

                            property real maxPos: {
                                if(control.linkColorsToHandles) {
                                    if (index == control.__colorBarModel.count - 1 || !control.__barLoaded || control.__handlePosList.children.length == 0)
                                    {
                                        gradientFrame.height
                                    }
                                    else
                                    {
                                        Math.floor(control.__handlePosList.children[index].range.position)
                                    }
                                }
                                else
                                {
                                    0
                                }
                            }

                            Layout.preferredHeight: {
                                if (control.linkColorsToHandles)
                                {
                                    maxPos - minPos
                                }
                                else
                                {
                                    (maxValue - minValue) * (gradientFrame.height / (control.maximumValue - control.minimumValue))
                                }
                            }

                            MouseArea
                            {
                                anchors.fill: parent
                                propagateComposedEvents: true

                                // Workaround for crash during Shift+Click, this colorBar may no longer be associated with the parent
                                // Without this when the event is propagated an attempt to access the null window crashes the application
                                onPressAndHold: { mouse.accepted = true}

                                onPressed: {
                                    //adds handles when bar is Shift Clicked
                                    if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier) && control.addDeleteHandles)
                                    {
                                        control.__draggable = false

                                        //find the right color inside the clicked bar
                                        function getIntPoint(a,b,percent)
                                        {
                                            return (a + (b - a) * percent)
                                        }

                                        var barPoint = mouseY / colorBar.height
                                        var newColor = Qt.rgba((getIntPoint(minColor.r, maxColor.r,barPoint)),(getIntPoint(minColor.g, maxColor.g,barPoint)),(getIntPoint(minColor.b, maxColor.b,barPoint)),(getIntPoint(minColor.a, maxColor.a,barPoint)))

                                        //get the position of the mouse inside the entire slider
                                        var mousePos = mapToItem(gradientFrame, mouseX, mouseY)
                                        var newPos = mousePos.y / (gradientFrame.height / (control.maximumValue - control.minimumValue))

                                        //add a new point to the data
                                        control.addData(index, newPos, newColor)
                                    }
                                    else if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier) && control.addDeleteHandles)
                                    {
                                        control.__draggable = false
                                        mouse.accepted = false
                                    }
                                    else
                                    {
                                        mouse.accepted = false
                                    }
                                }
                                onReleased: {
                                    control.__draggable = true
                                }
                            }

                            gradient: Gradient {
                                GradientStop {
                                    position: 0
                                    color: minColor
                                }
                                GradientStop {
                                    position: 1
                                    color: maxColor
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // There is no expanding bar that follows the handles in a ColorSlider
    bar: null
}
