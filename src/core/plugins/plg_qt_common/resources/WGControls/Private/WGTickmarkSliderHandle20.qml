import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
 \brief A Slider handle that looks like a downward pointing arrow used when showTickmarkLabels == true

Example:
\code{.js}
WGSlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    stepSize: 1.0

    showTickmarkLabels: true
}
\endcode
*/

WGSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"

    property Component handleStyle: Item{
        implicitWidth: handleWidth
        implicitHeight: handleHeight

        Canvas {
            id: handleCanvas
            width: handleWidth
            height: handleHeight
            y: 1

            Connections {
                target: parentSlider

                onHoveredHandleChanged: {
                    if (!parentSlider.__handleMoving)
                    {
                        handleCanvas.requestPaint()
                    }
                }
                onActiveFocusChanged: {
                    if (!parentSlider.__handleMoving)
                    {
                        handleCanvas.requestPaint()
                    }
                }
                onEnabledChanged: {
                    if (!parentSlider.__handleMoving)
                    {
                        handleCanvas.requestPaint()
                    }
                }
            }

            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = parentSlider.enabled ? handleColor : palette.mainWindowColor;
                ctx.strokeStyle = parentSlider.enabled ? palette.darkerShade : palette.darkShade;
                ctx.lineWidth = 1;
                ctx.beginPath();
                ctx.moveTo(0, 0);
                ctx.lineTo(0, height / 2)
                ctx.lineTo((width / 2) - 1, height)
                ctx.lineTo((width / 2) + 1, height)
                ctx.lineTo(width, height / 2)
                ctx.lineTo(width, 0)
                ctx.closePath()
                ctx.fill()
                ctx.stroke()
                ctx.fillStyle = parentSlider.hoveredHandle === handleIndex ? palette.lighterShade : "transparent"
                ctx.moveTo(1,1)
                ctx.lineTo(1, height / 2)
                ctx.lineTo(width / 2, height)
                ctx.lineTo(width - 1, height / 2)
                ctx.lineTo(width - 1, 1)
                ctx.closePath()
                ctx.fill()
            }
        }

    }

    handleWidth: Math.round(defaultSpacing.minimumRowHeight * 0.5)
    handleHeight: Math.round(defaultSpacing.minimumRowHeight * 0.5)
}

