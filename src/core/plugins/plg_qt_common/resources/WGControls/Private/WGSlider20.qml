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

import WGControls.Styles 2.0
import WGControls.Private 2.0

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
    objectName: "WGSlider20"
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
    property bool activeFocusOnPress: true

    /*!
        This property indicates whether the slider should display tickmarks
        at step intervals. Tick mark spacing is calculated based on the
        \l stepSize property.

        The default value is \c false.
    */
    property bool tickmarksEnabled: false

    /*!
        The interval (in value) between equal tickmarks, best set to a whole fraction of the max value.

        The default value is stepSize.
    */
    property real tickmarkInterval: stepSize

    /*!
        An array of values that can be used to show tickmarks at custom intervals. These can be used in
        addition to the default tickmarks.

        The values in the array should be within the slider max and minimum values.

        The default value is an empty array (no custom tickmarks)
    */
    property var customTickmarks: []

    /*!
        An array of values used for the labels for the customTickmarks

        The default value is the customTickmarks array itself.
    */
    property var customTickmarkLabels: customTickmarks

    /*!
        This property indicates whether the slider should display values
        underneath the tickmarks.

        The default value is \c false.
    */
    property bool showTickmarkLabels: false

    /*!
        An array of values that the slider handle will 'stick' to when dragged.

        The default value is empty.
    */
    property var stickyValues: []

    /*!
        The amount of space in pixels on either side of the values where the mouse will be 'sticky'

        The default value is the standard margin width plus the standard border size (6 pixels on each side).
    */
    property int stickyMargin: defaultSpacing.standardMargin + defaultSpacing.standardBorderSize

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

    /*! property indicates if the control represetnts multiple data values */

    property bool multipleValues: false


    /*!
        This property determines if the handles should be constrained within the length of the slider.

        This is useful to make sure the handles don't move outside the control boundaries but means the control values
        don't exactly follow the control height/width in a linear fashion. (the value is always accurate)

        The default value is \ctrue
    */
    property bool handleClamp: true

    /*!
        This property the specifieid amount of clickable area outside the visible groovearea at each end of the slider.
        This is useful if the handles project outside the groove but still need to be clickable

        The default value adds half the handle width if handleClamp = true
    */
    property int clickAreaBuffer: handleClamp ? 0 : (__horizontal ? __handleWidth / 2 : __handleHeight / 2)

    /*!
        This property determines the default color of the bar that 'fills' the slider if is enabled

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
        move the active handle to that position. Setting this to false gives better control
        and makes it harder to move the handles by accident.

        The default value is \c true
    */
    property bool grooveClickable: true

    /*!
        This property determines if the slider handles can be moved using the mouse wheel.

        The default value is \c true
    */

    property bool allowMouseWheel: true

    /*!
        This property determines if the slider will create a handle if none are initialised.

        The default value is \c true
    */

    property bool createInitialHandle: true

    /*!
        This string determines the the component for the slider handle.

        The default value is WGColorSliderHandle.
        It can be set to any Item based component.
    */
    property Component handleType: showTickmarkLabels ? tickmarkValueHandle : defaultSliderHandle

    /*!
        The cursorShape within the slider area.

        This is changed by hoverCursor and dragCursor in slider handles.

        The default value is Qt.ArrowCursor
    */
    property var cursorShape: Qt.ArrowCursor

    /*! The index of the currently hovered handle */

    property int hoveredHandle: -1

    /*! \internal */
    property bool __draggable: grooveClickable

    /*! \internal */
    property bool __horizontal: orientation === Qt.Horizontal

    /*! \internal */
    property int __activeHandle: 0

    /*! \internal */
    property int __handleHeight: defaultSpacing.minimumRowHeight

    /*! \internal */
    property int __handleWidth: defaultSpacing.minimumRowHeight

    property alias pressed: mouseArea.pressed

    /*!
        This property holds the handle objects.
    */
    property var __handlePosList: []

    /*!
        This property holds the number of handle objects.
    */
    property int __handleCount: 0

    property bool sliderHovered: mouseArea.containsMouse || hoveredHandle > -1

    activeFocusOnTab: true

    /*! \internal */
    property var __currentCursor: cursorShape

    /*! \internal */
    property bool __handleMoving: false

    /*! \internal */
    property real __grabbedValue: 0

    /*! \internal */
    property real __sliderLength: __horizontal ? slider.width : slider.height

    /*! \internal */
    property real __clampedLength: __visualMaxPos - __visualMinPos

    /*! \internal */
    property real __visualMinPos: handleClamp ? __handleWidth / 2 : 0

    /*! \internal */
    property real __visualMaxPos: handleClamp ? __sliderLength - __handleWidth / 2 : __sliderLength

    //Accessible.role: Accessible.Slider
    /*! \internal */
    function accessibleIncreaseAction() {
        __handlePosList.children[__activeHandle].range.increaseSingleStep()
    }
    /*! \internal */
    function accessibleDecreaseAction() {
        __handlePosList.children[__activeHandle].range.decreaseSingleStep()
    }

    style: Qt.createComponent("../Styles/Base/WGSliderStyle20.qml", slider)

    Keys.onPressed: {

        // Increase jump if Ctrl pressed
        var multiplier = (event.modifiers & Qt.ControlModifier) ? 10 : 1

        if ((event.key == Qt.Key_Right) &&  (__horizontal))
        {
            __handlePosList[__activeHandle].range.value += (stepSize * multiplier)
            event.accepted = true;
        }
        else if ((event.key == Qt.Key_Left) &&  (__horizontal))
        {
            __handlePosList[__activeHandle].range.value -= (stepSize * multiplier)
            event.accepted = true;
        }
        else if (event.key == Qt.Key_Up)
        {
            __handlePosList[__activeHandle].range.value += (stepSize * multiplier)
            event.accepted = true;
        }
        else if (event.key == Qt.Key_Down)
        {
            __handlePosList[__activeHandle].range.value -= (stepSize * multiplier)
            event.accepted = true;
        }
        else
        {
            event.accepted = false;
        }
    }

    /*!
        This signal is fired when the bar is double clicked
    */
    signal sliderDoubleClicked(int index, int mouseModifiers)

    /*!
        This signal is fired when a handle (index) is pressed with the mouse button (mouseButton) and mouse modifiers (mouseModifiers)
    */
    signal handleClicked(int index, int mouseButton, int mouseModifiers)

    /*!
        This signal is fired when a handle (index) is added
    */
    signal handleAdded(int index)

    /*!
        This signal is fired when a handle (index) is removed
    */
    signal handleRemoved(int index)

    /*!
        This signal is fired when the mouse is pressed anywhere in the groove.

        If the handle is able to be moved, draggable will be true
    */
    signal sliderPressed(bool draggable, point mouse)

    /*!
        This signal is fired when a handle (index) value (val) is changed
    */
    signal changeValue (real val, int index)

    /*!
        This signal is fired when a handle (index) begins dragging
    */
    signal beginDrag (int index)

    /*!
        This signal is fired when a handle (index) has finished dragging
    */
    signal endDrag (int index)

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: __handleWidth * 5

    /*!
        This function creates a new WGSliderHandle object (handle) with value (val) at (index)
        and automatically adds it to __handlePosList.

        Only the value is actually required.
    */
    function createHandle(val, handle, index, emitSignal)
    {
        if (typeof emitSignal == "undefined")
        {
            var emitSignal = true
        }

        if (typeof handle !== "undefined")
        {
            var newHandle = handle.createObject(slider, {
                                        "value": val,
                                        "parentSlider": slider
                                    });
        }
        else
        {

            var newHandle = slider.handleType.createObject(slider, {
                                        "value": val,
                                        "parentSlider": slider
                                    });
        }
        if (typeof index != "undefined")
        {
            addHandle(newHandle, index)
            if (emitSignal)
            {
                handleAdded(index)
            }
        }
        else
        {
            addHandle(newHandle, __handlePosList.length)
            if (emitSignal)
            {
                handleAdded(index)
            }
        }
        return newHandle
    }

    /*!
        Removes the handle (index) from __handlePosList and destroys it.
    */
    function removeHandle(index) {
        if(__handleCount > 0)
        {
            var handleToRemove = __handlePosList[index]
            // Remove the bindings from the new head to the handle being removed
            if(index === 0 && __handleCount > 1)
            {
                __handlePosList[index+1].minimumValue = handleToRemove.minimumValue
            }
            // Remove the bindings from the new tail to the handle being removed
            if(index === (__handleCount - 1) && index > 0)
            {
                __handlePosList[index-1].maximumValue = handleToRemove.maximumValue
            }
            __handlePosList.splice(index,1)
            __handleCount = __handlePosList.length
            __activeHandle = Math.min(__activeHandle,__handlePosList.length - 1)
            handleRemoved(index)
            handleToRemove.destroy()
        }
    }

    /*! Internal */
    function addHandle(handle, index) {
        handle.parentSlider = slider
        __handlePosList.splice(index,0,handle)
        __handleCount = __handlePosList.length
        __activeHandle = index
    }

    /*!
        Returns the handle value at a given index (index) or -1 if the handle doesn't exist
    */
    function getHandleValue(index) {
        index = typeof index !== "undefined" ? index : 0

        if (index <= __handleCount - 1)
        {
            return __handlePosList[index].value
        }
        else
        {
            return -1
        }
    }

    /*!
        Sets the handle value at a given index (index) to val or sets the values of an array of handle indexes to an array of values.

        TODO: Make it possible to set multiple handles to one value?
    */
    function setHandleValue(val, index) {

        index = typeof index !== "undefined" ? index : 0
        index = Array.isArray(index) ? index : [index]
        val = Array.isArray(val) ? val : [val]

        for (var i = 0; i < index.length; i++)
        {
            var indexToMove = index[i]
            if (indexToMove <= __handleCount - 1)
            {
                if (val[i] < __handlePosList[indexToMove].minimumValue)
                {
                    console.log("WARNING WGSlider: Tried to set the value of handle " + indexToMove + " to less than its minimum value")
                    setValueHelper(slider.__handlePosList[indexToMove], "value", __handlePosList[indexToMove].minimumValue);
                }
                else if ( val[i] > __handlePosList[indexToMove].maximumValue )
                {
                    console.log("WARNING WGSlider: Tried to set the value of handle " + indexToMove + " to more than its maximum value")
                    setValueHelper(slider.__handlePosList[indexToMove], "value", __handlePosList[indexToMove].maximumValue);
                }
                else
                {
                    setValueHelper(slider.__handlePosList[indexToMove], "value", val[i]);
                }
            }
            else
            {
                console.log("WARNING WGSlider: Tried to change the value of a handle that does not exist")
            }
        }
    }

    /*!
        Attempts to find a handle at a particular value and returns the index.

        Pretty hacky..
    */

    function getHandleAtValue(val)
    {
        var roundedPos
        for (var i = 0; i < __handlePosList.length; i++)
        {
            roundedPos = Number(Math.round(__handlePosList[i].value + 'e2') + 'e-2')
            if (roundedPos == val)
            {
                return i
            }
        }
        console.log("WARNING WGSlider: No handle found at position " + val)
        return -1
    }

    onValueChanged: {
        if (__handleCount == 1 && !__handleMoving)
        {
            setValueHelper(__handlePosList[0], "value", value);
        }
    }

    onChangeValue: {
        if (__handleCount == 1 && slider.value != val)
        {
            setValueHelper(slider, "value", val);
        }
    }

    onHoveredHandleChanged: {
        if (hoveredHandle == -1)
        {
            __currentCursor = cursorShape
        }
        else
        {
            __currentCursor = __handlePosList[hoveredHandle].hoverCursor
        }
    }

    Component.onCompleted: {
        //collect any child handles and put them in __handlePosList
        for (var i = 0; i < slider.children.length; i++)
        {
            if (slider.children[i].objectName == "SliderHandle")
            {
                addHandle(slider.children[i],__handlePosList.length)
            }
        }

        //create a handle if none were collected
        if(__handlePosList.length == 0 && createInitialHandle)
        {
            var newHandle = createHandle(slider.value)
        }
    }

    MouseArea {
        id: mouseArea
        objectName: "sliderDragArea"

        z:-1

        anchors.centerIn: parent

        height: __horizontal ? parent.height : parent.width
        width: (__horizontal ? parent.width : parent.height) + clickAreaBuffer

        rotation: __horizontal ? 0 : -90
        transformOrigin: Item.Center

        hoverEnabled: true

        cursorShape: slider.__currentCursor

        property real pressedVal

        function clamp ( val ) {
            return Math.max(__handlePosList[__activeHandle].range.positionAtMinimum, Math.min(__handlePosList[__activeHandle].range.positionAtMaximum, val))
        }

        onEntered: {
            hoveredHandle = -1
        }

        function updateHandlePosition(mouse, force) {

            if (__draggable)
                {
                var pos

                if (slider.clickAreaBuffer > 0)
                {
                    mouse = mouseArea.mapToItem(sliderArea,mouse.x,mouse.y)
                }

                pos = clamp (mouse.x)
                if (stickyValues.length > 0)
                {
                    var sVal = (((pos - (__handleWidth / 2)) / __clampedLength) * (maximumValue - minimumValue) + minimumValue)
                    var sRange = (maximumValue - minimumValue) * (stickyMargin / __clampedLength)
                    var updatePosition = true
                    for (var i=0; i < stickyValues.length; i++)
                    {
                        if (sVal >= stickyValues[i] - sRange && sVal <= stickyValues[i] + sRange)
                        {
                            setValueHelper(slider.__handlePosList[__activeHandle], "value", stickyValues[i]);
                            // leaving here if you want to calculate by position instead of value
                            //pos = ((stickyValues[i] - minimumValue) / (maximumValue - minimumValue) * __clampedLength) + (__handleWidth / 2)
                            updatePosition = false
                            break;
                        }
                    }
                    if (updatePosition) { __handlePosList[__activeHandle].range.position = pos }
                }
                else
                {
                    __handlePosList[__activeHandle].range.position = pos
                }
            }
        }

        onPositionChanged: {
            if (pressed)
            {
                updateHandlePosition(mouse, preventStealing)
            }
        }

        onPressed: {
            if ( __draggable )
            {
                preventStealing = true
                sliderPressed(__draggable, Qt.point(mouse.x,mouse.y))
                pressedVal = __handlePosList[__activeHandle].value
                beginUndoFrame();

                if ( grooveClickable )
                {
                    beginDrag(__activeHandle)
                    slider.__currentCursor = __handlePosList[__activeHandle].dragCursor
                    __handleMoving = true
                    __grabbedValue = value;
                    if (slider.activeFocusOnPress)
                    slider.forceActiveFocus();
                    updateHandlePosition(mouse, !Settings.hasTouchScreen)
                }
            }
        }

        onReleased: {
            if(__draggable) {
                if ( grooveClickable )
                {
                    updateHandlePosition(mouse, Settings.hasTouchScreen)
                }
                if (pressedVal !== __handlePosList[__activeHandle].value)
                {
                    endUndoFrame();
                }
                else
                {
                    abortUndoFrame();
                }
                slider.__currentCursor = slider.cursorShape

                endDrag(__activeHandle)

                preventStealing = false

                __handleMoving = false

                __draggable = grooveClickable
            }
        }

        //signal when bar is double clicked.
        //can be used for double clicking a handle, but if this is in the handle object, dragging won't work.
        onDoubleClicked: {
            if (hoveredHandle >= 0)
            {
                abortUndoFrame();
                __draggable = grooveClickable
                hoveredHandle = -1
                sliderDoubleClicked(__activeHandle, mouse.modifiers)
            }
        }

        onWheel: {
            if(slider.activeFocus && allowMouseWheel){
                if (wheel.angleDelta.y > 0)
                {
                    __handlePosList[__activeHandle].range.increaseSingleStep()
                }
                else
                {
                    __handlePosList[__activeHandle].range.decreaseSingleStep()
                }
            }
            else
            {
                wheel.accepted = false
            }
        }
    }

    //an invisible object to represent the area the handles can actually move
    Item {
        id: sliderArea
        anchors.centerIn: parent

        height: __horizontal ? parent.height : parent.width
        width: __horizontal ? parent.width : parent.height

        rotation: __horizontal ? 0 : -90
        transformOrigin: Item.Center
    }

    //make handles with activefocus the __activeHandle and vice versa
    Keys.onTabPressed: {
        if (slider.activeFocus && __activeHandle == __handlePosList.length - 1)
        {
            slider.nextItemInFocusChain(true).forceActiveFocus()
        }
        else
        {
            slider.__activeHandle++
        }
    }

    Keys.onBacktabPressed: {
        if (slider.activeFocus && __activeHandle == 0)
        {
            slider.nextItemInFocusChain(false).forceActiveFocus()
        }
        else
        {
            slider.__activeHandle--
        }
    }

    Component {
        id: defaultSliderHandle
        WGSliderHandle {}
    }

    Component {
        id: tickmarkValueHandle
        WGTickmarkSliderHandle {}
    }

    /* Deprecated */
    property alias handleStyle: slider.handleType;
}
