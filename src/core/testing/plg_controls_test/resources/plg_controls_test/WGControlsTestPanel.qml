import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

/**
 *	Sample panel with all the available controls and styles.
 *	Used for control testing
 */
Rectangle {
    id: mainWindow

    visible: true
    property var title: qsTr("QML Controls Test Window")

    WGTabView {
        anchors.fill: parent

        Tab {
            title: "WGControls 2.0"

            Loader {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                source: "WGControlsTestPanel20.qml"
            }
        }

        Component.onCompleted: {
            currentIndex = 0
        }
    }
}

