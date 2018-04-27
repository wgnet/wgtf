pragma Singleton
import QtQuick 2.5
import WGControls 2.0

Item {
    WGComponent { type: "WGToolTipTimer20" }
    id: tipTimer
    property alias interval: timer.interval

    property alias running: timer.running

    property var owner: undefined

    signal timerTriggered();

    function restartTimer() {
        timer.restart();
    }

    function stopTimer() {
        timer.stop();
    }

    QtObject {
        id: internal
        property Timer timer: Timer {
            id: timer
            interval: 300
            onTriggered: {
                tipTimer.timerTriggered()
            }
        }
    }
}
