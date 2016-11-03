import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGControls.Layouts 1.0


Rectangle {
    id: root
    WGComponent { type: "progress_manager" }

    color: palette.mainWindowColor
    width: 500
    height: 100

    property alias progressValue: root.progValue
    property alias progressCommandId: root.commandId
    property int progValue: 0
    property string commandId: "Progress Manager..."

    signal progressCancelled(bool cancelled)

    WGScrollPanel {
        childObject_ :
        WGDraggableColumn {
            WGInternalPanel {
                text: "Command In Progress"
                hasIcon_: false
                collapsible_: false
                childObject_ :

                WGColumnLayout {
                    id: columnLayout

                    WGProgressControl {
                        Layout.fillWidth: true
                        fakeProgress: false
                        text: commandId
                        units: "%"
                        value: progValue
                        maximumValue: 100

                        onProgressEnded: {
                            if (!completed)
                            {
                                // The user has cancel the command if we get the progress ended event with the completed flag off.
                                // Let the Progress Manager know the progress has been cancelled.
                                root.progressCancelled(true)
                            }
                        }
                    }
                }
            }
        }
    }

}
