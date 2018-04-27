import QtQuick 2.5
import QtQuick.Controls 1.4
import WGControls 2.0

/*!
 \ingroup wgcontrols
 \brief Standard QML Icon overlay.

\code{.js}
WGIconOverlay {
    icon: iconToOverlayOver
    position: topLeftCorner
    source: "icons/myImage.png";
}
\endcode
*/

Item {
    id: iconOverlay
    objectName: "WGIconOverlay"
    WGComponent { type: "WGIconOverlay20" }

    readonly property int topLeftCorner: 1
    readonly property int topRightCorner: 2
    readonly property int bottomLeftCorner: 4
    readonly property int bottomRightCorner: 3

    readonly property int defaultPosition: bottomRightCorner

    property int iconXOffset: iconSize > 32 ? Math.round(iconSize / 10) : 0
    property int iconYOffset: iconSize > 32 ? Math.round(iconSize / 14) : 0

    property int iconSize: icon != null && typeof icon.iconSize != 'undefined' ? icon.iconSize : parent.height

    property int position: defaultPosition
    property alias source: overlay.source
    property var icon: null

    height: iconSize
    width: iconSize

    WGImage {
        id: overlay
        asynchronous: true
        visible: status === Image.Ready
        smooth: false

        states: [
            State {
                name: "none"
                when: !internal.validIcon

                AnchorChanges {
                    target: overlay
                }
                PropertyChanges {
                    target: overlay
                    x: iconXOffset
                    y: iconYOffset
                }
            },
            State {
                name: "topLeft"
                when: internal.position === topLeftCorner

                PropertyChanges {
                    target: overlay
                    x: iconXOffset
                    y: iconYOffset
                }
            },
            State {
                name: "topRight"
                when: internal.position === topRightCorner

                PropertyChanges {
                    target: overlay
                    x: iconOverlay.width - overlay.width - iconXOffset
                    y: iconYOffset
                }
            },
            State {
                name: "bottomLeft"
                when: internal.position === bottomLeftCorner

                PropertyChanges {
                    target: overlay
                    x: iconXOffset
                    y: iconOverlay.height - overlay.height - iconYOffset
                }
            },
            State {
                name: "bottomRight"
                when: internal.position === bottomRightCorner

                PropertyChanges {
                    target: overlay
                    x: iconOverlay.width - overlay.width - iconXOffset
                    y: iconOverlay.height - overlay.height - iconYOffset
                }
            }
        ]
    }

    QtObject {
        id: internal
        property int position: iconOverlay.position >= 0 && iconOverlay.position <= 3 ? iconOverlay.position : defaultPosition
        property bool validIcon: icon != null && icon != undefined
    }

    // DEPRECATED

    property real iconCoverageLimit: 1.0
}
