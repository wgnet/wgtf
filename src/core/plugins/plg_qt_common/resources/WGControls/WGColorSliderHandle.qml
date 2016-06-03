import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import WGControls 1.0

/*!
 \A Slider handle intended for the WGColorSlider Control

 Generated dynamically by WGColorSlider. Don't use separately.
*/

WGSliderHandle {
    objectName: "WGColorSliderHandle"
    id: sliderHandle

    minimumValue: handleIndex == 0 ? parentSlider.minimumValue : parentSlider.__handlePosList.children[handleIndex - 1].value

    maximumValue: {
        if (parentSlider.linkColorsToHandles)
        {
            if (handleIndex == parentSlider.positionData.length - 1)
            {
                parentSlider.maximumValue
            }
            else
            {
                parentSlider.__handlePosList.children[handleIndex + 1].value
            }

        }
        else
        {
            parentSlider.maximumValue
        }
    }

    onValueChanged: {
        if (parentSlider.linkColorsToHandles && parentSlider.__barLoaded)
        {
            parentSlider.positionData[handleIndex] = value
            parentSlider.changeValue(value, handleIndex)
            parentSlider.updateColorBars()
        }
        if (!parentSlider.linkColorsToHandles && parentSlider.__barLoaded)
        {
            if (handleIndex == 0)
            {
                parentSlider.value = sliderHandle.value
            }
        }
    }

    handleStyle: WGButtonFrame
    {
        id: defaultHandleFrame
        implicitHeight: parentSlider.__horizontal ? parentSlider.height - 2 : 8
        implicitWidth: parentSlider.__horizontal ? 8 : parentSlider.width - 2
        color: parentSlider.__hoveredHandle == handleIndex ? "white" : palette.overlayLighterShade
        borderColor: palette.overlayDarkerShade
        innerBorderColor: parentSlider.__activeHandle == handleIndex && parentSlider.activeFocus ? palette.highlightShade : "transparent"

        radius: defaultSpacing.halfRadius
    }

    function updateValueBinding()
    {
        minimumValue = parentSlider.minimumValue
        maximumValue = parentSlider.maximumValue

        value = Qt.binding(function() {return parentSlider.positionData[handleIndex]})
    }

    function updateMinMaxBinding()
    {
        if (handleIndex == 0)
        {
            minimumValue = Qt.binding(function() {return parentSlider.minimumValue})
        }
        else
        {
            minimumValue = Qt.binding(function() {return parentSlider.__handlePosList.children[handleIndex - 1].value})
        }

        if (handleIndex == parentSlider.positionData.length - 1)
        {
            maximumValue = Qt.binding(function() {return parentSlider.maximumValue})
        }
        else
        {
            maximumValue = Qt.binding(function() {return parentSlider.__handlePosList.children[handleIndex + 1].value})
        }
    }
}
