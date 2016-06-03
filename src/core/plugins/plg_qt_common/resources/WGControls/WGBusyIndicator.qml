import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

//TODO: Style this component

/*!
 \brief An control used to indicate activity. Typically when the UI is blocked.

Example:
\code{.js}
WGBusyIndicator {
    id: busyIndicator1
    z:100
    anchors.centerIn: parent
    running: false
}
\endcode
*/

BusyIndicator {
    objectName: "WGBusyIndicator"
}
