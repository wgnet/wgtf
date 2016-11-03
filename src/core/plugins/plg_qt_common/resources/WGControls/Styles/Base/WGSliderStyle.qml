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

import WGControls 1.0
import WGControls.Private 1.0

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
    WGComponent { type: "WGSliderStyle" }

    /*! The \l Slider this style is attached to. */
    readonly property WGSlider control: __control

    property bool __horizontal: control.__horizontal

    property int vertPadding: __horizontal ? 0 : 1
    property int horzPadding: !__horizontal ? 0 : 1

    padding { top: vertPadding ; left: horzPadding ; right: horzPadding ; bottom: vertPadding }

    /*! This property loads the slider handle style found in WGSliderHande.qml or descendent.
    */
    property Component handle:

    Loader {
        id: handleFrame
        sourceComponent: control.__handlePosList.children[buttonid].handleStyle
    }
    /*! This property holds the background groove of the slider.
    */
    property Component groove: Item {

        anchors.verticalCenter: __horizontal ? parent.verticalCenter : undefined
        anchors.horizontalCenter: !__horizontal ? parent.horizontalCenter : undefined

        implicitWidth: Math.round(defaultSpacing.minimumRowHeight / 4)
        implicitHeight: Math.round(defaultSpacing.minimumRowHeight / 4)

        WGTextBoxFrame {
            radius: defaultSpacing.standardRadius
            anchors.fill: parent
            color: control.enabled ? palette.textBoxColor : "transparent"
        }
    }

    /*! This property holds the coloured bar of the slider.
    */
    property Component bar: Item {
        property color fillColor: control.__handlePosList.children[barid].barColor
        clip: true
        Rectangle {
            clip: true
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardBorderSize
            border.color: control.enabled ? Qt.darker(fillColor, 1.2) : palette.lighterShade
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
            vertical: __horizontal
            width: __horizontal ? defaultSpacing.separatorWidth : defaultSpacing.standardMargin
            height: !__horizontal ? defaultSpacing.separatorWidth : defaultSpacing.standardMargin

            x: __horizontal ? control.__handleWidth / 2 + index * ((repeater.width - control.__handleWidth) / (repeater.count-1)) - (defaultSpacing.separatorWidth / 2) : defaultSpacing.doubleMargin
            y: !__horizontal ? control.__handleHeight / 2 + index * ((repeater.height - control.__handleHeight) / (repeater.count-1)) - (defaultSpacing.separatorWidth / 2) : defaultSpacing.doubleMargin
        }
    }

    /*! This property holds the slider style panel.

        Note that it is generally not recommended to override this.
    */
    property Component panel: Item {
        id: root

        implicitWidth: __horizontal ? parent.width : grooveLoader.implicitWidth
        implicitHeight: !__horizontal ? parent.height : grooveLoader.implicitHeight

        Item {
            objectName: "sliderFrame"
            id: sliderFrame
            anchors.centerIn: parent
            height: control.height
            width: control.width

            Loader {
                id: grooveLoader
                sourceComponent: groove

                width: __horizontal ? parent.width - padding.left - padding.right : groove.implicitWidth

                height: !__horizontal ? parent.height - padding.top - padding.bottom : groove.implicitHeight

                x: {
                    if(control.groovePadding)
                    {
                        __horizontal ? padding.left : padding.left + ((__horizontal ? parent.height : parent.width - padding.left - padding.right) - grooveLoader.item.width)/2
                    }
                    else
                    {
                        __horizontal ? 0 : ((__horizontal ? parent.height : parent.width) - grooveLoader.item.width)/2
                    }
                }

                y: {
                    if(control.groovePadding)
                    {
                        !__horizontal ? padding.top : padding.top + ((__horizontal ? parent.height : parent.width - padding.top - padding.bottom) - grooveLoader.item.height)/2
                    }
                    else
                    {
                        !__horizontal ? 0 : ((__horizontal ? parent.height : parent.width) - grooveLoader.item.height)/2
                    }
                }

                Repeater {
                model: control.__handlePosList.children
                    Loader {
                        id: barLoader
                        sourceComponent: bar
                        property int barid: index
                        visible: control.__handlePosList.children[index].showBar

                        anchors.verticalCenter: __horizontal ? grooveLoader.verticalCenter : undefined
                        anchors.horizontalCenter: !__horizontal ? grooveLoader.horizontalCenter : undefined

                        height: __horizontal ? grooveLoader.height : control.height - control.__handlePosList.children[index].barMinPos - padding.top - padding.bottom
                        width: !__horizontal ? grooveLoader.width : control.__handlePosList.children[index].range.position - control.__handlePosList.children[index].barMinPos - padding.left - padding.right

                        y: !__horizontal ? control.__handlePosList.children[index].barMinPos : 0
                        x: __horizontal ? control.__handlePosList.children[index].barMinPos : 0
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
            model: control.__handlePosList.children
                Loader {
                    id: handleLoader
                    sourceComponent: handle
                    property int buttonid: index

                    anchors.verticalCenter: __horizontal ? grooveLoader.verticalCenter : undefined

                    anchors.horizontalCenter: !__horizontal ? grooveLoader.horizontalCenter : undefined

                    height: handleLoader.implicitHeight

                    width: handleLoader.implicitWidth

                    Behavior on height{
                        enabled: __horizontal
                        NumberAnimation {
                            duration: 120
                            easing {
                                type: Easing.OutCirc
                                amplitude: 1.0
                                period: 0.5
                            }
                        }
                    }

                    Behavior on width{
                        enabled: !__horizontal
                        NumberAnimation {
                            duration: 120
                            easing {
                                type: Easing.OutCirc
                                amplitude: 1.0
                                period: 0.5
                            }
                        }
                    }

                    x: __horizontal ? Math.round(control.__handlePosList.children[index].range.position - control.__handleWidth / 2) : 0
                    y: !__horizontal ? Math.round(control.__handlePosList.children[index].range.position - control.__handleHeight / 2) : 0

                    onLoaded: {
                        control.__handlePosList.children[index].handleIndex = index

                        control.__handleHeight = handleLoader.implicitHeight
                        control.__handleWidth = handleLoader.implicitWidth
                    }

                    MouseArea {
                        objectName: "sliderHandleArea"
                        hoverEnabled: true
                        anchors.fill: parent

                        propagateComposedEvents: true

                        onEntered: {
                            control.__hoveredHandle = handleIndex
                        }

                        onExited: {
                            if (control.__hoveredHandle == handleIndex)
                            {
                               control.__hoveredHandle = -1
                            }
                        }

                        onPressed: {
                            control.__activeHandle = handleIndex
                            control.forceActiveFocus()

                            if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
                            {
                                control.__draggable = false
                                control.handleCtrlClicked(handleIndex)
                            }

                            mouse.accepted = false
                        }

                        onReleased: {
                            control.__draggable = true
                        }

                    }
                }
            }
        }
    }
}
