import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls.Styles 2.0
import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
 \brief Slider with a color gradient background.
 Purpose: Allow the user to select a color visually. This is intended to
 be used for RGB sliders, HSL sliders etc. where changing the handle position selects
 a color on that gradient. It is generally intended that these sliders
 would only have one handle.

 The following example creates a grayscale or brightness slider with one handle.

Example:
\code{.js}
        WGColorSlider {
            Layout.fillWidth: true
            minimumValue: 0
            maximumValue: 255
            stepSize: 1
            value: 128
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#000000"
                }
                GradientStop {
                    position: 1
                    color: "#FFFFFF"
                }
            }
        }
\endcode

 \todo Test orientation = vertical. Create vertical slider. Remove option here
*/

WGSlider {
    id: sliderFrame
    objectName: "WGColorSlider20"
    WGComponent { type: "WGColorSlider20" }

    minimumValue: 0
    maximumValue: 255

    stepSize: 1

    property Gradient gradient: Gradient {
        GradientStop {
            position: 0
            color: "#000000"
        }
        GradientStop {
            position: 1
            color: "#FFFFFF"
        }
    }

    handleType: WGColorSliderHandle{}

    handleClamp: true
    grooveClickable: true

    style: WGColorSliderStyle{}
}
