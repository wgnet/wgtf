/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import WGControls 2.0

/*!
    \ingroup wgcontrols
    \brief A re-write of the default Slider style.

    Example:
    \code{.js}
    Slider {
        anchors.centerIn: parent
        style: SliderStyle {
            groove: Rectangle {
                implicitWidth: 200
                implicitHeight: 8
                color: "gray"
                radius: 8
            }
        }
    }
    \endcode
*/
Style {
    objectName: "WGSliderStyle"
    id: styleitem
    WGComponent { type: "WGSliderStyle20" }

    /*! The \l Slider this style is attached to. */
    readonly property QtObject control: __control

    property bool __horizontal: control.__horizontal

    property int vertPadding: 0
    property int horzPadding: 0

    property real __clampedLength: control.__clampedLength

    property int __multiValueDashSpacing: 9

    padding { top: vertPadding ; left: horzPadding ; right: horzPadding ; bottom: vertPadding }

    property Component groove: Item {

        anchors.verticalCenter: parent.verticalCenter

        implicitWidth: Math.round(defaultSpacing.minimumRowHeight / 4)
        implicitHeight: Math.round(defaultSpacing.minimumRowHeight / 4)

        WGTextBoxFrame {
            id: grooveFrame
            radius: defaultSpacing.standardRadius
            anchors.fill: parent
            color: control.enabled ? palette.textBoxColor : "transparent"

            Rectangle {
                id: multivalueStyling
                color: "transparent"
                visible: control.multipleValues
                anchors.fill: parent
                anchors.margins: parent.border.width
                radius: parent.radius


                Row {
                    Repeater {
                        id: multipleValueRepeater

                        model: grooveFrame.width / __multiValueDashSpacing
                        Item {
                            height: multivalueStyling.height
                            width: __multiValueDashSpacing
                            Rectangle {
                                id: dash
                                color: palette.highlightColor
                                height: multivalueStyling.height
                                width: 3
                            }
                        }
                    }
                }
            }
        }
    }

    /*! This property holds the coloured bar of the slider.
    */
    property Component bar: Item {
        property color fillColor: control.multipleValues ? "transparent" : control.__handlePosList[barid].barColor
        clip: true
        Rectangle {
            clip: true
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardBorderSize
            border.color: control.multipleValues ? "transparent" : control.enabled ? Qt.darker(fillColor, 1.2) : palette.lighterShade
            radius: defaultSpacing.halfRadius
            color: control.enabled ? fillColor : palette.lightShade
        }
    }

    /*! This property holds the tick mark labels
    */

    property Component tickmarks: Repeater {
        id: repeater
        model: control.stepSize > 0 ? 1 + (control.maximumValue - control.minimumValue) / control.stepSize : 0
        WGSeparator {
            vertical: true
            width: defaultSpacing.separatorWidth
            height: defaultSpacing.standardMargin

            x: control.__handleWidth / 2 + index * ((repeater.width - control.__handleWidth) / (repeater.count-1)) - (defaultSpacing.separatorWidth / 2)
            y: defaultSpacing.doubleMargin
        }
    }

    /*! This property holds the slider style panel.

        Note that it is generally not recommended to override this.
    */
    property Component panel: Item {
        id: root

        implicitWidth: control.width
        implicitHeight: control.height

        anchors.centerIn: parent

        Item {
            objectName: "sliderFrame"
            id: sliderFrame
            anchors.centerIn: parent

            height: __horizontal ? control.height : control.width
            width: __horizontal ? control.width : control.height

            rotation: __horizontal ? 0 : -90
            transformOrigin: Item.Center

            Loader {
                id: grooveLoader
                sourceComponent: groove

                width: parent.width - padding.left - padding.right

                anchors.horizontalCenter: parent.horizontalCenter

                height: groove.implicitHeight

                x: {
                    if(control.groovePadding)
                    {
                        padding.left
                    }
                    else
                    {
                        0
                    }
                }

                y: {
                    if(control.groovePadding)
                    {
                        padding.top + (parent.height - grooveLoader.item.height)/2
                    }
                    else
                    {
                        (parent.height - grooveLoader.item.height)/2
                    }
                }

                Repeater {
                model: control.__handleCount
                    Loader {
                        id: barLoader
                        sourceComponent: bar
                        property int barid: index
                        visible: control.__handlePosList[index].showBar

                        anchors.verticalCenter: grooveLoader.verticalCenter

                        property int barClampPadding: control.handleClamp ? control.__visualMinPos : 0

                        height: grooveLoader.height
                        width: Math.round((((control.__handlePosList[index].value - control.minimumValue) / (control.maximumValue - control.minimumValue)) * __clampedLength) + control.__visualMinPos - control.__handlePosList[index].barMinPos)

                        x: control.__handlePosList[index].barMinPos
                        z: 1
                    }
                }
            }

            Loader {
                id: tickMarkLoader
                anchors.centerIn: parent
                height: control.height
                width: control.width
                sourceComponent: control.tickmarksEnabled ? tickmarks : null
            }

            Repeater {
                model: control.__handleCount

                Loader {
                    id: handleLoader

                    property int handleIndex: index

                    property int handleOffset: control.__handlePosList[index].handleOffset

                    sourceComponent: control.__handlePosList[index].handleStyle

                    anchors.verticalCenter: grooveLoader.verticalCenter

                    x: Math.round((((control.__handlePosList[index].value - control.minimumValue) / (control.maximumValue - control.minimumValue)) * __clampedLength) + control.__visualMinPos + handleOffset)

                    onLoaded: {
                        control.__handleHeight = handleLoader.implicitHeight
                        control.__handleWidth = handleLoader.implicitWidth
                    }

                    Connections {
                        target: control.__handlePosList[index]
                        onValueChanged: {
                            control.changeValue(control.__handlePosList[index].value, index)
                        }
                    }

                    MouseArea {
                        objectName: "sliderHandleArea"
                        hoverEnabled: true
                        anchors.fill: parent

                        propagateComposedEvents: true

                        cursorShape: control.__currentCursor

                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onEntered: {
                            control.hoveredHandle = index
                        }

                        onExited: {
                            if (control.hoveredHandle == index)
                            {
                               control.hoveredHandle = -1
                            }
                        }

                        onPressed: {
                            control.__activeHandle = index
                            control.forceActiveFocus()

                            if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
                            {
                                control.__draggable = false
                            }
                            else if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier))
                            {
                                control.__draggable = false
                            }
                            else
                            {
                                if (!control.grooveClickable)
                                {
                                    control.__draggable = true
                                }
                            }
                            control.handleClicked(index, mouse.button, mouse.modifiers)
                            if (mouse.button == Qt.LeftButton)
                            {
                                mouse.accepted = false
                            }
                        }

                        onReleased: {
                            control.__draggable = control.grooveClickable
                        }
                    }
                }
            }
        }
    }
}
