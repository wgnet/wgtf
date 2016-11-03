/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
 \brief A rewrite of the default QML Slider control that allows 'x'
 handles and can be setup as a range slider easily.

 Generally if you want to use this slider in an NGT context you
 would use WGSliderControl or WGRangeSlider which includes the required
 number boxes.

 The WGSliderHandle range will default to the WGSlider range unless explicitely
 assigned


Example:
\code{.js}
WGSlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    stepSize: 1.0

    WGSliderHandle {
        minimumValue: 20
        maximumValue: 80
        value: 50
    }
}
\endcode
*/

Control {
    objectName: "WGSlider"
    id: slider

    /*!
        This property holds the layout orientation of the slider.
        The default value is \c Qt.Horizontal.

        TODO: Make range sliders work if orientation: Qt.Vertical
    */
    property int orientation: Qt.Horizontal

    /*!
        This property indicates whether the slider should receive active focus when
        pressed.
    */
    property bool activeFocusOnPress: false

    /*!
        This property indicates whether the slider should display tickmarks
        at step intervals. Tick mark spacing is calculated based on the
        \l stepSize property.

        The default value is \c false.
    */
    property bool tickmarksEnabled: false

    /*!
        This property holds the minimum value of the slider.
        The default value is \c{0.0}.

        This is not always the same as the minimumValue of the slider handle.
    */

    property real minimumValue: 0
    /*!
        This property holds the maximum value of the slider.
        The default value is \c{100.0}.

        This is not always the same as the maximumValue of the slider handle.
    */

    property real maximumValue: 100

    /*!
        This is a helper property that sets the value of the slider IF there is only one handle
        and it is NOT otherwise set itself.
        The default value is \c{0.0}.
    */

    property real value: 0

    /*!
        This property determines if the handles should be constrained within the length of the slider.

        This is useful to make sure the handles don't move outside the control boundaries but means the control values
        don't exactly follow the control height/width in a linear fashion. (the value is always accurate)

        The default value is \ctrue
    */

    property bool handleClamp: true

    /*!
        This property determines the color of the bar that 'fills' the slider if is enabled

        The default value is \c palette.highlightColor
    */

    property color barColor: palette.highlightColor

    /*! This property indicates the slider step size.

        A value of 0 indicates that the value of the slider operates in a
        continuous range between \l minimumValue and \l maximumValue.

        Any non 0 value indicates a discrete stepSize. The following example
        will generate a slider with integer values in the range [0-5].

        \code{.js}
        Slider {
            maximumValue: 5.0
            stepSize: 1.0
        }
        \endcode

        The default value is \c{0.0}.
    */
    property real stepSize: 1

    /*!
        This property determines if the user can click the bar itself to
        move the handle to that position. Setting this to false gives better control
        and makes it harder to move the handles by accident.

        The default value is \c true
    */
    property bool grooveClickable: true

    /*! \internal */
    property bool __draggable: true

    /*! \internal */
    property bool __horizontal: orientation === Qt.Horizontal

    /*! \internal */
    property int __activeHandle: 0

    /*! \internal */
    property int __hoveredHandle: -1

    /*! \internal */
    property int __handleHeight

    /*! \internal */
    property int __handleWidth

    property alias pressed: mouseArea.pressed


    /*!
        This property holds of the handle objects.
    */

    default property alias defaultProperty: __handlePosList.children

    property alias __handlePosList: __handlePosList

    Item {
        id: __handlePosList
        anchors.fill: parent
    }

    activeFocusOnTab: true


    /*! \internal */
    property bool __handleMoving: false

    property real __grabbedValue: 0

    //Accessible.role: Accessible.Slider
    /*! \internal */
    function accessibleIncreaseAction() {
        __handlePosList.children[__activeHandle].range.increaseSingleStep()
    }
    /*! \internal */
    function accessibleDecreaseAction() {
        __handlePosList.children[__activeHandle].range.decreaseSingleStep()
    }

    style: Qt.createComponent("../Styles/Base/WGSliderStyle.qml", slider)

    Keys.onRightPressed: if (__horizontal) __handlePosList.children[__activeHandle].range.increaseSingleStep()
    Keys.onLeftPressed: if (__horizontal) __handlePosList.children[__activeHandle].range.decreaseSingleStep()

    Keys.onUpPressed: __handlePosList.children[__activeHandle].range.increaseSingleStep()
    Keys.onDownPressed: __handlePosList.children[__activeHandle].range.decreaseSingleStep()

    property int internalWidth: handleClamp ? mouseArea.width - __handleWidth : mouseArea.width
    property int internalHeight: handleClamp ? mouseArea.height - __handleHeight : mouseArea.height

    x: __horizontal ? __handleWidth / 2 : 0
    y: !__horizontal ? __handleHeight / 2 : 0

    /*!
        This signal is fired when the bar is double clicked
    */
    signal sliderDoubleClicked(int index)

    /*!
        This signal is fired when a handle (handleIndex == index) is left pressed when holding the Ctrl key
    */
    signal handleCtrlClicked(int index)

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: __handleWidth * 5

    MouseArea {
        id: mouseArea
        objectName: "sliderDragArea"

        z:-1

        anchors.centerIn: parent

        width: parent.width
        height: parent.height

        hoverEnabled: true

        property int clickOffset: 0
        property real pressX: 0
        property real pressY: 0

        property bool dragStarted: false

        function clamp ( val ) {
            return Math.max(__handlePosList.children[__activeHandle].range.positionAtMinimum, Math.min(__handlePosList.children[__activeHandle].range.positionAtMaximum, val))
        }

        function updateHandlePosition(mouse, force) {

            if (__draggable)
                {
                var pos, overThreshold
                if (__horizontal) {
                    pos = clamp (mouse.x + clickOffset)
                    overThreshold = Math.abs(mouse.x - pressX) >= Settings.dragThreshold
                    if (overThreshold)
                        preventStealing = true
                    if (overThreshold || force)
                        __handlePosList.children[__activeHandle].x = pos
                } else if (!__horizontal) {
                    pos = clamp (mouse.y + clickOffset)
                    overThreshold = Math.abs(mouse.y - pressY) >= Settings.dragThreshold
                    if (overThreshold)
                        preventStealing = true
                    if (overThreshold || force)
                        __handlePosList.children[__activeHandle].y = pos
                }
            }
        }

        onPositionChanged: {
            if (pressed)
                updateHandlePosition(mouse, preventStealing)
        }

        onPressed: {
            if (__draggable)
            {
                beginUndoFrame();
                __handleMoving = true
                __grabbedValue = value;
                if (slider.activeFocusOnPress)
                    slider.forceActiveFocus();

                if(!grooveClickable)
                {
                    if(dragStarted) {
                        pressX = mouse.x
                        pressY = mouse.y

                        updateHandlePosition(mouse, !Settings.hasTouchScreen)
                    }
                }
                else
                {
                    pressX = mouse.x
                    pressY = mouse.y

                    updateHandlePosition(mouse, !Settings.hasTouchScreen)
                }
            }
        }

        onReleased: {

            if (!grooveClickable)
            {
                if(dragStarted) {
                    updateHandlePosition(mouse, Settings.hasTouchScreen)
                    // If we don't update while dragging, this is the only
                    // moment that the range is updated.
                }
            }
            else
            {
                updateHandlePosition(mouse, Settings.hasTouchScreen)
            }

            endUndoFrame();

            clickOffset = 0
            preventStealing = false

            __handleMoving = false

            dragStarted = false

            __draggable = true
        }

        //signal when bar is double clicked.
        //can be used for double clicking a handle, but if this is in the handle object, dragging won't work.
        onDoubleClicked: {
            sliderDoubleClicked(__activeHandle)
        }

        onWheel: {
            if(slider.activeFocus){
                if (wheel.angleDelta.y > 0)
                {
                    __horizontal ? __handlePosList.children[__activeHandle].range.increaseSingleStep() : __handlePosList.children[__activeHandle].range.decreaseSingleStep()
                }
                else
                {
                    !__horizontal ? __handlePosList.children[__activeHandle].range.increaseSingleStep() : __handlePosList.children[__activeHandle].range.decreaseSingleStep()
                }
            }
        }
    }
}
