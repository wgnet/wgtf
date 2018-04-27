pragma Singleton
import QtQuick 2.5
import WGControls 2.0

/*  A singleton timer for catching double clicks on Button controls that don't normally
    have a double click event. */

Item {
    WGComponent { type: "WGDoubleClickTimer20" }
    id: dblClickTimer
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
            interval: 200
            onTriggered: {
                dblClickTimer.timerTriggered()
            }
        }
    }
}
