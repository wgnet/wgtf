import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import Qt.labs.controls 1.0 as Labs

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
 \brief A drop down menu for selecting icon size.
*/

WGPushButton {
    id: displayButton
    objectName: "WGIconSizeMenu"
    WGComponent { type: "WGIconSizeMenu20" }
    implicitWidth: 100

    property QtObject assetBrowser

    text: assetBrowser.showIcons ? (assetBrowser.iconSize + "px Icons") : "List View"

    // IconSize control pulldown
    WGPopup {
        id: popup
        openPolicy: openOnPress
        closePolicy: Labs.Popup.CloseOnPressOutsideParent | Labs.Popup.CloseOnEscape

        exitMouseAreaOnTop: false

        remainOpen: slider.pressed
        Rectangle {
            id: sizeMenu
            height: defaultSpacing.minimumRowHeight * 5 + defaultSpacing.doubleMargin
            width: 140

            color: palette.mainWindowColor
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.darkColor

            WGSlider {
                id: slider
                objectName: "slider"
                stepSize: 16
                minimumValue: 16
                maximumValue: 256
                width: 16
                height: menuItems.childrenRect.height
                orientation: Qt.Vertical
                anchors.top: sizeMenu.top
                anchors.left: sizeMenu.left
                anchors.margins: {left: 2; right: 2; top: 5; bottom: 5}
                rotation: 180

                onChangeValue: {
                    assetBrowser.iconSize = val
                }

                onSliderHoveredChanged: {
                    popup.popupHovered = sliderHovered
                }

                WGSliderHandle {
                    id: sliderHandle
                    minimumValue: slider.minimumValue
                    maximumValue: slider.maximumValue
                    showBar: true

                    value: assetBrowser.iconSize

                    Connections {
                        target: assetBrowser
                        onIconSizeChanged: {
                            sliderHandle.value = iconSize
                        }
                    }
                }
            }

            // Iconsize Pulldown Buttons
            Item {
                id: menuItems
                height: childrenRect.height
                width: 110
                anchors.top: sizeMenu.top
                anchors.left: slider.right
                anchors.margins: {left: 2; right: 2; top: 5; bottom: 5}

                WGPushButton {
                    id: listViewButton
                    objectName: "listViewButton"
                    anchors.top: menuItems.top
                    anchors.left: menuItems.left
                    width: menuItems.width
                    text: "List View"

                    onHoveredChanged: {
                        popup.popupHovered = hovered
                    }

                    onClicked: {
                        assetBrowser.iconSize = 0
                    }
                }
                WGPushButton {
                    id: smallIconsButton
                    objectName: "smallIconsButton"
                    anchors.top: listViewButton.bottom
                    anchors.left: menuItems.left
                    width: menuItems.width
                    text: "Small Icons"

                    onHoveredChanged: {
                        popup.popupHovered = hovered
                    }

                    onClicked: {
                        assetBrowser.iconSize = 48
                    }
                }
                WGPushButton {
                    id: mediumIconsButton
                    objectName: "mediumIconsButton"
                    anchors.top: smallIconsButton.bottom
                    anchors.left: menuItems.left
                    width: menuItems.width
                    text: "Medium Icons"

                    onHoveredChanged: {
                        popup.popupHovered = hovered
                    }

                    onClicked: {
                        assetBrowser.iconSize = 96
                    }
                }
                WGPushButton {
                    id: largeIconsButton
                    objectName: "largeIconsButton"
                    anchors.top: mediumIconsButton.bottom
                    anchors.left: menuItems.left
                    width: menuItems.width
                    text: "Large Icons"

                    onHoveredChanged: {
                        popup.popupHovered = hovered
                    }

                    onClicked: {
                        assetBrowser.iconSize = 128
                    }
                }
                WGPushButton {
                    id: extraLargeIconsButton
                    objectName: "extraLargeIconsButton"
                    anchors.top: largeIconsButton.bottom
                    anchors.left: menuItems.left
                    width: menuItems.width
                    text: "Extra Large Icons"

                    onHoveredChanged: {
                        popup.popupHovered = hovered
                    }

                    onClicked: {
                        assetBrowser.iconSize = 256
                    }
                }
            }
        }
    }

    MouseArea {
        id: sliderCoverMouseArea
        objectName: "sliderCoverMouseArea"

        anchors.fill: parent
        propagateComposedEvents: true
        acceptedButtons: Qt.NoButton

        onWheel: {
            if (wheel.angleDelta.y > 0)
            {
                sliderHandle.range.decreaseSingleStep()
            }
            else
            {
                sliderHandle.range.increaseSingleStep()
            }
        }
    }
}

