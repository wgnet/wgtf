import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import WGControls 2.0
import WGControls.Styles 2.0
import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
 \brief A multi-handle Slider with a color gradient background.
 Purpose: Allow the user to edit a linear gradient.

WGGradientSlider is a gradient or ramp slider where moving the handles changes the gradient
positions and individual colors in the gradient can be edited by the user. It is intended that
this slider could have any number of handles.

Handles can be added with Shift+left click anywhere in the slider.
Handles can be deleted by Ctrl+left clicking on a handle
Handles colors can be changed by double clicking them.

There are multiple methods of creating a gradient slider. Both of the following example create a
gradient from red to yellow to white with three handles. The gradient slider needs some kind of
color data to work with and so will not create a default handle.

Example:
\code{.js}
    WGGradientSlider {
        Layout.fillWidth: true

        Component.onCompleted: {
            createColorHandle(.25,handleStyle,__handlePosList.length, "red")
            createColorHandle(.5,handleStyle,__handlePosList.length, "yellow")
            createColorHandle(.75,handleStyle,__handlePosList.length, "white")
        }
    }
\endcode

Example:
\code{.js}
    WGGradientSlider {
        Layout.fillWidth: true

        WGGradientSliderHandle {
            value: 0.25
            color: "red"
        }
        WGGradientSliderHandle {
            value: 0.5
            color: "yellow"
        }
        WGGradientSliderHandle {
            value: 0.75
            color: "white"
        }
    }
\endcode

\todo TODO: Test orientation = vertical. Create vertical slider. Remove option here
*/

WGSlider {
    id: slider
    objectName: "WGColorSlider"
    WGComponent { type: "WGGradientSlider20" }

    /*!
        This value determines whether double clicking a handle should display a color picker.
    */
    property bool useColorPicker: true

    /*!
        This value determines whether the alpha value will appear when changing a color handle via a color picker.
    */
    property bool showAlphaChannel: true

    /*!
        This value determines whether the user can add and delete handles from the slider by ctrl and shift clicking.
    */
    property bool addDeleteHandles: true

    /*!
        This value determines both the vertical offset of the handles AND the additional margin
        below the slider groove. This works well with handles that look like arrows to make them
        sit below the groove.

        The default value is 0.
    */
    property int handleVerticalOffset: 0

    /*!
        This value is true if the color picker is visible
    */
    readonly property bool colorPickerOpen: colorPicker.visible;

    minimumValue: 0

    maximumValue: 1

    stepSize: .001

    handleType: WGGradientSliderHandle{}

    implicitHeight: defaultSpacing.minimumRowHeight

    handleClamp: false

    grooveClickable: false

    createInitialHandle: false

    style: WGGradientSliderStyle{}

    property var colorPicker: ColorDialog {
        id: colorPicker
        title: "Please choose a color"
        showAlphaChannel: slider.showAlphaChannel

        property int currentColorIndex: -1

        onVisibleChanged: {
            if (visible) {
                beginUndoFrame();
            }
        }

        onAccepted: {
            if(currentColorIndex >= 0)
            {
                colorModified(Qt.rgba(colorPicker.color.r,colorPicker.color.g,colorPicker.color.b,colorPicker.color.a), currentColorIndex)
                endUndoFrame();
                currentColorIndex = -1
            }
        }

        onRejected: {
            abortUndoFrame();
            currentColorIndex = -1
        }
    }

    // the repeated column of gradient bars loaded in WGGradientSliderStyle
    property Component gradientFrame: Item {
        id: gradientFrame

        //finds the right color bar and then tells it to create a new handle based off pos
        Connections {
            target: slider
            onCreateHandleAtPosition: {
                pos = Math.max(pos, 0)
                pos = Math.min(pos, 1)

                var framePos = mapToItem(gradientFrame, 1, pos * __sliderLength)

                var bar = childAt(framePos.x, framePos.y)
                bar.addPointToBar(framePos)
            }
        }

        Repeater {
            id: barRepeater
            //adds an extra bar at the end that has no corresponding handle
            model: __handleCount + 1

            Rectangle {
                id: colorBar
                objectName: "ColorBar"

                property real minPos: {
                    if (index == 0)
                    {
                        1
                    }
                    else
                    {
                        Math.max(slider.__handlePosList[index - 1].range.position, 1)
                    }
                }

                property real maxPos: {
                    if (index == slider.__handleCount)
                    {
                        gradientFrame.height - 1
                    }
                    else
                    {
                        Math.min(slider.__handlePosList[index].range.position, gradientFrame.height - 1)
                    }
                }

                width: gradientFrame.width
                height: Math.max((maxPos - minPos),0)
                y: minPos

                function addPointToBar(pos)
                {
                    var barPos = mapFromItem(gradientFrame, pos.x, pos.y)

                    barPos = barPos.y / colorBar.height

                    var newColor = Qt.rgba(1,1,1,1)
                    if (index == slider.__handleCount && index != 0)
                    {
                        newColor = slider.__handlePosList[index - 1].color
                    }
                    else if(slider.__handleCount > index)
                    {
                        newColor = slider.__handlePosList[index].getInternalColor(barPos)
                    }

                    var newPos = pos.y / (gradientFrame.height / (slider.maximumValue - slider.minimumValue))

                    slider.createColorHandle(newPos, slider.handleType, index, newColor, true)
                }

                gradient: {
                    if (slider.__handleCount > 0)
                    {
                        if (index == slider.__handleCount)
                        {
                            //if it's the last bar, just uses a solid color
                            colorBar.color = slider.__handlePosList[index - 1].color
                            return null
                        }
                        else
                        {
                            slider.__handlePosList[index].gradient
                        }
                    }
                }

                // Mouse area for adding new handles
                MouseArea
                {
                    anchors.fill: parent
                    propagateComposedEvents: true

                    // Workaround for crash during Shift+Click, this colorBar may no longer be associated with the parent
                    // Without this when the event is propagated an attempt to access the null window crashes the application
                    onPressAndHold: { mouse.accepted = true}

                    onPressed: {
                        //adds handles when bar is Shift Clicked
                        if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier) && slider.addDeleteHandles)
                        {
                            var framePos = mapToItem(gradientFrame, mouseX, mouseY)
                            addPointToBar(framePos)
                        }
                        else if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.sliderModifier) && slider.addDeleteHandles)
                        {
                            mouse.accepted = false
                        }
                        else
                        {
                            mouse.accepted = false
                        }
                    }
                    onReleased: {
                        slider.__draggable = true
                    }
                }
            }
        }
    }

    /*!
        This signal is fired when a point's color is changed
    */
    signal colorModified(color color, int index)

    /*!
        creates a new color handle with value (val), handleType (handle), color (col) and gradient (grad)

        Only a value is actually needed to create a handle. handleType will default to the slider handleType.
        Color will default to the default color in the handle.
        The gradient can usually be ignored unless you wish to create a non-linear gradient
    */

    /*!
        creates a new color handle at the position (pos) in pixels of the entire slider.

        The color will be worked out automatically.
    */
    signal createHandleAtPosition(real pos)

    /*!
        creates a new color handle at the position (pos) in pixels of the entire slider.

        The color will be worked out automatically.
    */
    signal createHandleAtPixelPosition(int pos)

    onColorModified: {
        __handlePosList[index].color = color
        updateHandles()
    }

    onCreateHandleAtPixelPosition: {
        pos = pos / __sliderLength
        pos = Math.max(pos, 0)
        pos = Math.min(pos, 1)
        createHandleAtPosition(pos)
    }

    onChangeValue: {
        updateHandles()
    }

    //delete a handle
    onHandleClicked: {
        if (mouseModifiers & Qt.ControlModifier)
        {
            removeHandle(index)
            updateHandles()
        }
    }

    onHandleRemoved: updateHandles()

    onHandleAdded:  updateHandles()

    //pick a color using ColorDialog
    onSliderDoubleClicked: {
        if (useColorPicker)
        {
            colorPicker.color = __handlePosList[index].color
            colorPicker.currentColorIndex = index
            colorPicker.open()
        }
    }

    function createColorHandle(val, handle, index, color, emitSignal, grad)
    {
        emitSignal = emitSignal !== "undefined" ? emitSignal : true

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

        if (typeof color != "undefined")
        {
            newHandle.color = color
        }

        if (typeof grad != "undefined")
        {
            newHandle.gradient = grad
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

        updateHandles()

        return newHandle
    }

    /*!
        This can be used to assign a new gradient to a WGGradientSliderHandle.__horizontal which can be useful
        if you need to apply a non-linear gradient.

        TODO: Fix odd color bars be created with non-linear gradients and Shift+Click
    */
    function setHandleGradient(index, grad)
    {
        if (typeof grad != "undefined")
        {
            __handlePosList[index].gradient = grad
        }
        else
        {
            __handlePosList[index].gradient.stops[0].color = __handlePosList[index].minColor
            __handlePosList[index].gradient.stops[1].color = __handlePosList[index].color
        }
        updateHandles()
    }

    /*!
        Changes the color (col) of handle (index).
    */
    function setHandleColor(color, index)
    {
        //default values
        index = typeof index !== "undefined" ? index : [0]
        index = Array.isArray(index) ? index : [index]
        color = Array.isArray(color) ? color : [color]

        for (var i = 0; i < index.length; i++)
        {
            var indexToChange = index[i]
            if (indexToChange <= __handleCount - 1)
            {
                __handlePosList[indexToChange].color = color[i]
            }
            else
            {
                console.log("WARNING WGSlider: Tried to change the color of a handle that does not exist")
            }
        }
        updateHandles()
    }

    /*!
        applies the deltas to the handle color at (index) and returns the new color
    */
    function getChangedHandleColor(rDelta,gDelta,bDelta,aDelta, index)
    {
        index = typeof index !== "undefined" ? index : 0

        var handleColor = slider.__handlePosList[index].color

        handleColor.r = Math.max(Math.min(handleColor.r += rDelta, 1),0)
        handleColor.g = Math.max(Math.min(handleColor.g += gDelta, 1),0)
        handleColor.b = Math.max(Math.min(handleColor.b += bDelta, 1),0)
        handleColor.a = Math.max(Math.min(handleColor.a += aDelta, 1),0)

        return handleColor
    }

    function getHandleColor(index) {

        index = typeof index !== "undefined" ? index : 0

        if(index <= __handleCount - 1)
        {
            return __handlePosList[index].color
        }
        else
        {
            console.log("WARNING WGGradientSlider: Tried to return a color for a handle that does not exist")
            return -1
        }
    }

    /*!
        This updates the color, mininmum and maximum values for the slider handles
    */
    function updateHandles()
    {
        for (var i = 0; i < __handlePosList.length; i++)
        {
            var iHandle = __handlePosList[i]
            if (iHandle.objectName = "SliderHandle")
            {

                if (i === 0)
                {
                    iHandle.minimumValue = Qt.binding(function() {return slider.minimumValue})
                    iHandle.minColor = iHandle.color
                }
                else
                {
                    iHandle.minimumValue = Qt.binding(function() {
                        if(i - 1 < slider.__handlePosList.length)
                            return slider.__handlePosList[i - 1].value
                        return slider.minimumValue
                    })
                    iHandle.minColor = slider.__handlePosList[i - 1].color
                }
                if (i === slider.__handlePosList.length - 1)
                {
                    iHandle.maximumValue = Qt.binding(function() {return slider.maximumValue})
                }
                else
                {
                    iHandle.maximumValue = Qt.binding(function() {
                        return slider.__handlePosList[i + 1].value
                    })
                }
            }
            else
            {
                i--
                console.log("WARNING WGGradientSlider: Child object found in __handlePosList that isn't a valid slider handle")
            }
        }
    }

    Component.onCompleted: {
        updateHandles()
    }
}
