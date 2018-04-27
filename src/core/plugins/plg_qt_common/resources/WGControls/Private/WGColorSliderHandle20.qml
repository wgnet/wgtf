import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import WGControls 2.0
import WGControls.Private 2.0


/*!
 \ingroup wgcontrols
 \brief A Slider handle intended for the WGColorSlider Control that fills the entire bar.
*/

WGSliderHandle {
    objectName: "SliderHandle"
    id: sliderHandle

    handleHeight: defaultSpacing.minimumRowHeight
    handleWidth: 8

    handleStyle: WGButtonFrame
    {
        id: defaultHandleFrame
        implicitHeight: handleHeight
        implicitWidth: handleWidth
        color: parentSlider.enabled ? parentSlider.hoveredHandle == handleIndex ? "white" : palette.overlayLighterShade : palette.darkestShade
        borderColor: palette.overlayDarkerShade
        innerBorderColor: parentSlider.__activeHandle == handleIndex && parentSlider.activeFocus ? palette.highlightShade : "transparent"

        radius: defaultSpacing.halfRadius
    }
}
