import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \A Control for changing the maximum brightnessRange to HDR values ( < 1.0)

Example:
\code{.js}
WGHDRSelector {

    rgbaColor: myVec4Color
    brightnessRange: myBrightnessRange

    onChangeBrightness: {
        myBrightnessRange = br
    }
}
\endcode
*/

WGAbstractColor {
    id: hdrSelector
    objectName: "WGHDRSelector"
    WGComponent { type: "WGHDRSelector20" }

    /*!
        The maximum brightness range that is edited by this control
    */
    property real brightnessRange: 1.0

    signal changeBrightness (var br)

    onBrightnessRangeChanged: {
        switch (brightnessRange)
        {
            case 1.0: radio1.checked = true; break;
            case 10.0: radio2.checked = true; break;
            case 100.0: radio3.checked = true; break;
            default: radio1.checked = true; break;
        }
    }

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            ExclusiveGroup {
                id: brightnessGroup
            }

            WGLabel {
                text: "  HDR Range: "
                Layout.fillWidth: true
            }

            WGRadioButton {
                id: radio1
                text: "1x"
                exclusiveGroup: brightnessGroup
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight

                checked: brightnessRange == 1.0

                onCheckedChanged: {
                    if (checked)
                    {
                        changeBrightness(1.0)
                    }
                }
            }
            WGRadioButton {
                id: radio2
                text: "10x"
                exclusiveGroup: brightnessGroup
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight

                checked: brightnessRange == 10.0

                onCheckedChanged: {
                    if (checked)
                    {
                        changeBrightness(10.0)
                    }
                }
            }
            WGRadioButton {
                id: radio3
                text: "100x"
                exclusiveGroup: brightnessGroup
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight

                checked: brightnessRange == 100.0

                onCheckedChanged: {
                    if (checked)
                    {
                        changeBrightness(100.0)
                    }
                }
            }
        }


        //temporary hdr preview that's probably wrong
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            Image {
                anchors.fill: parent
                source: "icons/bw_check_6x6.png"
                fillMode: Image.Tile
            }

            Rectangle {
                id: preview
                height: parent.width
                width: parent.height

                y: width

                rotation: -90

                transformOrigin: Item.TopLeft

                property vector3d ldrColor: {
                    return tonemap(Qt.vector3d(hdrSelector.rgbaColor.x,hdrSelector.rgbaColor.y,hdrSelector.rgbaColor.z))
                }

                border.width: defaultSpacing.standardBorderSize
                border.color: palette.midLightColor

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: Qt.rgba(preview.ldrColor.x,preview.ldrColor.y,preview.ldrColor.z,hdrSelector.rgbaColor.w)
                    }
                    GradientStop {
                        position: 0.5
                        color: Qt.rgba(hdrSelector.rgbaColor.x,hdrSelector.rgbaColor.y,hdrSelector.rgbaColor.z,hdrSelector.rgbaColor.w)
                    }
                    GradientStop {
                        position: 1
                        color: Qt.rgba(preview.ldrColor.x,preview.ldrColor.y,preview.ldrColor.z,hdrSelector.rgbaColor.w)
                    }
                }
            }
        }
    }
}
