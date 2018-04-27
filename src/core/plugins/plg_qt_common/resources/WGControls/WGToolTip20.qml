import QtQuick 2.5
import QtQuick.Controls.Private 1.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0

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
    property alias text: tooltipBase.text

    /*!
        The time interval in ms for tooltip
    */
    property alias timer: tooltipBase.timer

    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: Qt.NoButton
    propagateComposedEvents: true

    WGToolTipBase {
        id: tooltipBase
        tooltipArea: tooltip
    }
}
