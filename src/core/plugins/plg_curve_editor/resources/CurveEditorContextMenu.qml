import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1

import WGControls 1.0

WGContextArea {
	Action {
		id: zoomExtentsAction
		text: qsTr("Zoom Extents")
		shortcut: "Ctrl+E"
		//iconName: "zoomExtents"
		onTriggered: timeline.zoomExtents()
	}

	Action {
		id: deleteSelectedAction
		text: qsTr("Delete Selected")
		shortcut: StandardKey.Delete
		//iconName: "deleteSelected"
		onTriggered: curveEditor.deleteSelected()
		enabled: curveEditor.selection != undefined
	}

    contextMenu: WGMenu {
        MenuItem {
			action: zoomExtentsAction
		}
		MenuItem {
            action: deleteSelectedAction
        }
   }
}