import QtQuick 2.5
import QtQuick.Controls.Private 1.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Global 2.0

/*!
 \ingroup wgcontrols
 \brief A tooltip triggered by a mouseover on a single global timer.

 Used by WGToolTip

Example:
\code{.js}
WGToolTip {
    tooltipArea: myMouseArea
    text: "This is the description..."
    timer: 500
}
\endcode
*/


Item
{
    id: tooltipBase
    objectName: "WGToolTipBase"
    WGComponent { type: "WGToolTipBase20" }

    /*!
        The value of the tooltip text
    */
    property string text: ""

    /*!
        The title text of the tooltip.
    */
    property string title: ""

    /*!
        The time interval in ms for tooltip
    */
    property int timer: 300

    /*!
        The mousearea that triggers the tooltip
    */
    property MouseArea tooltipArea

    readonly property string tooltipText: title != "" ? "<b>" + title + "</b>" + (text != "" ? "<br>" + text: ""): text

    QtObject {
        id: internal
    }

    Connections {
        target: tooltipArea

        onEntered: {
            WGToolTipTimer.owner = internal
            Tooltip.hideText()
            if (!tooltipArea.pressed && tooltipBase.tooltipText.length)
            {
                timerConnection.target = WGToolTipTimer
                WGToolTipTimer.restartTimer();
            }
        }

        onExited: {
            timerConnection.target = internal
            Tooltip.hideText()
        }
        onCanceled: {
            timerConnection.target = internal
            Tooltip.hideText()
        }
    }

    Connections {
        id: timerConnection
        target: WGToolTipTimer
        ignoreUnknownSignals: true
        onTimerTriggered: {
            if (tooltipArea.containsMouse && !tooltipArea.pressed && tooltipBase.tooltipText.length && WGToolTipTimer.owner === internal)
            {
                Tooltip.showText(tooltipArea, Qt.point(tooltipArea.mouseX, tooltipArea.mouseY), tooltipBase.tooltipText);
            }
        }
    }

    Component.onCompleted: {
        WGToolTipTimer.interval = tooltipBase.timer;
    }
}
