import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1

import WGControls 1.0

WGContextArea {
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
