pragma Singleton
import QtQuick 2.5
import WGControls 2.0

/*  A singleton timer for repeating something over a short interval, such as incrementing a number box. */

Item {
    WGComponent { type: "WGPressAndHoldTimer20" }
    id: pressHoldTimer
    property alias interval: timer.interval

    property alias running: timer.running

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
            interval: 10
            repeat: true
            onTriggered: {
                pressHoldTimer.timerTriggered()
            }
        }
    }
}
