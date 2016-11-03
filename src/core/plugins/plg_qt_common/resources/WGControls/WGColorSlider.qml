import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls.Styles 1.0
import WGControls.Private 1.0

/*!
 \ingroup wgcontrols
 \brief Slider with a color gradient background.
 Purpose: Allow the user to select a color visually

 This slider has two main variations:

 A color value slider where the gradient background is visual only. This is intended to
 be used for RGB sliders, HSL sliders etc. where changing the handle position selects
 a color on that gradient. The gradient itself is not changed by the handles position,
 (but could be changed by other means.) It is generally intended that these sliders
 would only have one handle.

 The following example creates a grayscale or brightness slider with one handle.
 
Example:
\code{.js}
    WGColorSlider {
        Layout.fillWidth: true
        minimumValue: 0
        maximumValue: 255
        stepSize: 1
        colorData: [Qt.rgba(0,0,0,1), Qt.rgba(1,1,1,1)]
        positionData: [0, 255]
        value: 128
        linkColorsToHandles: false
    }
\endcode

The second variation is a gradient or ramp slider where moving the handles changes the gradient
positions and individual colors in the gradient can be edited by the user. It is intended that
this slider could have any number of handles.

Handles can be added with Shift+left click anywhere in the slider.
Handles can be deleted by Ctrl+left clicking on a handle
Handles colors can be changed by double clicking them.

It is not recommended to set handleClamp: true if linkColorsToHandles: true or the handles will
not match the gradient transitions. handleClamp is false if linkColorsToHandles is true by default.

The following example creates a gradient from red to yellow to white with three handles.

Example:
\code{.js}
    WGColorSlider {
        Layout.fillWidth: true
        minimumValue: 0
        maximumValue: 100
        stepSize: 0.1
        colorData: [Qt.rgba(1,0,0,1), Qt.rgba(1,1,0,1), Qt.rgba(1,1,1,1)]
        positionData: [25, 50, 75]
        linkColorsToHandles: true
    }
\endcode

\todo - Test orientation = vertical. Create vertical slider. Remove option here.
      - Hook up to proper C++ data.
      - Make it work with Undo, macros etc.
      - Make multi handle slider with linkColorsToHandles: true work in vertical orientation (make all multi handle sliders work in vertical tbh).
      - Make safer with bad data, colorData.length != posData.length, bad colors etc.
      - Fix slight difficulty grabbing handles at max and min values when handleClamp: false
      - Get rid of spammy undefined messages when handle is deleted.
*/

WGSlider {
    id: sliderFrame
    objectName: "WGColorSlider"
    WGComponent { type: "WGColorSlider" }

    minimumValue: 0
    maximumValue: linkColorsToHandles ? 100 : 255

    stepSize: linkColorsToHandles ? 0.1 : 1

    /*!
        This property determines whether the handleValues will be based off the positionData or off
        a general value. It is the main property that determines whether the slider will behave like
        a standard color slider or a gradient/ramp slider.

        If true, the handles will change the gradient positions when they are moved.

        Setting linkColorsToHandles: true will enable the color picker, adding and deleting handles
        disable handleClamp and use the offset arrow style of handles by default.

        The default value is \c false
    */

    property bool linkColorsToHandles: false

    /*!
        This array contains the values for the colors in the gradient as set contained in colorData.concat:
        These values are in slider value units, not QML gradient values.

        If linkColorsToHandles: true, the first and last colors will extend to
        the edges of the slider. In this case do not add colors/positions that correspond to the slider
        minimumValue and maximumValue.
    */

    //TODO: Data should probably be in C++

    property var positionData: []

    /*!
        This array contains the gradient color values for the points in positionData.

        If linkColorsToHandles: true, the first and last colors will extend to
        the edges of the slider. In this case do not add colors/positions that correspond to the slider
        minimumValue and maximumValue.
    */

    //TODO: Data should probably be in C++

    property var colorData: []

    /*!
        This value determines the number of handles in the slider. The default value
        is based on linkColorsToHandles.

        If true it will create handles for each color/position value.

        If false it will create one handle.

        If you need more handles, set this value to 0 and create WGColorHandles manually as with WGSlider.

    */

    /*!
        This value determines whether double clicking a handle should display a color picker.

        This is intended mostly to be used with linkColorsToHandles = true.constructor
    */
    property bool useColorPicker: linkColorsToHandles

    /*!
        This value determines whether the alpha value will appear when changing a color handle via a color picker.
    */
    property bool showAlphaChannel: true

    /*!
        This value determines the number of handles generated for the slider

        It is not intended that this be changed unless you need multiple handles
        with linkColorsToHandles = false. If linkColorsToHandles = true,
        the control will create handles based off the color and position data.
    */
    property int handles: linkColorsToHandles ? positionData.length : 1

    /*!
        This value determines whether the user can add and delete handles from the slider.
        This is intended to be used with \c linkColorsToHandles

        The default value is true if linkColorsToHandles is true
    */
    property bool addDeleteHandles: linkColorsToHandles

    /*!
        This value determines both the vertical offset of the handles AND the additional margin
        below the slider groove. This works well with handles that look like arrows to make them
        sit below the groove.

        The default value is 0.
    */
    property int handleVerticalOffset: 0

    /*!
        This string determines the the component for the slider handle.

        The default value is WGColorSliderHandle.
        It can be set to any Item based component.
    */
    property Component handleStyle: WGColorSliderHandle{}

    implicitHeight: defaultSpacing.minimumRowHeight

    //handleClamp: true will make the handles and gradient transitions not line up
    //at the edges if linkColorsToHandles: true
    handleClamp: !linkColorsToHandles

    grooveClickable: !linkColorsToHandles

    style: WGColorSliderStyle{}

    /*! \internal */
    property bool __barLoaded: false

    /*! \internal */
    property var __colorBarModel: ListModel {}


    property var colorPicker: ColorDialog {
        id: colorPicker
        title: "Please choose a color"
        showAlphaChannel: sliderFrame.showAlphaChannel
        property int currentColorIndex: -1

        onAccepted: {
            if(currentColorIndex >= 0)
            {
                colorData[currentColorIndex] = Qt.rgba(colorPicker.color.r,colorPicker.color.g,colorPicker.color.b,colorPicker.color.a)
                colorModified(currentColorIndex)
                currentColorIndex = -1
                updateColorBars()
            }
        }

        onRejected: {
            currentColorIndex = -1
        }
    }

    /*!
        This signal will update the gradient bars as well as the small color boxes
        in the handles if offsetArrowButtons: true
    */
    signal updateColorBars()

    /*!
        This signal is fired when a point is added to the data with addData()
    */
    signal pointAdded(int index)

    /*!
        This signal is fired when a point is removed from the data with deleteData()
    */
    signal pointRemoved(int index)

    /*!
        This signal is fired when a point's color is changed via the color picker
    */
    signal colorModified(int index)

    signal changeValue(real val, int handleIndex)

    onChangeValue:
    {
        if (!linkColorsToHandles && __handlePosList.children.length > 0)
        {
            __handlePosList.children[handleIndex].value = val;
        }
    }

    function addData (index, pos, col)
    {
        //TODO: Data should be changed via C++
        __barLoaded = false
        positionData.splice(index,0,pos)
        colorData.splice(index,0,col)
        pointAdded(index)
    }

    function deleteData (index)
    {
        //TODO: Data should be changed via C++
        if(positionData.length && colorData.length > 1)
        {
            __barLoaded = false
            positionData.splice(index,1)
            colorData.splice(index,1)
            pointRemoved(index)
        }
    }

    function updateData()
    {
        if (__barLoaded) {
            //Turn off updating values, create a new handle and update everything
            __barLoaded = false

            if (positionData.length < __handlePosList.children.length) {
                for (var i = 0; i++; i < __handlePosList.children.length - positionData.length)
                {
                    __handlePosList.children[i].destroy()
                }
            }
            else if (positionData.length > __handlePosList.children.length)
            {
                createHandles(positionData.length - __handlePosList.children.length)
            }

            updateHandles()

            //easier to wipe the color bars and re-create them
            __colorBarModel.clear()

            createBars()

            __draggable = true

            __barLoaded = true
        }
    }

    function updateHandles()
    {
        //update the value and index of the handles
        for (var i = 0; i < __handlePosList.children.length; i++)
        {
            __handlePosList.children[i].handleIndex = i
            __handlePosList.children[i].updateValueBinding()

        }

        //update the min max values to reflect the new order and values
        for (var j = 0; j < __handlePosList.children.length; j++)
        {
            __handlePosList.children[j].updateMinMaxBinding()
        }
    }

    function createHandles(handlesToCreate)
    {
        for (var i = 0; i < handlesToCreate; i++)
        {
            if (handleStyle.status === Component.Ready)
            {
                var newObject = handleStyle.createObject(__handlePosList, {
                                           "value": linkColorsToHandles ? positionData[i] : sliderFrame.value,
                                           "showBar": false
                                       });
            }
        }
    }

    function createBars()
    {
        //create the first bar if linkColorsToHandles
        if (linkColorsToHandles)
        {
            __colorBarModel.append({"minValue": minimumValue,
                                     "maxValue": positionData[0],
                                     "minColorVal": 0,
                                     "maxColorVal": 0
                                   });
        }
        //create the middle bars if linkColorsToHandles
        //or create all the bars if !linkColorsToHandles
        for (var j = 0; j < colorData.length - 1; j++)
        {
            __colorBarModel.append({"minValue": positionData[j],
                                     "maxValue": positionData[j+1],
                                     "minColorVal": j,
                                     "maxColorVal": j+1
                                 });
        }
        //create the last bar if linkColorsToHandles
        if (linkColorsToHandles)
        {
            __colorBarModel.append({"minValue": positionData[positionData.length - 1],
                                     "maxValue": maximumValue,
                                     "minColorVal": colorData.length - 1,
                                     "maxColorVal": colorData.length - 1
                                 });
        }
    }

    onPointAdded:
    {
        //Turn off updating values, create a new handle and update everything
        __barLoaded = false

        if (handleStyle.status === Component.Ready)
        {
            var newObject = handleStyle.createObject(__handlePosList, {
                                        "showBar": false
                                   });
        }
        updateHandles()


        //easier to wipe the color bars and re-create them
        __colorBarModel.clear()

        createBars()

        __draggable = true

        __barLoaded = true
    }

    onPointRemoved:
    {
        //Turn off updating values, destroy the unneeded handle and update everything
        __barLoaded = false

        for (var i = 0; i < __handlePosList.children.length; i++)
        {
            if (__handlePosList.children[i].handleIndex == index)
            {
                var handleToDestroy = __handlePosList.children[i]
            }
        }

        // This parenting change is needed as the object seems to take
        // a while to destroy and the handles won't update properly
        handleToDestroy.parent = sliderFrame
        handleToDestroy.visible = false
        handleToDestroy.destroy()

        updateHandles()

        //easier to wipe the color bars and re-create them
        __colorBarModel.clear()

        createBars()

        __barLoaded = true
    }

    onUpdateColorBars: {
        if (__barLoaded)
        {
            if(linkColorsToHandles)
            {
                __colorBarModel.set(0,{"minValue": minimumValue,
                                         "maxValue": positionData[0],
                                         "minColorVal": 0,
                                         "maxColorVal": 0
                                     });
            }

            var startBar = linkColorsToHandles ? 1 : 0
            var endBar = linkColorsToHandles ? __colorBarModel.count - 2 : __colorBarModel.count - 1

            for (startBar; startBar <= endBar; startBar++)
            {
                __colorBarModel.set(startBar,{"minValue": positionData[startBar - 1],
                                         "maxValue": positionData[startBar],
                                         "minColorVal": startBar - 1,
                                         "maxColorVal": startBar
                                     });
            }

            if(linkColorsToHandles)
            {
                __colorBarModel.set(__colorBarModel.count - 1,{"minValue": positionData[positionData.length - 1],
                                      "maxValue": maximumValue,
                                      "minColorVal": colorData.length - 1,
                                      "maxColorVal": colorData.length - 1
                                     });
            }
        }
    }

    //delete a handle
    onHandleCtrlClicked: {
        if (addDeleteHandles)
        {
            deleteData(index)
        }
    }

    //pick a color using ColorDialog
    onSliderDoubleClicked: {
        if (useColorPicker)
        {
            colorPicker.color = colorData[index]
            colorPicker.currentColorIndex = index
            colorPicker.open()
        }
    }

    Component.onCompleted: {
        //generate handles
        if(handles > 0)
        {
            createHandles(handles)
        }

        //create colored bars
        createBars()

        __barLoaded = true
    }
}
