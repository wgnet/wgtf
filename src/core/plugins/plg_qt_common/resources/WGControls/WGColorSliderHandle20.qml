import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0

/*!
 \A Slider handle intended for the WGColorSlider Control that fills the entire bar.
*/

WGSliderHandle {
    objectName: "SliderHandle"
    id: sliderHandle

    handleStyle: WGButtonFrame
    {
        id: defaultHandleFrame
        implicitHeight: parentSlider.__handleHeight
        implicitWidth: 8
        color: parentSlider.hoveredHandle == handleIndex ? "white" : palette.overlayLighterShade
        borderColor: palette.overlayDarkerShade
        innerBorderColor: parentSlider.__activeHandle == handleIndex && parentSlider.activeFocus ? palette.highlightShade : "transparent"

        radius: defaultSpacing.halfRadius
    }
}
