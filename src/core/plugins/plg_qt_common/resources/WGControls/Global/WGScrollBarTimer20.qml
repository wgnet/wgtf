pragma Singleton
import QtQuick 2.5
import WGControls 2.0

/*  A singleton timer for shrinking scrollbar widths */

Item {
    WGComponent { type: "WGScrollBarTimer20" }
    id: scrollbarTimer
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
            interval: 600
            onTriggered: {
                scrollbarTimer.timerTriggered()
            }
        }
    }
}
