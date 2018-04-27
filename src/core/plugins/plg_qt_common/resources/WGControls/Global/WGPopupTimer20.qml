pragma Singleton
import QtQuick 2.5
import WGControls 2.0

Item {
    WGComponent { type: "WGPopupTimer20" }
    id: popupTimer
    property alias interval: timer.interval

    property alias running: timer.running

    signal timerTriggered();

    property var triggeringObject: null

    property bool open: false

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
                popupTimer.timerTriggered()
            }
        }
    }
}
