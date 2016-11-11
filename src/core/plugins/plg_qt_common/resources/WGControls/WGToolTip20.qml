import QtQuick 2.5
import QtQuick.Controls.Private 1.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

/*!
 \ingroup wgcontrols
 \brief A tooltip

Example:
\code{.js}
WGToolTip {
    text: "This is the description..."
    timer: 500
}
\endcode
*/


MouseArea
{
    id: tooltip
    objectName: "WGToolTip"
    WGComponent { type: "WGToolTip20" }

    /*!
        The value of the tooltip text
    */
    property string text: ""

    /*!
        The time interval in ms for tooltip
    */
    property alias timer: timer.interval

    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: Qt.NoButton
    propagateComposedEvents: true
    onExited: Tooltip.hideText()
    onCanceled: Tooltip.hideText()

    Timer
    {
        id: timer
        interval: 300
        running: tooltip.containsMouse && !tooltip.pressed && tooltip.text.length
        onTriggered: Tooltip.showText(tooltip, Qt.point(tooltip.mouseX, tooltip.mouseY), tooltip.text);
    }
}
