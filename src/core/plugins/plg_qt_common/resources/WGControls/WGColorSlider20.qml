import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import WGControls 2.0

//TODO: Test orientation = vertical. Create vertical slider. Remove option here

/*!
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
*/

WGSlider {
    id: sliderFrame
    objectName: "WGColorSlider"

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
