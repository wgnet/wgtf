import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import WGControls 2.0
import WGControls.Styles 2.0
import WGControls.Private 2.0
import WGControls.Global 2.0

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
            createColorHandle(.25,handleStyle,__handlePosList.length, Qt.vector4d(1,0,0,1))
            createColorHandle(.5,handleStyle,__handlePosList.length, Qt.vector4d(1,1,0,1))
            createColorHandle(.75,handleStyle,__handlePosList.length, Qt.vector4d(1,1,1,1))
        }
    }
\endcode
*/

WGSlider {
    id: slider
    objectName: "WGColorSlider"
    WGComponent { type: "WGGradientSlider20" }

    /*!
        This value determines whether double clicking a handle should display a color picker.
    */
    property bool useColorPicker: true

    property bool defaultColorDialog: false

    property bool useHDR: false

    property var tonemap: function(col) { return col; }

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
    readonly property bool colorPickerOpen: __dialogInstance != null ? __dialogInstance.visible : false

    minimumValue: 0

    maximumValue: 1

    stepSize: .001

    handleType: WGGradientSliderHandle{}

    implicitHeight: defaultSpacing.minimumRowHeight

    handleClamp: false

    grooveClickable: false

    createInitialHandle: false

    style: WGGradientSliderStyle{}

    /*! internal */
    property var __dialogInstance: null

    property int colorEditingIndex: -1

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
                            var newCol = slider.__handlePosList[index - 1].color
                            var tm_Col = tonemap(Qt.vector3d(newCol.x, newCol.y, newCol.z))
                            colorBar.color = Qt.rgba(tm_Col.x, tm_Col.y, tm_Col.z, newCol.w)
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
                        if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier ||  mouse.modifiers & Qt.AltModifier) && slider.addDeleteHandles)
                        {
                            if (slider.hoveredHandle === -1)
                            {
                                var framePos = mapToItem(gradientFrame, mouseX, mouseY)
                                addPointToBar(framePos)
                            }
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
    signal colorModified(var color, int index)

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
        if (useColorPicker && mouseModifiers === Qt.NoModifier)
        {
            makeFakeMouseRelease();
            beginUndoFrame();
            colorEditingIndex = index
            openColorDialog(__handlePosList[index].color)
        }
    }

    Connections {
        target: __dialogInstance
        ignoreUnknownSignals: true

        onAccepted: {

            if(colorEditingIndex >= 0)
            {
                if (defaultColorDialog)
                {
                    colorModified(Qt.vector4d(selectedValue.r,selectedValue.g,selectedValue.b,selectedValue.a), colorEditingIndex)
                    endUndoFrame();
                    colorEditingIndex = -1
                }
                else
                {
                    colorModified(selectedValue, colorEditingIndex)
                    endUndoFrame();
                    colorEditingIndex = -1
                }
            }
        }

        onRejected: {
            abortUndoFrame();
            colorEditingIndex = -1
        }

        onClosed: {
            __dialogInstance = null
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

        newHandle.tonemap = slider.tonemap

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

            var tmpCol = tonemap(Qt.vector3d(__handlePosList[index].color.x, __handlePosList[index].color.y, __handlePosList[index].color.z))
            var tm_Col = Qt.rgba(tmpCol.x, tmpCol.y, tmpCol.z, __handlePosList[index].color.w)
            __handlePosList[index].gradient.stops[1].color = tm_Col

            var tmpCol = tonemap(Qt.vector3d(__handlePosList[index].minColor.x, __handlePosList[index].minColor.y, __handlePosList[index].minColor.z))
            var tm_MinCol = Qt.rgba(tmpCol.x, tmpCol.y, tmpCol.z, __handlePosList[index].minColor.w)
            __handlePosList[index].gradient.stops[1].color = tm_MinCol
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

        handleColor.x = Math.max(Math.min(handleColor.x += rDelta, 1),0)
        handleColor.y = Math.max(Math.min(handleColor.y += gDelta, 1),0)
        handleColor.z = Math.max(Math.min(handleColor.z += bDelta, 1),0)
        handleColor.w = Math.max(Math.min(handleColor.w += aDelta, 1),0)

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

    /*! This function opens the desired dialog box.
    */
    function openColorDialog(curColor) {
        if (defaultColorDialog) {
            WGDialogs.defaultColorPickerDialog.close()
            __dialogInstance = WGDialogs.defaultColorPickerDialog

            //MacOS default color picker cannot be modal.
            __dialogInstance.modality = Qt.NonModal
            __dialogInstance.showAlphaChannel = colorButton.showAlphaChannel
            __dialogInstance.open(600, 380, Qt.rgba(curColor.x, curColor.y, curColor.z, curColor.w))
        }
        else {
            __dialogInstance = WGDialogs.customColorPickerDialog
            if(typeof viewId != "undefined")
            {
                __dialogInstance.viewId = viewId
            }
            if(typeof viewPreference != "undefined")
            {
                __dialogInstance.viewPreference = viewPreference
            }
            __dialogInstance.modality = Qt.ApplicationModal
            __dialogInstance.useHDR = slider.useHDR
            __dialogInstance.tonemap = slider.tonemap
            __dialogInstance.showAlphaChannel = slider.showAlphaChannel
            __dialogInstance.open(600, 380, curColor)
        }
    }

    /*! This function closes the desired dialog box depending on whether useAssetBrowser == true or not.
    */
    function closeColorDialog() {
        __dialogInstance.close()
    }

    Component.onCompleted: {
        updateHandles()
    }
}
