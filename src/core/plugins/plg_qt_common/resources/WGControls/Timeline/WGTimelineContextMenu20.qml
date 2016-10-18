import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import WGControls 2.0

/*!
    \ingroup wgcontrols
    \brief A context right click menu for the Timeline control
*/

WGContextArea {
    WGComponent { type: "WGTimelineContextMenu20" }
    
    Action {
        id: showFullAction
        text: qsTr("Show Full Duration")
        shortcut: "Ctrl+1"
        //iconName: "showFull"
        onTriggered: {
            gridCanvas.viewTransform.xScale = gridCanvas.width * .9
            gridCanvas.viewTransform.origin = Qt.point(gridCanvas.width * .05, gridCanvas.height * .9)
            gridCanvas.requestPaint()
        }
    }

    Action {
        id: selectAllAction
        text: qsTr("Select All Items")
        shortcut: "Ctrl+A"
        //iconName: "selectAll"
        onTriggered: {
            timelineView.selectAll()
            timelineView.selectionChanged()
        }
    }

    contextMenu: WGMenu {
        MenuItem {
            action: showFullAction
        }
        MenuItem {
            action: selectAllAction
        }
   }
}
