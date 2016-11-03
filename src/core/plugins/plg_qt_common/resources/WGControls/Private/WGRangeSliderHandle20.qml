import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
  \ingroup wgcontrols
  \brief A Slider handle intended for the WGRangeSlider Control. It has animation properties to handle overlapping it's partner handle.
   Shouldn't really be needed to be used on it's own but may be useful in some other kind of slider.
*/

WGSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"

    /*!
        A paired handle that handles the max or min value in a range slider.
    */
    property QtObject rangePartnerHandle: sliderHandle

    /*!
        True if the handle is the maximum value in a range slider
    */
    property bool maxHandle: false

    /*!
        This is the Component for the handle style.

        This can be any Item based component.
    */
    property Component handleStyle: Item {
        implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
        implicitHeight: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2

        WGButtonFrame{
            color: parentSlider.enabled ? handleColor : palette.mainWindowColor
            borderColor: parentSlider.enabled ? palette.darkerShade : palette.darkShade
            highlightColor: parentSlider.hoveredHandle === handleIndex ? palette.lighterShade : "transparent"
            innerBorderColor: parentSlider.__activeHandle === handleIndex && parentSlider.activeFocus ? palette.highlightShade : "transparent"
            implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
            implicitHeight: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2

            Behavior on height{
                NumberAnimation {
                    duration: 120
                    easing {
                        type: Easing.OutCirc
                        amplitude: 1.0
                        period: 0.5
                    }
                }
            }

            height: __overlapping ? implicitHeight / 2 : implicitHeight
            width: implicitWidth

            Component.onCompleted: {
                if (maxHandle)
                {
                    anchors.bottom = parent.bottom
                }
                else
                {
                    anchors.top = parent.top
                }
            }
        }
    }

    /*! \internal */
    property bool __overlapping: {
        if(rangePartnerHandle != sliderHandle)
        {
            if((range.position >= rangePartnerHandle.range.position - parentSlider.__handleWidth/2) && (range.position <= rangePartnerHandle.range.position + parentSlider.__handleWidth/2))
            {
                return true
            }
            else
            {
                return false
            }
        }
        else
        {
            return false
        }
    }
}

